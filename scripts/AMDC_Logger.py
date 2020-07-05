import time
import numpy as np
import pandas as pd
import pathlib as pl
import os
import datetime
import struct

#########################################################
# Title:       AMDC Logging code
# Author(s):   Nicholas Hemenway, Nathan Petersen
# Date:        06/30/2020
#########################################################

class AMDC_Logger():
    
    default_max_slots = 32
    
    def __init__(self, AMDC, mapfile):
        
        self.amdc = AMDC
        self.mapfile = Mapfile(mapfile)
        
        self.available_indices = list(range(AMDC_Logger.default_max_slots))[-1::-1]
        self.log_vars = []
        self.log_var_indices = []
        
    def info(self):
        
        old_state = self.amdc.cmdEcho
        self.amdc.cmdEcho = False
        
        max_slots, names, types, indices, sample_rates, num_samples = self._get_amdc_state()
        
        N = 15
        
        title = 'AMDC LOGGER INFO'.center(22).center(60, '#') + '\n\n'
        header = 'Variable Name:'.ljust(N) + 'Index:'.center(N) + 'Type:'.center(N) + 'Sample Rate [Hz]:'.center(N + 10) + 'Number of Samples:'
        
        print(title + header)
        
        for (var, idx, t, sps, samples) in zip(names, indices, types, sample_rates, num_samples):
            line = f'{var}'.ljust(N) + f'{idx}'.center(N) + f'{t}'.center(N) + f'{sps}'.center(N + 10) + f'{samples}'.center(N)
            print(line)
            
        self.amdc.cmdEcho = old_state

    def sync(self):
        
        #the AMDC is the source of truth so calling sync will synchronize python
        #with the amdc. this is useful if the python kernel is restarted
        
        max_slots, names, types, indices, sample_rates, num_samples = self._get_amdc_state()
        self.max_slots = max_slots

        self._reset()
        
        #we don't actually register the variables because they are already registered
        for var, idx in zip(names, indices):
            self.log_vars.append(var)
            self.log_var_indices.append(idx)
            
            #remove index from list of available indices
            loc = self.available_indices.index(idx)
            self.available_indices.pop(loc)
        
    def register(self, log_vars, samples_per_sec = 1000, var_type = 'double'):
        
        names = self._sanitize_inputs(log_vars)
        
        for name in names:
            
            if len(self.available_indices) > 0:
                
                memory_addr = int(self.mapfile.address(name), 0)
                
                if memory_addr == 0:
                    print(f"ERROR: couldn't find memory address for '{name}'")
                
                else:
                    if not (name in self.log_vars):
                        self.log_vars.append(name)
                        idx = self.available_indices.pop() #extract next available index
                        self.log_var_indices.append(idx)
                        cmd = f'log reg {idx} {name} {memory_addr} {samples_per_sec} {var_type}'
                        out = self.amdc.cmd(cmd)
                        if out[1] == 'FAILURE':
                            self.unregister(name, send_cmd = False)
            
            else:
                print(f'Did not register, cannot exceed maximum of {self.max_slots} logged variables')
                print(f'Logged variables are:\n{self.log_vars}')
                
    def auto_register(self, root, samples_per_sec = None):
        
        #provide path to root of C code for which you want the logger to search for variables
        #of the form `LOG_{var_name}`
               
        log_vars, log_types = self.auto_find_vars(root)
        
        print(f'Total Number of Variables to be Logged: {len(log_vars)}')
        
        #register all found variables
        for var, tp in zip(log_vars, log_types):
            if samples_per_sec != None:
                self.register(log_vars = var, samples_per_sec = samples_per_sec, var_type = tp)
            else:
                self.register(log_vars = var, var_type = tp)
                
    def unregister(self, log_vars, send_cmd = True):
        
        variables = self._sanitize_inputs(log_vars)
        
        for var in variables:
            try:
                loc = self.log_vars.index(var) #find location of var in list of vars
            except:
                print('ERROR: Invalid variable name')
            else:
                self.log_vars.pop(loc)
                idx = self.log_var_indices.pop(loc)
                self.available_indices.append(idx)
                self.available_indices.sort(reverse = True)
                
                if send_cmd:
                    self.amdc.cmd(f'log unreg {idx}')
    
    def unregister_all(self):
        
        #we have to put log_vars into a local variable because self.log_vars changes
        #as we iterate over it
        variables = self.log_vars.copy()
        
        for var in variables:
            self.unregister(var)    
                
    def clear(self, var):
        
        log_var_idx = self._get_index(var)
        self.amdc.cmd(f'log empty {log_var_idx}')
        
    def clear_all(self):
        
        for var in self.log_vars:
            self.clear(var)
        
    def auto_find_vars(self, root):
        
        log_vars = []
        log_types = []
        
        #extract all valid logging variables
        for path, dirs, files in os.walk(root):
            for file in files:
                p = pl.Path(path) / file
                if p.suffix == '.c':
                    lv, lt = self._find_log_vars(p)
                    for var, tp in zip(lv, lt):
                        log_vars.append(var)
                        log_types.append(tp)
                        
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
        
        
    def dump(self, log_vars = None, file = None, comment = '', timestamp = True, timestamp_fmt = '%Y-%m-%d_H%H-M%M-S%S', how = 'binary', max_tries = 4):
              
        if log_vars is not None:
            variables = self._sanitize_inputs(log_vars)
        else:
            variables = self.log_vars
        
        if how.lower() == 'ascii':
            dump_func = self._dump_single_text
        elif how.lower() == 'binary':
            dump_func = self._dump_single_bin
        else:
            raise Exception("Invalid type of dump, valid types are: 'binary' or 'ascii'")
        
        tries = 0
        
        if len(variables) > 0:
            for i, var in enumerate(variables):
                
                if var in self.log_vars:
                    
                    #try multiple times to load data if it fails
                    while tries < max_tries:
                        try:
                            df_new = dump_func(var)
                        except Exception as e:
                            print(e)
                            
                            tries += 1
                            if tries < max_tries:
                                print(f'failed loading {var}... retrying\n')
                        else:
                            
                            if i == 0:
                                df = df_new
                            else:
                                df = df.join(df_new, how = 'outer')
                                
                            tries = 0
                            break
                    
                    if tries > 0:
                        raise Exception('Loading Data Failed')

            if file != None:
                
                #ensure that file is pathlib path
                file = pl.Path(file)
                p = file.parent
                
                if not p.exists():
                    p.mkdir()
                
                if timestamp:
                    path = p / (file.stem + '_' + datetime.datetime.now().strftime(timestamp_fmt) + '.csv')
                else:
                    path = p / (file.stem + '.csv')
                    
                if len(comment) > 0:
                    
                    if comment[0] != '#':
                        comment = '#' + comment
                    if comment[-1] != '\n':
                        comment += '\n'
                        
                    with open(path, 'a', newline = '') as f:
                        f.write(comment)
                        df.to_csv(f)
                else:            
                    df.to_csv(path)
                    
            return df
        
        else:
            return None
        
    def load(self, file):
        
        p = pl.Path(file)
        p = p.parent / (p.stem + '.csv')
        
        out = pd.read_csv(p, comment = '#', index_col = 't')
        
        return out
        
    def _dump_single_bin(self, var):
        
        var = self._sanitize_inputs(var)[0]
        log_var_idx = self._get_index(var)
        
        # Don't automatically capture binary output data! 
        old_state = self.amdc.captureOutput
        self.amdc.captureOutput = False

        start_time = time.time()
        timeout_sec = 50 # dump is at 2kSPS, so this could wait for 100k samples

        # Start dumping to host
        self.amdc.cmd(f"log dump bin {log_var_idx}")    
        
        # Reset the previous state
        self.amdc.captureOutput = old_state

        # These are repeating 4 times in the binary stream!
        MAGIC_HEADER = 0x12345678
        MAGIC_FOOTER = 0x11223344

        magic_header_idx = 0
        # magic_footer_idx = 0

        dump_data = bytearray()
        dump_data_idx = 0

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
                    # magic_footer_idx = dump_data_idx
                    found_footer = True
                    break

                dump_data_idx += 1

            if not found_footer:
                # Sleep for 100 ms to let OS do other work if needed
                time.sleep(0.1)

                # Break loop if timeout
                if time.time() >= start_time + timeout_sec:
                    raise Exception("ERROR: couldn't find magic footer!")

        end_time = time.time()

        # Flush the host OS to make sure all
        # serial data is out of buffers
        # (this might be necessary to get last sample value!)
        for i in range(0,10):
            out = bytes(self.amdc.ser.read_all())
            dump_data += out

        # print("magic_header_idx:", magic_header_idx)
        # print("magic_footer_idx:", magic_footer_idx)

        
        N = len(dump_data)
        bout = bytes(dump_data)

        metadata_start_idx = magic_header_idx+(4*4)

        s = struct.Struct("<III")
        unpacked_header      = s.unpack(bout[metadata_start_idx:metadata_start_idx+s.size])
        num_samples          = unpacked_header[0]
        sample_interval_usec = unpacked_header[1]
        data_type            = unpacked_header[2]

        print("Num samples:", num_samples)
        print("Dump took:", '{:.2f}'.format(end_time - start_time), " sec\n")
        # print("data type:", hex(data_type), data_type)
        
        dump_samples_idx = metadata_start_idx+s.size+1

        if data_type == 1:
            s = struct.Struct('<i')
        elif data_type == 2 or data_type == 3:
            s = struct.Struct('<f')
        else:
            raise Exception("ERROR: unknown data type!")

        samples = []
        time_sec = 0

        for i in range(0,num_samples):   
            sample = s.unpack(bout[dump_samples_idx:dump_samples_idx+s.size])
            dump_samples_idx += s.size

            ts  = time_sec
            val = sample[0]

            time_sec += sample_interval_usec / 1e6

            # FIXME(NP): we shouldn't need to do this.... :(
            if i != 0:
                samples.append([ts, val])
        
        # Convert to DataFrame

        arr = np.array(samples)
        df = pd.DataFrame(data = arr, columns = ['t', var[4::]])
        df['t'] = df['t'] - df['t'].min()
        df.set_index('t', inplace = True)

        return df
        
    def _dump_single_text(self, var):
        
        var = self._sanitize_inputs(var)[0]
        log_var_idx = self._get_index(var)
        
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
        df = pd.DataFrame(data = arr, columns = ['t', var[4::]])
        df['t'] = df['t'] - df['t'].min()
        
        df.set_index('t', inplace = True)
        
        self.amdc.printOutput = old_state #reset the print state

        return df
    
    def _get_index(self, var):
        
        try:
            loc = self.log_vars.index(var) #find location of var in list of vars
        except:
            raise Exception('ERROR: Invalid variable name')
        else:
            log_var_idx = self.log_var_indices[loc] #use location to get 
            return log_var_idx
    
    def _reset(self):
        
        self.available_indices = list(range(self.max_slots))[-1::-1]
        self.log_vars = []
        self.log_var_indices = []      
            
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
        
    @staticmethod
    def _sanitize_inputs(names):
        
        valid = [list, tuple]
        
        if type(names) in valid:
            pass
        else:
            names = names.split()
            
        out = []
            
        for name in names:
            if not ('LOG_' in name):
                name = 'LOG_' + name
            
            out.append(name)
            
        return out
    
    @staticmethod
    def _find_log_vars(file):
        
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
        
    def _get_amdc_state(self):
        
        old_state = self.amdc.printOutput
        self.amdc.printOutput = False
        
        out = self.amdc.cmd('log info')
        self.amdc.printOutput = old_state

        max_slots = int(out[4].split()[-1])
        
        names = []
        types = []
        indices = []
        sample_rates = []
        num_samples = []
        
        slot = 0
        
        for line in out:
            
            if ('Slot' in line) and ('unused' not in line):
                indices.append(slot)
                slot += 1
                
            if ('Slot' in line) and ('unused' in line):
                slot += 1
            
            if 'Name' in line:
                names.append(line.split()[-1])
                
            if 'Type' in line:
                types.append(line.split()[-1])
                
            if 'Sampling' in line:
                sample_rates.append(1 / ((int(line.split()[-1]))/1e6))
                
            if 'Num samples' in line:
                num_samples.append(int(line.split()[-1]))
                
        return max_slots, names, types, indices, sample_rates, num_samples
        
def find_mapfile(root):
    
    f = 'mapfile.txt'
    
    p = None

    for path, dirs, files in os.walk(root):
        if f in files:
            p = pl.Path(path) / f

    return p


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