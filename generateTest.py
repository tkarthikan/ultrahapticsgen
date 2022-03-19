import numpy as np
import matplotlib.pyplot as plt
import csv
import ctypes
import threading
import time
import sys
import os
import pandas as pd
from ctypes import *
import argparse
import random

DATA_PATH = 'data/'
CSV_DATA_PATH = 'data/csv/'

# unchanged
f_s = 20e3 #for some reasons SDK run at 20kHz, so we set our sampling rate frequency to 20k

# changeable parameters
intensity = 1.00
size = 1
f_stm = 100 # angular frequency, 100Hz
end_time = 0.05 # (duration) we are going to produce 0.05s long stimuli

sample = 0

filename = 'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\uhmultithread\\build\\Release\\uhdriver.dll'
lib = ctypes.WinDLL(filename)

parser = argparse.ArgumentParser(description='Define testing configurations')
parser.add_argument('-c', '--category', type=str, default='circle', help='shape of generated samples')
parser.add_argument('-s','--samples', type=int, default=0, help='number of samples to automatically generate')

args = parser.parse_args()
category = args.category
print('Creating generated samples for category: {}'.format(category))

itr = 0 # iteration
while True:
    if (args.samples == 0):
        params = input("Params: ")

        # q to quit
        if params == "q": break 

        # parameters to tune:
        # time (t), intensity (i)
        parsed_params = params.split(" ")

        for p in parsed_params:
            if not p:
                break

            pType = float(p[:-1])

            if 't' in p:
                end_time = pType
            elif 'i' in p:
                intensity = pType
            elif 'l' in p:
                size = pType
            elif 'f' in p:
                f_stm = pType
    else:
        # automatic testing
        if (itr >= args.samples): break
        
        end_time = random.uniform(0, 1) # duration
        intensity = random.uniform(0, 1) # intensity
        size = float(random.randrange(100, 300))/100
        f_stm = random.randint(1, 400) # frequency in Hz
        
        # generate random params for end_time, intensity, size, f_stm

    ic = None
    xc = None
    yc = None
    zc = None

    stim_len = 0

    t = np.arange(0,end_time,1./f_s) #create the vector "time" 

    if (category == 'seven'):
        length = 0.04 * size
        v = length*f_stm 

        length1 = 0.08 * size
        v2 = length1*f_stm 

        #line with a jump on one end:
        x_pos_jump = np.mod(t*v, length) - length/2 

        x_pos1 = np.mod(t*v, length)
        y_pos1 = np.zeros((len(t)))

        x_pos2 = (length) - np.mod(t*v2, length) 
        y2 = 1.73*x_pos2 - 0.07

        x_pos3 = np.concatenate((x_pos1, x_pos2))
        y_pos3 = np.concatenate((y_pos1, y2))
        inten_3 = np.full(len(x_pos3), intensity)
        # z_pos3 = np.zeros((len(x_pos3)))

        new_t2 = t + end_time
        z_pos3 = np.concatenate((t, new_t2))

        arr_t = ctypes.c_double * len(z_pos3) #type are defined using ctypes

        #casting x,y and z position and intensity
        ic = arr_t(*inten_3.tolist())
        xc = arr_t(*x_pos3.tolist()) 
        yc = arr_t(*y_pos3.tolist())
        zc = arr_t(*z_pos3.tolist())
        
        stim_len = len(z_pos3)

    elif (category == 'circle'):
        radius = 0.02*size # 2cm radius
        height = 0.2*size # 20cm height, i.e. 20cm above the device

        #note, that is also possible to define a circle using focal point speed instead of angular frequency.
        #recall that focal point speed (speed) is equal to the circle cirumeference (2*pi*radius) times angular frequency (f_stm)
        #Hence, speed = 2*pi*radius * f_stm
        #Which is equivalent to f_stm = speed / (2*pi*radius)
        #Commonly, it's preferred to aim for speed = 7-8m/s

        # Using the vector time, we define a STM circle at 20cm height, of radius 2cm radius, and draw_frequency 100Hz
        x_pos = radius*np.cos(2*np.pi*f_stm*t) # parametric equation of a circle
        y_pos = radius*np.sin(2*np.pi*f_stm*t) # parametric equation of a circle
        z_pos = height*np.ones_like(t) #this means the z position is constant and set to height
        inten = np.full(len(t), intensity)

        # The library takes double as input, so we will create the appropriate casting type, length included
        arr_t = ctypes.c_double * len(t) #type are defined using ctypes

        #casting x,y and z position
        ic = arr_t(*inten.tolist())
        xc = arr_t(*x_pos) 
        yc = arr_t(*y_pos)
        zc = arr_t(*z_pos)

        stim_len = len(t)

    elif (category == 'square'):
        side = 0.1 * size
        length = 4*side
        v = length*f_stm 

        tmp = np.mod(t*v, length) #dummy variable that represent the position within the cycle

        # we compute each edge independently
        x_1 = np.where(tmp <= side, tmp, 0)
        x_2 = np.where(np.logical_and(tmp > side, tmp <= 2*side), side, 0)
        x_3 = np.where(np.logical_and(tmp > 2*side, tmp <= 3*side), 3*side - tmp, 0)
        x_4 = np.where(np.logical_and(tmp > 3*side, tmp <= 4*side), 0, 0)

        y_1 = np.where(tmp <= side, 0, 0)
        y_2 = np.where(np.logical_and(tmp > side, tmp <= 2*side), tmp - side, 0)
        y_3 = np.where(np.logical_and(tmp > 2*side, tmp <= 3*side), side, 0)
        y_4 = np.where(np.logical_and(tmp > 3*side, tmp <= 4*side), 4*side - tmp, 0)

        #we sumup the edges
        x_pos = x_1 + x_2 + x_3 + x_4
        y_pos = y_1 + y_2 + y_3 + y_4

        z_pos = np.ones_like((len(t)))
        inten = np.full(len(t), intensity)

        arr_t = ctypes.c_double * len(t) #type are defined using ctypes

        #casting x,y and z position
        ic = arr_t(*inten.tolist())
        xc = arr_t(*x_pos) 
        yc = arr_t(*y_pos)
        zc = arr_t(*z_pos.tolist())

    # display
    # ax = plt.axes(projection='3d')~
    # ax.scatter3D(x_pos3, y_pos3, inten_3, c=z_pos3);
    # plt.show()

    dataset_path = DATA_PATH + 'dataset.csv'

    # if directory doesn't exist create it
    if not os.path.exists(CSV_DATA_PATH):
        os.makedirs(CSV_DATA_PATH)

    if os.path.isfile(dataset_path):
        df = pd.read_csv(dataset_path)
        sample = int(df['sample_number'].max()) + 1

    csv_path = CSV_DATA_PATH + '{}.csv'.format(sample)

    print("Creating sample {} ({:.2f}i, {:.2f}t, {:.2f}l, {:.2f}f)...".format(sample, intensity, end_time, size, f_stm))

    with open(csv_path, 'w', newline='') as f:
        wr = csv.writer(f)
        for i in range(stim_len):    
            wr.writerow([ic[i], xc[i], yc[i], zc[i]])

    with open(dataset_path, 'a', newline='') as f:
        wr = csv.writer(f)

        if os.path.getsize(dataset_path) == 0:
            wr.writerow(['sample_number', 'category', 'intensity', 'duration', 'size', 'frequency', 'path'])

        wr.writerow([sample, category,intensity, end_time, size, f_stm, csv_path])

    if (args.samples == 0):
        data = create_string_buffer('{}'.format(os.path.abspath(csv_path)).encode(),size=200)
        lib.alter_me(data,sizeof(data))
        lib.csvfile()

        # # The array will run in a thread in the background, waiting for the rest of our instrunctions
        thread = threading.Thread(target=lib.main, args=([False])) #create said thread
        thread.daemon = True  # Daemonize thread
        thread.start()     # Start the execution

        time.sleep(1)
        thread.join()

    itr += 1
