import time
import numpy as np
import pandas as pd
import pathlib as pl
import os

class AMDC_Logger():
    
    max_vars = 8
    
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
        
        
    def dump(self, file = None):
        
        if len(self.log_vars) > 0:
            for i, var in enumerate(self.log_vars):
                
                if i == 0:
                    df = self.dump_single(var)
                else:
                    df = df.join(self.dump_single(var), how = 'outer')
                    
            if file != None:
                df.to_csv(file)
                    
            return df
        
        else:
            return None

    def dump_single(self, var):
        
        try:
            log_var_idx = self.log_vars.index(var)
        except:
            print('ERROR: Invalid variable name')
            return None
        
        #we want to set the print state to false so we don't
        #print all of our data to the screen
        old_state = self.amdc.printOutput
        self.amdc.printOutput = False
        
        self.amdc.cmd(f"log dump {log_var_idx}")
        
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