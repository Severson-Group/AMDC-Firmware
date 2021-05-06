import socket
import sqlite3
import pandas
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import json

HOST = '192.168.1.10'  # The server's hostname or IP address
PORT = 7        # The port used by the server

SUPPORTED_TYPES = {
    "uint32_t": {"size": 4, "color": (91, 155, 213)},
    "uint16_t": {"size": 2, "color": (167, 202, 233)},
    "uint8_t": {"size": 1, "color": (223, 236, 247)},
    "bool": {"size": 1, "color": (159, 224, 142)},
    "float": {"size": 4, "color": (111, 111, 111)},
    "double": {"size": 8, "color": (111, 111, 111)},
    "char": {"size": 1, "color": (111, 111, 111)},
    "string": {"size": 0, "color": (111, 111, 111)},
}



byte_map_size = 12
num_vars = 6
byte_map = bytearray([0x00] * (16 + 4))
table = []

def main():

    with open("amdc_icc.json", "r+") as fp:
        data = fp.read()
        dict = json.loads(data)
        table = dict["table"]

    db = {
        "ticks": [],
    }

    for row in table:
        db[row["name"]] = []

    plt.ion()

    # con = sqlite3.connect('logger.db')
    # cur = con.cursor()
    #
    # cols = '("tick", '
    #
    # for i in range(0, len(table)):
    #
    #     cols += '"' + table[i]["name"] + '"' + (', ' if i < len(table)-1 else '')
    #
    # cols += ')'
    #
    # create = 'CREATE TABLE log ' + cols
    # cur.execute(create)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        while True:
            s.sendall(byte_map)
            data = s.recv(1024)
            insert = '(' + str(int.from_bytes(data[1:4], "little")) + ', '
            db["ticks"].append(int.from_bytes(data[1:4], "little"))
            offset = 5

            for i in range(0, len(table)):
                range_low = i + offset

                var_type = table[i]["datatype"]
                var_size = SUPPORTED_TYPES[var_type]["size"]
                range_high = i + offset + var_size
                res = str(int.from_bytes(data[range_low:range_high], "little"))
                insert += res + (', ' if i < len(table)-1 else '')
                offset += var_size

                db[table[i]["name"]].append(int.from_bytes(data[range_low:range_high], "little"))

            if len(db["ticks"]) > 10000:
                break
            # cur.execute('INSERT INTO log VALUES ' + insert)


        s.close()

        dataframe = pd.DataFrame.from_dict(db)
        dataframe.plot()
        plt.show()




if __name__ == "__main__":
    main()