import serial
import socket
import select
import time
    
class AMDC:
    def __init__(self):
        self.comm_is_inited = False

        # Set default comms.
        # User will probably override
        self.setup_comm_defaults()
    

    def setup_comm_defaults(self, comm_method='uart'):
        valid_comm_methods = ['uart', 'serial', 'eth', 'ethernet']
        if comm_method not in valid_comm_methods:
            raise CommMethodUnknownException('Valid methods: ', valid_comm_methods)

        self.comm_cmd_cmd_print = True
        self.comm_cmd_cmd_print_prepend = "\t> "

        self.comm_cmd_resp_capture = True
        self.comm_cmd_resp_print = True
        self.comm_cmd_resp_print_prepend = ""

        if comm_method in ['uart', 'serial']:
            self.comm_cmd_delay_cmd = 0.001 # [sec]
        elif comm_method in ['eth', 'ethernet']:
            self.comm_cmd_delay_cmd = 0.001 # [sec]


    def uart_init(self, port, baudrate = '115200'):
        self.comm_method = 'uart'
        
        self.comm_uart_ser = serial.Serial(timeout = 0)
        self.comm_uart_ser.baudrate = baudrate
        self.comm_uart_ser.port = port

        self.comm_is_inited = True


    def eth_init(self, amdc_ip_addr = '192.168.1.10', amdc_ip_port = 7):
        self.comm_method = 'eth'

        self.comm_eth_amdc_ip_addr = amdc_ip_addr
        self.comm_eth_amdc_ip_port = amdc_ip_port

        self.comm_eth_default_ascii_cmd_socket = None
        self.comm_eth_sockets = []

        self.comm_is_inited = True


    def eth_new_socket(self, socket_type):
        if not self.comm_is_inited:
            raise Exception('Comm not initialized')

        if self.comm_method is not 'eth':
            raise Exception('Comm not set up for eth')

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)        
        s.connect((self.comm_eth_amdc_ip_addr, self.comm_eth_amdc_ip_port))
        
        s.setblocking(1)
        
        # Upon connection, the AMDC tells the host the socket ID
        # by sending back 1 byte with the ID
        #
        # Endianness does not matter; only one byte!
        sock_id = int.from_bytes(s.recv(1), "little")
        
        # Tell AMDC what type of socket this is
        if socket_type in ['ascii_cmd']:
            s.send(bytearray([12, 34]))
        elif socket_type in ['log_var']:
            s.send(bytearray([56, 78]))
        else:
            raise InvalidSocketTypeExeption('Valid types:', 'ascii_cmd', 'log_var')
        
        self.comm_eth_sockets.append(s)
        
        return (s,sock_id)


    def eth_set_default_ascii_cmd_socket(self, s):
        self.comm_eth_default_ascii_cmd_socket = s


    def connect(self):
        if not self.comm_is_inited:
            raise Exception('Comms not initialized')
        else:
            if self.comm_method is 'uart':
                self.comm_uart_ser.open()

            if self.comm_method is 'eth':
                # Sockets are connected at time of creation,
                # so do nothing here...
                pass


    def disconnect(self):
        if not self.comm_is_inited:
            raise Exception('Comms not initialized')
        else:
            if self.comm_method is 'uart':
                self.comm_uart_ser.close()

            if self.comm_method is 'eth':
                for s in self.comm_eth_sockets:
                    s.close()
        
    
    def __enter__(self):
        self.connect()
        return self


    def __exit__(self, type, value, traceback):
        self.disconnect()

    @staticmethod
    def try_read_sock(sock, num_bytes):
        incoming_data = bytes()
        
        try:
            sock.setblocking(0) 
            incoming_data = sock.recv(num_bytes)
            sock.setblocking(1)
        except BlockingIOError:
            # Nothing was available, this is ok
            pass

        return incoming_data
        
        
    def read(self, num_bytes):
        incoming_data = bytes()
        
        try:
            if self.comm_method is 'uart':
                incoming_data = self.comm_uart_ser.read(num_bytes)
            elif self.comm_method is 'eth':
                self.comm_eth_default_ascii_cmd_socket.setblocking(0) 
                incoming_data = self.comm_eth_default_ascii_cmd_socket.recv(num_bytes)
                self.comm_eth_default_ascii_cmd_socket.setblocking(1)
        except BlockingIOError:
            # Nothing was available, this is ok
            pass

        return incoming_data


    def cmd(self, cmd_str, timeout_sec = 1):
        to_send_str = f"{cmd_str}\n"
        to_send_bytes = str.encode(to_send_str)

        if self.comm_method is 'uart':
            for b in to_send_bytes:
                self.comm_uart_ser.write(bytes([b]))
        elif self.comm_method is 'eth':
            self.comm_eth_default_ascii_cmd_socket.send(to_send_bytes)
        
        # Wait for cmd to execute on AMDC
        if self.comm_cmd_delay_cmd > 0:
            time.sleep(self.comm_cmd_delay_cmd)
        
        # Print command we just sent
        if self.comm_cmd_cmd_print:
            print(f"{self.comm_cmd_cmd_print_prepend}{cmd_str}")

        # Optionally listen for response
        if self.comm_cmd_resp_capture:
            retdata = bytearray()

            if self.comm_method is 'eth':
                self.comm_eth_default_ascii_cmd_socket.setblocking(0)
            
            looking_for_cmd_resp_code = True

            start_time = time.time()
            while (looking_for_cmd_resp_code):
                try:
                    if self.comm_method is 'uart':
                        incoming_data = self.comm_uart_ser.read(4096)
                    elif self.comm_method is 'eth':
                        incoming_data = self.comm_eth_default_ascii_cmd_socket.recv(4096)

                    retdata.extend(incoming_data)
                except BlockingIOError:
                    # Could not read enough data
                    pass

                if len(retdata) > 0:
                    # Convert byte array into chars
                    retdata_chars = retdata.decode("utf-8").split('\r\n')
                    output = []
                    for d in retdata_chars:
                        d = d.strip('\n\r')
                        if len(d) > 0:
                            output.append(d)

                    # Only read until valid response!
                    valid_cmd_resp = ['SUCCESS', 'FAILURE', 'INVALID ARGUMENTS', 'INPUT TOO LONG', 'UNKNOWN CMD', 'UNKNOWN ERRROR']
                    for valid_resp_str in valid_cmd_resp:
                        for line in output:
                            if line.find(valid_resp_str) != -1:
                                # Found a valid response!
                                looking_for_cmd_resp_code = False
                                break
                        
                        if not looking_for_cmd_resp_code:
                            break

                # Break loop if timeout
                if time.time() >= start_time + timeout_sec:
                    raise Exception("ERROR: timeout, could not find command response!")

            if self.comm_method is 'eth':
                self.comm_eth_default_ascii_cmd_socket.setblocking(1)

            # Convert byte array into chars
            retdata_chars = retdata.decode("utf-8").split('\r\n')
            output = []
            for d in retdata_chars:
                d = d.strip('\n\r')
                if len(d) > 0:
                    output.append(d)

            if self.comm_cmd_resp_print:
                print(f"{self.comm_cmd_resp_print_prepend}{output}")

            return output



def main():
    # Example script to show how to configure either the UART or Ethernet interface
    # to the AMDC hardware. After configuration, both interfaces look the same to
    # the user and can be used without knowing which method is active!

    if 1:
        # Option 1 - UART:
        amdc = AMDC()
        amdc.setup_comm_defaults('uart')
        amdc.uart_init('COM6')
    else:
        # Option 2 - Ethernet
        amdc = AMDC()
        amdc.setup_comm_defaults('eth')
        amdc.eth_init()
        s1 = amdc.eth_new_socket('ascii_cmd')
        amdc.eth_set_default_ascii_cmd_socket(s1)

    # Now, same interface for both UART and Ethernet
    amdc.connect()
    amdc.cmd('hw anlg read 0')
    amdc.cmd('hw anlg read 1')
    amdc.disconnect()
    
if __name__ == '__main__': main()