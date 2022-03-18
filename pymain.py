#First let's add a few packages, that we will need
import ctypes
import os
import sys
import numpy as np
import threading
import time
from ctypes import *

filename = "/Users/karthik/Documents/GenHap/UH_Multithread/build/libuhdriver.so" 
lib = ctypes.cdll.LoadLibrary(filename)

alter_me = lib.alter_me
alter_me.argtypes = c_char_p,c_size_t
alter_me.restype = None


data = create_string_buffer(b'/Users/karthik/Documents/GenHap/UH_Multithread/build/lineseven4_8_100hz_0005s.csv' ,size=200)
alter_me(data,sizeof(data))
lib.csvfile()

# # The array will run in a thread in the background, waiting for the rest of our instrunctions
thread = threading.Thread(target=lib.main, args=()) #create said thread
thread.daemon = True  # Daemonize thread
thread.start()     # Start the execution

time.sleep(120)
