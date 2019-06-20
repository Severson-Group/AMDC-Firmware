import serial
import time
import numpy as np
from Mapfile import Mapfile

class AMDC:
	def __init__(self, mapfile,
							port, baudrate = '115200',
							cmdDelay = 0.5, cmdDelayChar = 0.001,
							cmdEcho = True, cmdEchoPrepend = "\t> "):
		# Serial port configuration
		self.ser = serial.Serial(timeout = 0)
		self.ser.baudrate = baudrate
		self.ser.port = port

		# Mapfile config
		self.mapfile = Mapfile(filepath = mapfile)

		# Command config
		self.cmdDelay = cmdDelay
		self.cmdDelayChar = cmdDelayChar
		self.cmdEcho = cmdEcho
		self.cmdEchoPrepend = cmdEchoPrepend

	def connect(self):
		self.ser.open()


	def disconnect(self):
		self.ser.close()


	def cmd(self, cmd_str):
		to_send_str = "{0}\r\n".format(cmd_str)
		to_send_bytes = str.encode(to_send_str)
		for b in to_send_bytes:
			self.ser.write(bytes([b]))
			
			# Pause between letters so we don't send data too fast
			time.sleep(self.cmdDelayChar)
		
		# Wait for cmd to execute on AMDC
		time.sleep(self.cmdDelay)
		
		# Print log for user
		if self.cmdEcho:
			print("{0}{1}".format(self.cmdEchoPrepend, cmd_str))


	def log_reg(self, log_var_idx, name, sps, var_type):
		memory_addr = int(self.mapfile.address(name), 0)
		if memory_addr == 0:
			print("ERROR: couldn't find memory address for '{0}'".format(name))
		cmd = 'log reg {0} {1} {2} {3} {4}'.format(log_var_idx, name, memory_addr, sps, var_type)
		self.cmd(cmd)


	def log_dump(self, log_var_idx):
		self.cmd("log dump {0}".format(log_var_idx))
		
		samples = []
		
		line = ""
		while True:
			c = self.ser.read().decode()
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
						self.ser.read()
					
					# End the outer for loop
					break
			
				line = ""

		return np.array(samples)


	def _process_line(self, line):
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
