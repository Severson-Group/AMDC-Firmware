import serial
import time


class AMDC:
    def __init__(self,
                 port,
                 baudrate='115200',
                 cmdDelay=0.2,
                 cmdDelayChar=0.001,
                 cmdEcho=True,
                 cmdEchoPrepend="\t> ",
                 printOutput=True,
                 outputPrepend="",
                 captureOutput=True):

        # Serial port configuration
        self.ser = serial.Serial(timeout=0)
        self.ser.baudrate = baudrate
        self.ser.port = port

        # Command config
        self.cmdDelay = cmdDelay
        self.cmdDelayChar = cmdDelayChar
        self.cmdEcho = cmdEcho
        self.cmdEchoPrepend = cmdEchoPrepend
        self.printOutput = printOutput
        self.outputPrepend = outputPrepend
        self.captureOutput = captureOutput

    def connect(self):
        self.ser.open()

    def disconnect(self):
        self.ser.close()

    def __enter__(self):
        self.connect()
        return self
    
    def print_outputs(self, val = True):
        self.printOutput = val
        
    def print_cmds(self, val = True):
        self.cmdEcho = val
        
    def print_all(self, val = True):
        self.print_outputs(val)
        self.print_cmds(val)

    def __exit__(self, type, value, traceback):
        self.disconnect()

    def cmd(self, cmd_str):
        to_send_str = f"{cmd_str}\n"
        to_send_bytes = str.encode(to_send_str)
        for b in to_send_bytes:
            self.ser.write(bytes([b]))

            # Pause between letters so we don't send data too fast
            time.sleep(self.cmdDelayChar)

        # Wait for cmd to execute on AMDC
        time.sleep(self.cmdDelay)

        # Print log for user
        if self.cmdEcho:
            print(f"{self.cmdEchoPrepend}{cmd_str}")

        if self.captureOutput:
            #Print out any feedback from command
            output = []  #empty array for output that's returned from command
            count_empty = 0  #number of empty lines in a row
            allowed_empty = 10  #the code will keep reading lines until it reaches
            #this many consecutive blank lines
            while count_empty < allowed_empty:

                line = self.ser.readline().decode()  #read in line and decode

                if len(
                        line
                ) > 0 and line != '\n':  #if line is not empty and not just new line
                    line = line.strip(
                        '\n\r')  #remove newline and carriage returns
                    output.append(line)  #append line to output list
                    count_empty = 0
                else:
                    count_empty += 1

            if self.printOutput:
                print(f"{self.outputPrepend}{output}")

            return output


def main():

    pass


if __name__ == '__main__': main()
