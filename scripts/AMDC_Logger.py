import time
import numpy as np
import pandas as pd
import pathlib as pl
import os
import datetime
import struct

class AMDC_Logger():
    
    max_vars = 32
    
    def __init__(self, AMDC, mapfile):
        
        self.amdc = AMDC
        self.mapfile = Mapfile(mapfile)
        self.log_vars = []
    
    def register(self, name, samples_per_sec = 1000, var_type = 'double'):
        
        names = name.split()
        
        for name in names:
            
            if not ('LOG_' in name):
                
                name = 'LOG_' + name
            
            if len(self.log_vars) < self.max_vars:
                
                memory_addr = int(self.mapfile.address(name), 0)
                
                if memory_addr == 0:
                    print(f"ERROR: couldn't find memory address for '{name}'")
                
                else:
                    if not (name in self.log_vars):
                        self.log_vars.append(name)
                        idx = len(self.log_vars) - 1
                        cmd = f'log reg {idx} {name} {memory_addr} {samples_per_sec} {var_type}'
                        self.amdc.cmd(cmd)
            
            else:
                print(f'Did not register, cannot exceed maximum of {max_vars} logged variables')
                print(f'Logged variables are:\n{self.log_vars}')
        
                
    def auto_register(self, root, samples_per_sec = None):
        
        #provide path to root of C code for which you want the logger to search for variables
        #of the form `LOG_{var_name}`
               
        log_vars, log_types = self.auto_find_vars(root)
        
        #register all found variables
        for var, tp in zip(log_vars, log_types):
            if samples_per_sec != None:
                self.register(name = var, samples_per_sec = samples_per_sec, var_type = tp)
            else:
                self.register(name = var, var_type = tp)
                
    def auto_find_vars(self, root):
        
        log_vars = []
        log_types = []
        
        #extract all valid logging variables
        for path, dirs, files in os.walk(root):
            for file in files:
                p = pl.Path(path) / file
                if p.suffix == '.c':
                    lv, lt = self.find_log_vars(p)
                    for var, tp in zip(lv, lt):
                        log_vars.append(var)
                        log_types.append(tp)
                        
        return log_vars, log_types
        
        
    @staticmethod
    def find_log_vars(file):
        
        valid_types = 'int double float char'.split()
        log_vars = []
        log_types = []
        
        with open(file) as f:
            
            for line in f:
                if 'LOG_' in line:
                    lst = line.split()
                    if lst[0] in valid_types:
                        log_types.append(lst[0])
                        log_vars.append(lst[1])
            
        return log_vars, log_types
        
    def start(self):
        self.amdc.cmd('log start')
        
    def stop(self):
        self.amdc.cmd('log stop')
        
    def log(self, duration = 0.25):
        
        old_delay = self.amdc.cmdDelay #store old time delay
        self.amdc.cmdDelay = 0.01 #temporarily set delay between commands to shorter
        
        self.start()
        time.sleep(duration)
        self.stop()
        
        self.amdc.cmdDelay = old_delay #reset cmd delay to previous value
        
        
    def dump(self, file = None, timestamp = True, timestamp_fmt = '%Y-%m-%d_H%H-M%M-S%S'):
        if len(self.log_vars) > 0:
            for i, var in enumerate(self.log_vars):
                
                if i == 0:
                    df = self.dump_single_bin(var)
                else:
                    df = df.join(self.dump_single_bin(var), how = 'outer')
                    
            if file != None:
                
                #ensure that file is pathlib path
                file = pl.Path(file)
                p = file.parent
                
                if timestamp:
                    path = p / (file.stem + '_' + datetime.datetime.now().strftime(timestamp_fmt) + '.csv')
                else:
                    path = p / (file.stem + '.csv')
            
                df.to_csv(path)
                    
            return df
        
        else:
            return None

    def dump_single_bin(self, var):
        try:
            log_var_idx = self.log_vars.index(var)
        except:
            print('ERROR: Invalid variable name')
            return None
        
        # Don't automatically capture binary output data! 
        old_state = self.amdc.captureOutput
        self.amdc.captureOutput = False

        # Start dumping to host
        self.amdc.cmd(f"log dump bin {log_var_idx}")    
        
        # Reset the previous state
        self.amdc.captureOutput = old_state

        # These are repeating 4 times in the binary stream!
        MAGIC_HEADER = 0x12345678
        MAGIC_FOOTER = 0x11223344

        magic_header_idx = 0
        magic_footer_idx = 0

        dump_data = bytearray()
        dump_data_idx = 0

        start_time = time.time()
        timeout_sec = 60 # about 1kSPS

        found_footer = False
        while not found_footer:   
            # Read in all serial data from OS buffer
            out = bytes(self.amdc.ser.read_all())
            dump_data += out

            N = len(out)

            s = struct.Struct('<IIII')    

            for i in range(0,N-s.size):
                magic = s.unpack(out[i:i+s.size])

                if magic[0] == MAGIC_HEADER and magic[1] == MAGIC_HEADER and magic[2] == MAGIC_HEADER and magic[3] == MAGIC_HEADER:
                    magic_header_idx = dump_data_idx

                if magic[0] == MAGIC_FOOTER and magic[1] == MAGIC_FOOTER and magic[2] == MAGIC_FOOTER and magic[3] == MAGIC_FOOTER:
                    magic_footer_idx = dump_data_idx
                    found_footer = True
                    break

                dump_data_idx += 1

            if not found_footer:
                # Sleep for 100 ms to let OS do other work if needed
                time.sleep(0.1)

                # Break loop if timeout
                if time.time() >= start_time + timeout_sec:
                    print("ERROR: couldn't find magic footer!")
                    break

        end_time = time.time()

        # Flush the host OS to make sure all
        # serial data is out of buffers
        # (this might be necessary to get last sample value!)
        for i in range(0,10):
            out = bytes(self.amdc.ser.read_all())
            dump_data += out

        print("magic_header_idx:", magic_header_idx)
        print("magic_footer_idx:", magic_footer_idx)

        print("Dump took:", '{:f}'.format(end_time - start_time), " sec")
        
        N = len(dump_data)
        bout = bytes(dump_data)

        metadata_start_idx = magic_header_idx+(4*4)

        s = struct.Struct("<II")
        unpacked_header = s.unpack(bout[metadata_start_idx:metadata_start_idx+s.size])
        num_samples  = unpacked_header[0]
        data_type    = unpacked_header[1]

        print("num samples:", hex(num_samples), num_samples)
        print("data type:", hex(data_type), data_type)
        
        dump_samples_idx = metadata_start_idx+s.size+1

        if data_type == 1:
            s = struct.Struct('<Ii')
        elif data_type == 2 or data_type == 3:
            s = struct.Struct('<If')
        else:
            print("ERROR: unknown data type!")

        samples = []

        for i in range(0,num_samples):   
            sample = s.unpack(bout[dump_samples_idx:dump_samples_idx+s.size])
            dump_samples_idx += s.size

            ts  = sample[0]/1e6
            val = sample[1]

            # FIXME(NP): we shouldn't need to do this.... :(
            if i != 0:
                samples.append([ts, val])
        
        # Convert to DataFrame

        arr = np.array(samples)
        df = pd.DataFrame(data = arr, columns = ['t', var[4::]])
        df['t'] = df['t'] - df['t'].min()
        df.set_index('t', inplace = True)

        return df
        
    def dump_single_text(self, var):
        
        try:
            log_var_idx = self.log_vars.index(var)
        except:
            print('ERROR: Invalid variable name')
            return None
        
        #we want to set the print state to false so we don't
        #print all of our data to the screen
        old_state = self.amdc.printOutput
        self.amdc.printOutput = False
        
        self.amdc.cmd(f"log dump text {log_var_idx}")
        
        samples = []
        
        line = ""
        while True:
            c = self.amdc.ser.read().decode()
            line += c
            
            if ("\n" in c):
                try:
                    sample = self._process_line(line)
                    samples.append(sample)
                except:
                    # Line was not a sample
                    # which is okay, just do nothing
                    pass

                if ("-------END-------" in line):
                    # Flush the rest of the RX line
                    for j in range(10000):
                        self.amdc.ser.read()
                    
                    # End the outer for loop
                    break
            
                line = ""

        arr = np.array(samples)
        df = pd.DataFrame(data = arr, columns = ['t [s]', var[4::]])
        df['t [s]'] = df['t [s]'] - df['t [s]'].min()
        
        df.set_index('t [s]', inplace = True)
        
        self.amdc.printOutput = old_state #reset the print state

        return df
            
    def clear(self, var):
        
        try:
            log_var_idx = self.log_vars.index(var)
        except:
            print('ERROR: Invalid variable name')
            return None
        
        self.amdc.cmd(f'log empty {log_var_idx}')
        
    def clear_all(self):
        
        for i, var in enumerate(self.log_vars):
            if i == 0:
                self.clear(var)
            else:
                time.sleep(3)
                self.clear(var)

    @staticmethod   
    def _process_line(line):
        colline = ' '.join(line.split())
        
        # Don't process empty lines
        if (len(colline) == 0):
            raise Exception()

        # Make sure it is a data line
        if (colline[0] != ">"):
            raise Exception()

        args = colline.split(' ')

        ts  = float(int(args[1])) / 1e6
        v   = float(args[2])

        if (ts != 0.0):
            return [ts, v]
        
        raise Exception()


class Mapfile:
    
    def __init__(self, filepath):
        
        self.filepath = filepath

    def address(self, var_name):
        
        with open(self.filepath, 'r') as fp:
            line = fp.readline()
            
            while line:
                colline = ' '.join(line.split())
                
                # Don't process empty lines
                if len(colline) == 0:
                    line = fp.readline()
                    continue
                
                tokens = colline.split(' ')
                
                # Only process lines which could be of interest
                if len(tokens) != 2:
                    line = fp.readline()
                    continue
                
                # Check if we are at the line of interest
                if tokens[1] == var_name:
                    return tokens[0]
                    break
                
                line = fp.readline()
                
            return "0x0"
        
def main():
    
    pass    

if __name__ == '__main__': main()