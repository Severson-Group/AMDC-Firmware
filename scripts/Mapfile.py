
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