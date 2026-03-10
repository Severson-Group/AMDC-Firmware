from AMDC import AMDC

amdc = AMDC()

# Set comm defaults for ETHERNET
amdc.setup_comm_defaults('eth')

# Init ethernet 
amdc.eth_init()

# Set up the default ASCII command socket
s0, s0_id = amdc.eth_new_socket('ascii_cmd')
amdc.eth_set_default_ascii_cmd_socket(s0)

amdc.connect()
amdc.comm_cmd_delay_cmd = .01
amdc.cmd('ctrl get enc')
amdc.cmd('test analog')
amdc.disconnect()