from AMDC import AMDC
from AMDC_Logger import AMDC_Logger

from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
from threading import Thread
import numpy as np
import time
import struct

class AMDC_Log_Stream_Parser:
    def __init__(self, logger):
        self.logger = logger
        
        # Internal store of bytes
        # We will append onto this
        self.buffer = bytes()
        
    def consume_bytes(self, in_bytes):
        self.buffer += in_bytes
        
        # Per the AMDC C-code,
        # 
        # Packet format: HEADER, VAR_SLOT, TS, DATA, FOOTER
        # where each entry is 32 bits

        # Total packet length: 5*4 = 20 bytes

        # HEADER = 0x11111111
        # FOOTER = 0x22222222          
    
        s = struct.Struct('<IIIII')

        HEADER = 0x11111111
        FOOTER = 0x22222222

        output_vars_idx = []
        output_vars_ts = []
        output_vars_data = []

        # Record how far we consumed into the buffer
        consumed_up_to = 0
        
        for i in range(0,len(self.buffer) - s.size):
            potential_packet = s.unpack(self.buffer[i:i+s.size])

            if (potential_packet[0] == HEADER) and (potential_packet[4] == FOOTER):
                consumed_up_to = i+s.size
                
                var_idx = potential_packet[1]
                ts = potential_packet[2]
                data = potential_packet[3]

                # Find LV entry
                LV = None
                for v in self.logger.log_vars:
                    LV = self.logger.log_vars[v]
                    if LV.index == var_idx:
                        break        

                if LV.var_type == 'int':
                    s2 = struct.Struct('<i')
                elif LV.var_type == 'float' or LV.var_type == 'double':
                    s2 = struct.Struct('<f')

                data = s2.unpack(data.to_bytes(4, 'little'))[0]

                # Convert ts to seconds (comes as usec)
                ts = ts * 1e-6

                output_vars_idx.append(var_idx)
                output_vars_ts.append(ts)
                output_vars_data.append(data)
                
                
        # Throw away the consumed data from the internal buffer
        self.buffer = self.buffer[consumed_up_to:]     
        
        return (output_vars_ts, output_vars_data)
                

class AMDC_LivePlot:
    # This class is based on code from:
    # https://stackoverflow.com/a/53952210
    
    def __init__(self, logger, vars_to_stream, update_interval_ms = 100, window_sec = 1):
        self.logger = logger
        self.vars_to_stream = AMDC_Logger._sanitize_inputs(vars_to_stream)
        
        if len(self.vars_to_stream) > 1:
            raise Exception('Currently only supports a single variable per stream')
        
        self.is_streaming = False
        self.stream_parser = AMDC_Log_Stream_Parser(self.logger)
        
        self.x_data = []
        self.y_data = []
        
        self.figure = plt.figure()
        self.line, = plt.plot(self.x_data, self.y_data)
        self.text = self.figure.gca().text(0,0, "", va="bottom", ha="left")
        self.animation = FuncAnimation(self.figure, self.update, interval=update_interval_ms)
        
        self.th = Thread(target=self.thread_worker, args=(window_sec,), daemon=True)
        self.th.start()

        self.cid = self.figure.canvas.mpl_connect('resize_event', self.on_event)

    def on_event(self, event):
        self.text = "HELLO TEST!!!"
        print('event:',event)

#     def sync_stream(self):
#         if not self.is_streaming:
#             raise Exception('Not streaming yet')
#        
#         # Resets time for streaming variables
#         self.logger.amdc.cmd('log stream synctime')
        
    def start_stream(self):
        if self.is_streaming:
            raise Exception('Already streaming')
        
        # Make a new socket for the stream
        self.socket, self.socket_id = self.logger.amdc.eth_new_socket('log_var')
        
        for var_to_stream in self.vars_to_stream:
            LV = self.logger.log_vars[var_to_stream]
            
            # Start streaming this LV to the socket
            self.logger.amdc.cmd('log stream start {} {}'.format(LV.index, self.socket_id))
        
        self.is_streaming = True
        
#         # Sync stream timing
#         #
#         # Required since we started streaming from the AMDC
#         # in a "wave" -- each cmd above happened at different times
#        
#         self.sync_stream()
#        
#         # Read data to throw out first few samples before we synced the stream
#        
#         for i in range(0, 100000):
#             AMDC.try_read_sock(self.socket, 8)
#        
#        
#         # NOTE: this is commented out since we decided to only
#         # support 1 variable per stream... So, sync is not an issue!
        
        
    def stop_stream(self):
        if not self.is_streaming:
            raise Exception('Not streaming yet')
        
        for var_to_stream in self.vars_to_stream:
            LV = self.logger.log_vars[var_to_stream]
            
            # Start streaming this LV to the socket
            self.logger.amdc.cmd('log stream stop {} {}'.format(LV.index, self.socket_id))
        
        # Close our socket
        self.socket.close()
        self.is_streaming = False
        
    def update(self, frame):
        self.line.set_data(self.x_data, self.y_data)
        self.figure.gca().relim()
        self.figure.gca().autoscale_view()
        return self.line,

    def show(self):
        plt.show()

    def thread_worker(self, window_sec):
        while True:
            if self.is_streaming:
                # Get the latest data streamed from the AMDC
                #
                # Up to 10 kB, but this is non-blocking, so might return 0 bytes
                #
                # Tried with smaller buffer, but then overruns on the AMDC TCP/IP stack
                # since the host is not pulling out the data fast enough
                try:
                    in_bytes = AMDC.try_read_sock(self.socket, 10*1024)
                except:
                    # Socket is closed, which means we should kill ourselves
                    break

                # Parse data
                new_sample_times, new_sample_data = self.stream_parser.consume_bytes(in_bytes)            

                # Append data to CSV file
                # ...
                
                # Update plot data source
                if len(new_sample_times) > 0:
                    self.x_data += new_sample_times
                    self.y_data += new_sample_data
                    
                    # Trim data to only show latest X seconds
                    if 1:
                        xx = np.array(self.x_data)
                        yy = np.array(self.y_data)

                        xx_idxs = xx.argsort()
                        xx_sorted = xx[xx_idxs]
                        yy_sorted = yy[xx_idxs]

                        filter_arr = xx_sorted > np.max(xx_sorted)-window_sec
                        xx_new = xx_sorted[filter_arr]
                        yy_new = yy_sorted[filter_arr]

                        self.x_data = xx_new.tolist()
                        self.y_data = yy_new.tolist()
            
            # Sleep for a short period
            #
            # Found that if we do not sleep, Jupyter falls apart
            # Even though this is another thread (?)
            time.sleep(0.001)