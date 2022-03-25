import csv
import ctypes
import threading
import time
import os
import pandas as pd
from ctypes import *
import argparse

filename = 'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\uhmultithread\\build\\Release\\uhdriver.dll'
lib = ctypes.WinDLL(filename)

parser = argparse.ArgumentParser(description='Visualizer script for data samples')
parser.add_argument('-n', '--name', type=str, default='dataset', help='name of the dataset')
parser.add_argument('-s', '--sample', type=int, help='the sample number to test otherwise tests all samples')

args = parser.parse_args()

dataset_path = 'data/{}.csv'.format(args.name)
df = pd.read_csv(dataset_path)

if not args.sample:
    for i, row in df.iterrows():
        csv_path = row['path']

        data = create_string_buffer('{}'.format(os.path.abspath(csv_path)).encode(),size=200)
        lib.alter_me(data,sizeof(data))
        lib.csvfile()

        # # The array will run in a thread in the background, waiting for the rest of our instrunctions
        thread = threading.Thread(target=lib.main, args=([False])) #create said thread
        thread.daemon = True  # Daemonize thread
        thread.start()     # Start the execution

        time.sleep(1)
        thread.join()
else:
    csv_path = df.iloc[args.sample]['path']

    print("visualizing sample {} with data path: {}".format(args.sample, csv_path))

    data = create_string_buffer('{}'.format(os.path.abspath(csv_path)).encode(),size=200)
    lib.alter_me(data,sizeof(data))
    lib.csvfile()

    # # The array will run in a thread in the background, waiting for the rest of our instrunctions
    thread = threading.Thread(target=lib.main, args=([False])) #create said thread
    thread.daemon = True  # Daemonize thread
    thread.start()     # Start the execution

    time.sleep(1)
    thread.join()