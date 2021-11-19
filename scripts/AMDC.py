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
            self.comm_cmd_delay_cmd = 0.2 # [sec]
            self.comm_cmd_delay_cmd_char = 0.001 # [sec]

        if comm_method in ['eth', 'ethernet']:
            self.comm_cmd_delay_cmd = 0.001 # [sec]
            self.comm_cmd_delay_cmd_char = 0 # [sec]


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
        
        # Tell AMDC what type of socket this is
        if socket_type in ['ascii_cmd']:
            s.send(bytearray([12, 34]))
        elif socket_type in ['log_var']:
            s.send(bytearray([56, 78]))
        else:
            raise InvalidSocketTypeExeption('Valid types:', 'ascii_cmd', 'log_var')
        
        self.comm_eth_sockets.append(s)
        return s


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


    def cmd(self, cmd_str):
        to_send_str = f"{cmd_str}\n"
        to_send_bytes = str.encode(to_send_str)
        for b in to_send_bytes:
            if self.comm_method is 'uart':
                self.comm_uart_ser.write(bytes([b]))
            
            if self.comm_method is 'eth':
                self.comm_eth_default_ascii_cmd_socket.send(bytes([b]))
            
            # Pause between letters so we don't send data too fast
            if self.comm_cmd_delay_cmd_char > 0:
                time.sleep(self.comm_cmd_delay_cmd_char)
        
        # Wait for cmd to execute on AMDC
        if self.comm_cmd_delay_cmd > 0:
            time.sleep(self.comm_cmd_delay_cmd)
        
        # Print command we just sent
        if self.comm_cmd_cmd_print:
            print(f"{self.comm_cmd_cmd_print_prepend}{cmd_str}")

        # Optionally listen for response
        if self.comm_cmd_resp_capture:
            if self.comm_method is 'uart':
                output = []

                # Number of empty lines in a row
                count_empty = 0

                # Keep reading lines until we get this many consecutive blank lines
                allowed_empty = 10 
                                    
                while count_empty < allowed_empty:
                    # Read in line and decode
                    line = self.comm_uart_ser.readline().decode()

                    if len(line) > 0 and line != '\n':
                        line = line.strip('\n\r')
                        output.append(line)
                        count_empty = 0
                    else:
                        count_empty += 1
        
            if self.comm_method is 'eth':
                retdata = bytearray()

                self.comm_eth_default_ascii_cmd_socket.setblocking(0)
                
                timeout_sec = 0.010
                num_consecutive_no_data = 50
                i = 0
                while (i < num_consecutive_no_data):
                    inputs = [self.comm_eth_default_ascii_cmd_socket]
                    outputs = []

                    readable, writable, exceptional = select.select(inputs, outputs, inputs, timeout_sec)
                    if len(readable) > 0:
                        incoming_data = self.comm_eth_default_ascii_cmd_socket.recv(4096)
                        retdata.extend(incoming_data)
                        i = 0
                    else:
                        # No new data after the timeout
                        i += 1

                self.comm_eth_default_ascii_cmd_socket.setblocking(1)

                # Convert byte array into chars
                retdata = retdata.decode("utf-8").split('\r\n')
                output = []
                for d in retdata:
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