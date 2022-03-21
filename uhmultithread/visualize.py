import csv
import ctypes
import threading
import time
import os
import pandas as pd
from ctypes import *

DATASET_PATH = 'data/dataset.csv'

filename = 'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\uhmultithread\\build\\Release\\uhdriver.dll'
lib = ctypes.WinDLL(filename)

df = pd.read_csv(DATASET_PATH)

for i, row in df.iterrows():
    csv_path = row['path']

    data = create_string_buffer('{}'.format(os.path.abspath(csv_path)).encode(),size=200)
    lib.alter_me(data,sizeof(data))
    lib.csvfile()

    # # The array will run in a thread in the background, waiting for the rest of our instrunctions
    thread = threading.Thread(target=lib.main, args=([False])) #create said thread
    thread.daemon = True  # Daemonize thread
    thread.start()     # Start the execution

    time.sleep(2)
    thread.join()