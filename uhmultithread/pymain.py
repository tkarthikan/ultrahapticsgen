#First let's add a few packages, that we will need
import ctypes
import os
import sys
import numpy as np
import threading
import time
from ctypes import *

filename = 'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\uhmultithread\\build\\Release\\uhdriver.dll'
lib = ctypes.WinDLL(filename)

# alter_me = lib.alter_me
# alter_me.argtypes = c_char_p,c_size_t
# alter_me.restype = None

data = create_string_buffer(b'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\data\\csv\\0.csv',size=200)
lib.alter_me(data,sizeof(data))
lib.csvfile()

# # The array will run in a thread in the background, waiting for the rest of our instrunctions
thread = threading.Thread(target=lib.main, args=()) #create said thread
thread.daemon = True  # Daemonize thread
thread.start()     # Start the execution

time.sleep(120)
