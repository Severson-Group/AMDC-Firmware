import time
from collections import namedtuple
import numpy as np
import pandas as pd
import pathlib as pl
import os
import datetime
import struct
import binascii
import re

#########################################################
# Title:       AMDC Logging code
# Author(s):   Nicholas Hemenway, Nathan Petersen
# Date:        06/30/2020
#########################################################

LogVar = namedtuple(typename = 'LogVar', field_names = 'name index var_type samples_per_sec memory_addr')

class AMDC_Logger():
    
    default_max_slots = 32
    
    def __init__(self, AMDC, mapfile):
        
        self.amdc = AMDC
        self.mapfile = Mapfile(mapfile)
        
        self.available_indices = list(range(AMDC_Logger.default_max_slots))[-1::-1]
        self.log_vars = {}
        
    def info(self):
        max_slots, max_sample_depth, names, types, indices, sample_rates, num_samples = self._get_amdc_state()
        
        N = 15
        
        title = 'AMDC LOGGER INFO'.center(22).center(60, '#') + '\n\n'
        header = 'Variable Name:'.ljust(N) + 'Index:'.center(N) + 'Type:'.center(N) + 'Sample Rate [Hz]:'.center(N + 10) + 'Number of Samples:'
        
        print(title + header)
        
        for (var, idx, t, sps, samples) in zip(names, indices, types, sample_rates, num_samples):
            line = f'{var}'.ljust(N) + f'{idx}'.center(N) + f'{t}'.center(N) + f'{sps}'.center(N + 10) + f'{samples}'.center(N)
            print(line)

        # Add extra new line
        print()

    def sync(self):
        
        #the AMDC is the source of truth so calling sync will synchronize python
        #with the amdc. this is useful if the python kernel is restarted
        
        max_slots, max_sample_depth, names, types, indices, sample_rates, num_samples = self._get_amdc_state()
        self.max_slots = max_slots
        self.max_sample_depth = max_sample_depth

        self._reset()
        
        #we don't actually register the variables because they are already registered
        for var, idx, tp, sps in zip(names, indices, types, sample_rates):
            self._create_log_var(name = var, samples_per_sec = sps, var_type = tp, manual_index = idx)
            
    def register(self, log_vars, samples_per_sec = 1000, var_type = 'double'):
        
        names = self._sanitize_inputs(log_vars)
        
        for name in names:
            
            if len(self.available_indices) > 0:
                
                try:
                    LV = self._create_log_var(name, samples_per_sec, var_type)
                except Exception as e:
                    print(e)
                else:
                    #send command to AMDC
                    cmd = f'log reg {LV.index} {LV.name} {LV.memory_addr} {LV.samples_per_sec} {LV.var_type}'
                    out = self.amdc.cmd(cmd)
                    if out[1] == 'FAILURE':
                        self._pop(LV)
            
            else:
                print(f'Did not register, cannot exceed maximum of {self.max_slots} logged variables')
                print(f'Logged variables are:\n{self.log_vars}')
            
    def auto_register(self, root, samples_per_sec = 1000):
        
        #provide path to root of C code for which you want the logger to search for variables
        #of the form `LOG_{var_name}`
               
        log_vars, log_types = self.auto_find_vars(root)
        
        print(f'Total Number of Variables to be Logged: {len(log_vars)}')
        
        #register all found variables
        for var, tp in zip(log_vars, log_types):
            self.register(log_vars = var, samples_per_sec = samples_per_sec, var_type = tp)
                
    def unregister(self, log_vars, send_cmd = True):
        
        variables = self._sanitize_inputs(log_vars)
        
        for var in variables:
            try:
                LV = self._pop(var)
            except:
                print('ERROR: Invalid variable name')
            else:
                self.amdc.cmd(f'log unreg {LV.index}')
    
    def unregister_all(self):
        
        #we have to put log_vars into a local variable because self.log_vars changes
        #as we iterate over it
        variables = list(self.log_vars.keys())
        
        for var in variables:
            self.unregister(var)    
                
    def empty(self, log_vars):
        
        variables = self._sanitize_inputs(log_vars)
        
        for var in variables:
            try:
                log_var_idx = self.log_vars[var].index
                self.amdc.cmd(f'log empty {log_var_idx}')
            except:
                pass
        
    def empty_all(self):
        
        self.amdc.cmd(f'log empty_all')
        #for var in self.log_vars:
        #    self.empty(var)
        
    def auto_find_vars(self, root, regex=None):
        
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

        if regex is None:
            # Return all
            return log_vars, log_types
        else:
            # Filter by user-provided regex
            output_vars = []
            output_types = []

            for i,n in enumerate(log_vars):
                if re.search(regex, n) is not None:
                    output_vars.append(log_vars[i])
                    output_types.append(log_types[i])

            return output_vars, output_types
    
    def start(self):
        self.amdc.cmd('log start')
        
    def stop(self):
        self.amdc.cmd('log stop')
        
    def log(self, duration = 0.25):
        
        old_delay = self.amdc.comm_cmd_delay_cmd #store old time delay
        self.amdc.comm_cmd_delay_cmd = 0.01 #temporarily set delay between commands to shorter
        
        self.start()
        time.sleep(duration)
        self.stop()
        
        self.amdc.comm_cmd_delay_cmd = old_delay #reset cmd delay to previous value
        
        
    def dump(self, log_vars = None, file = None, comment = '', timestamp = True, timestamp_fmt = '%Y-%m-%d_H%H-M%M-S%S', how = 'binary', max_tries = 4, print_output = True):
              
        if log_vars is not None:
            variables = self._sanitize_inputs(log_vars)
        else:
            variables = self.log_vars.keys()
            
        binary = 0
        
        if how.lower() == 'ascii':
            dump_func = self._dump_single_text
        elif how.lower() == 'binary':
            binary = 1
            dump_func = self._dump_single_bin
        else:
            raise Exception("Invalid type of dump, valid types are: 'binary' or 'ascii'")
        
        tries = 0
        
        if len(variables) > 0:
            for i, var in enumerate(variables):
                
                if var in self.log_vars.keys():
                    
                    #try multiple times to load data if it fails
                    while tries < max_tries:
                        try:
                            if binary:
                                df_new = dump_func(var, print_output = print_output)
                            else:
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
    
    def _create_log_var(self, name, samples_per_sec, var_type, manual_index = None):
        
        memory_addr = int(self.mapfile.address(name), 0)
        
        if memory_addr == 0:
            raise Exception(f"ERROR: couldn't find memory address for '{name}'")
            
        else:
            if name not in self.log_vars.keys(): #check if variable name already registered

                if manual_index is None:
                    idx = self.available_indices.pop() #extract next available index
                else:
                    #remove index from list of available indices
                    loc = self.available_indices.index(manual_index)
                    idx = self.available_indices.pop(loc)
                
                #create LogVar object and append it to logvars
                LV = LogVar(name = name, index = idx, var_type = var_type, samples_per_sec = samples_per_sec, memory_addr = memory_addr)
                self.log_vars[name] = LV
                
                return LV
            
            else:
                raise Exception('Error: Variable already exists')
            
    def _pop(self, log_var):
        
        if log_var in self.log_vars.keys():
            
            LV = self.log_vars.pop(log_var)
            self.available_indices.append(LV.index)
            self.available_indices.sort(reverse = True)
            return LV
        
    def _dump_single_bin(self, var, print_output = True):
        # Function level debugging (for print statements)
        debug = False
        
        var = self._sanitize_inputs(var)[0]
        LV = self.log_vars[var]
        log_var_idx = LV.index
        
        # Don't automatically capture binary output data! 
        old_state_captureOutput = self.amdc.comm_cmd_resp_capture
        old_state_cmdDelay = self.amdc.comm_cmd_delay_cmd
        self.amdc.comm_cmd_resp_capture = False
        self.amdc.comm_cmd_delay_cmd = 0.010

        start_time = time.time()

        # Dump is at ~2kSPS, wait for max of full sample depth (plus margin)
        timeout_sec = 1.1 * (self.max_sample_depth / 1800)

        # Start dumping to host
        if self.amdc.comm_method is 'uart':
            self.amdc.cmd(f"log dump uart bin {log_var_idx}") 
        elif self.amdc.comm_method is 'eth':
            self.amdc.cmd(f"log dump eth bin {log_var_idx}") 
            
        if print_output:
            print("Dumping:", var)
        
        # Reset the previous state
        self.amdc.comm_cmd_resp_capture = old_state_captureOutput
        self.amdc.comm_cmd_delay_cmd = old_state_cmdDelay

        # These are repeating 4 times in the binary stream!
        MAGIC_HEADER = 0x12345678
        MAGIC_FOOTER = 0x11223344

        magic_header_idx = 0
        magic_footer_idx = 0

        dump_data = bytearray()
        dump_data_idx = 0
        dump_data_max = 0

        latest_data_time = time.time()

        found_footer = False
        while not found_footer:
            # Read in all serial data from OS buffer
            out = bytes(self.amdc.read(4096))
            dump_data += out

            N = len(out)
            if N > 0:
                latest_data_time = time.time()

            dump_data_max += N
            dump_data_idx = 0

            s = struct.Struct('<IIII')    

            for i in range(0,dump_data_max-s.size):
                magic = s.unpack(dump_data[i:i+s.size])

                if magic[0] == MAGIC_HEADER and magic[1] == MAGIC_HEADER and magic[2] == MAGIC_HEADER and magic[3] == MAGIC_HEADER:
                    magic_header_idx = dump_data_idx

                if magic[0] == MAGIC_FOOTER and magic[1] == MAGIC_FOOTER and magic[2] == MAGIC_FOOTER and magic[3] == MAGIC_FOOTER:
                    magic_footer_idx = dump_data_idx
                    found_footer = True
                    break

                dump_data_idx += 1

            if not found_footer:
                # Break loop if timeout or have not seen new data for 1 second
                if (time.time() >= start_time + timeout_sec) or (time.time() - latest_data_time >= 1):
                    raise Exception("ERROR: timeout, could not find footer!")

        end_time = time.time()

        # Flush the host OS to make sure all serial data is out of buffers
        #
        # This might be unnessecary; at minimum, we might need to do this
        # one time to get the CRC bytes from the AMDC since they come after
        # the footer bytes.
        for i in range(0,1000):
            out = bytes(self.amdc.read(4096))
            dump_data += out

        if debug:
            print("DEBUG:", "all data:", binascii.hexlify(bytearray(dump_data)), len(dump_data))

        if debug:
            print("DEBUG:", "magic_header_idx:", magic_header_idx)
            print("DEBUG:", "magic_footer_idx:", magic_footer_idx)

            foot_data = dump_data[magic_footer_idx:magic_footer_idx+16]
            print("DEBUG:", "FOOTER:", binascii.hexlify(bytearray(foot_data)), len(foot_data))

        # Calculate CRC-32 over data we got and make sure it is valid!
        crc_data = dump_data[magic_header_idx:magic_footer_idx+16]

        if debug:
            print("DEBUG:", "full data length:", len(crc_data))
            print("DEBUG:", "HEADER:", binascii.hexlify(bytearray(crc_data[0:16])))
            print("DEBUG:", "full data:", binascii.hexlify(bytearray(dump_data[magic_header_idx:magic_footer_idx+16+4])))

        crc_calculated_python = binascii.crc32(crc_data, 0) & 0xffffffff

        if debug:
            print("DEBUG:", "crc_calculated_python:", hex(crc_calculated_python))

        # Extract the CRC that the AMDC sent
        crc_from_amdc = dump_data[magic_footer_idx+16:magic_footer_idx+20]
        crc_from_amdc = struct.unpack('<I', crc_from_amdc)[0]

        if debug:
            print("DEBUG:", "crc_from_amdc:", hex(crc_from_amdc))

        # Check to make AMDC CRC matches Python CRC
        if crc_calculated_python != crc_from_amdc:
            raise Exception("ERROR: CRC doesn't match:", hex(crc_calculated_python), hex(crc_from_amdc))

        N = len(dump_data)
        bout = bytes(dump_data)

        metadata_start_idx = magic_header_idx+(4*4)

        s = struct.Struct("<III")
        unpacked_header      = s.unpack(bout[metadata_start_idx:metadata_start_idx+s.size])
        num_samples          = unpacked_header[0]
        sample_interval_usec = unpacked_header[1]
        data_type            = unpacked_header[2]

        if print_output:
            print("Dump took:", '{:.3f}'.format(end_time - start_time), " sec")
            print("Dump rate:", '{:.3f}'.format(num_samples / (end_time - start_time)), " sps")

        if debug:
            print("DEBUG:", "data type:", hex(data_type), data_type)

        dump_samples_idx = metadata_start_idx+s.size

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

            samples.append([ts, val])

            time_sec += sample_interval_usec / 1e6

        if print_output:        
            print("Num samples:", num_samples, "\n")

        # Convert to DataFrame
        arr = np.array(samples)

        # Round all timesteps to nearest 1usec
        arr[:,0] = arr[:,0].round(6)

        df = pd.DataFrame(data = arr, columns = ['t', var[4::]])
        df['t'] = df['t'] - df['t'].min()
        df.set_index('t', inplace = True)

        return df
        
    def _dump_single_text(self, var):
        
        var = self._sanitize_inputs(var)[0]
        LV = self.log_vars[var]
        log_var_idx = LV.index
        
        #we want to set the print state to false so we don't
        #print all of our data to the screen
        old_state = self.amdc.comm_cmd_resp_capture
        self.amdc.comm_cmd_resp_capture = False
        
        if self.amdc.comm_method is 'uart':
            self.amdc.cmd(f"log dump uart text {log_var_idx}", timeout_sec = 120)
        elif self.amdc.comm_method is 'eth':
            self.amdc.cmd(f"log dump eth text {log_var_idx}", timeout_sec = 120)
        
        samples = []
        
        line = ""
        while True:
            c = self.amdc.read(1).decode()
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
                        self.amdc.read(1)
                    
                    # End the outer for loop
                    break
            
                line = ""

        arr = np.array(samples)
        df = pd.DataFrame(data = arr, columns = ['t', var[4::]])
        df['t'] = df['t'] - df['t'].min()
        
        df.set_index('t', inplace = True)
        
        # Reset the print state
        self.amdc.comm_cmd_resp_capture = old_state

        return df
    
    def _reset(self):
        
        self.available_indices = list(range(self.max_slots))[-1::-1]
        self.log_vars = {}
            
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
        
        old_state = self.amdc.comm_cmd_resp_print
        self.amdc.comm_cmd_resp_print = False
        
        oldDelay = self.amdc.comm_cmd_delay_cmd
        self.amdc.comm_cmd_delay_cmd = 0.5
        
        out = self.amdc.cmd('log info')
        self.amdc.comm_cmd_resp_print = old_state

        max_slots = int(out[3].split()[-1])
        max_sample_depth = int(out[4].split()[-1])
        
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
                
        self.amdc.comm_cmd_delay_cmd = oldDelay
                
        return max_slots, max_sample_depth, names, types, indices, sample_rates, num_samples
        
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
