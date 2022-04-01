import numpy as np
import csv
import ctypes
import os
import pandas as pd
from ctypes import *
import argparse
import random
import threading
import time
from scipy.spatial import distance

DATA_PATH = 'data/'
CSV_DATA_PATH = 'data/csv/'

filename = 'C:\\Users\\andy9\\Dev\\Ultraleap-Haptics-3.0.0-beta.10-Windows-Win64-Research\\genhap\\uhmultithread\\build\\Release\\uhdriver.dll'
lib = ctypes.WinDLL(filename)

# unchanged
f_s = 20e3 #for some reasons SDK run at 20kHz, so we set our sampling rate frequency to 20k

# changeable parameters
intensity = 1.00
size = 1
f_stm = 100 # angular frequency, 100Hz
end_time = 0.05 # (duration) we are going to produce 0.05s long stimuli

sample = 0

parser = argparse.ArgumentParser(description='Define testing configurations')
parser.add_argument('-n', '--name', type=str, default='dataset', help='name of the dataset')
parser.add_argument('-c', '--category', type=str, default='circle', help='shape of generated samples')
parser.add_argument('-s','--samples', type=int, default=0, help='number of samples to automatically generate')

args = parser.parse_args()
category = args.category
print('Creating generated samples for category: {}'.format(category))

def seven(intensity, size, f_stm, t):
    length = 0.04 * size
    v = length*f_stm

    length1 = 0.08 * size
    v2 = length1*f_stm

    #line with a jump on one end:
    # x_pos_jump = np.mod(t*v, length) - length/2

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

    return inten_3, x_pos3, y_pos3, z_pos3

def circle(intensity, size, f_stm, t):
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

    return inten, x_pos, y_pos, z_pos

def square(intensity, size, f_stm, t):
    side = 0.1*size
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

    z_pos = np.ones_like(t)
    inten = np.full(len(t), intensity)

    return inten, x_pos, y_pos, z_pos

def compute_polygon(points, t, f_stm):
    sides = np.zeros(len(points))
    #first we need to compute the length of the path:
    for i in np.arange(len(points)):
        if(i < len(points)-1):
            sides[i] = distance.euclidean(points[i], points[i+1])
        else:
            sides[i] = distance.euclidean(points[i], points[0])
    length = np.sum(sides)
    print(length)

    #compute the speed:
    v = f_stm * length

    #start computing the
    tmp = np.mod(t*v, length)
    x_pos = np.zeros_like(t)
    y_pos = np.zeros_like(t)
    for i in np.arange(len(points)):
        if(i == 0): #first line
            alpha = tmp/sides[0]
            pos_x = (1-alpha)*points[i,0] + alpha*points[i+1,0]
            pos_y = (1-alpha)*points[i,1] + alpha*points[i+1,1]

            x_pos += np.where(tmp <= sides[0], pos_x, 0)
            y_pos += np.where(tmp <= sides[0], pos_y, 0)

        elif(i >= len(points)-1): #last line
            alpha = (tmp - np.sum(sides[:i]))/sides[i]
            pos_x = (1-alpha)*points[i,0] + alpha*points[0,0]
            pos_y = (1-alpha)*points[i,1] + alpha*points[0,1]

            x_pos += np.where(tmp > np.sum(sides[:i]), pos_x, 0)
            y_pos += np.where(tmp > np.sum(sides[:i]), pos_y, 0)

        else: #all the other lines
            alpha = (tmp - np.sum(sides[:i]))/sides[i]
            pos_x = (1-alpha)*points[i,0] + alpha*points[i+1,0]
            pos_y = (1-alpha)*points[i,1] + alpha*points[i+1,1]

            x_pos += np.where(np.logical_and(tmp > np.sum(sides[:i]), tmp <= np.sum(sides[:i+1])), pos_x, 0)
            y_pos += np.where(np.logical_and(tmp > np.sum(sides[:i]), tmp <= np.sum(sides[:i+1])), pos_y, 0)
            
    return x_pos, y_pos

def triangle(intensity, f_stm, t):
    #We define a list of points, which will be the verteces of our polygon
    points = np.array([[0, 0], [1, 0], [0.5, np.sqrt(3)/2]]) #should look like an equilateral triangle
    x_tri, y_tri = compute_polygon(points, t, f_stm)

    z_pos = np.ones_like(t)
    inten = np.full(len(t), intensity) 

    return inten, x_tri, y_tri, z_pos

def hexagon(intensity, f_stm, t):
    points = np.array([[0, 1], [1, 0], [2,0], [3,1], [2,2], [1,2]]) #should look like an hexagon
    x_hex, y_hex = compute_polygon(points, t, f_stm)

    z_pos = np.ones_like(t)
    inten = np.full(len(t), intensity)

    return inten, x_hex, y_hex, z_pos

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
        
        # generate random params for end_time, intensity, size, f_stm
        end_time = round(random.uniform(0, 1), 2) # duration
        intensity = round(random.uniform(0, 1), 2) # intensity
        size = float(random.randrange(100, 300))/100
        f_stm = random.randint(16, 265) # frequency in Hz

    inten = None
    x_pos = None
    y_pos = None
    z_pos = None

    stim_len = 0

    t = np.arange(0,end_time,1./f_s) #create the vector "time" 

    if (category == 'seven'):
        inten, x_pos, y_pos, z_pos = seven(intensity, size, f_stm, t)
    elif (category == 'circle'): # default category
        inten, x_pos, y_pos, z_pos = circle(intensity, size, f_stm, t)
    elif (category == 'square'):
        inten, x_pos, y_pos, z_pos = square(intensity, size, f_stm, t)
    elif (category == 'triangle'):
        inten, x_pos, y_pos, z_pos = triangle(intensity, f_stm, t)
    elif (category == 'hexagon'):
        inten, x_pos, y_pos, z_pos = hexagon(intensity, f_stm, t)
    else:
        print("Invalid category")
        break

    # The library takes double as input, so we will create the appropriate casting type, length included
    arr_t = ctypes.c_double * len(x_pos) #type are defined using ctypes

    #casting x,y and z position
    ic = arr_t(*inten.tolist())
    xc = arr_t(*x_pos) 
    yc = arr_t(*y_pos)
    zc = arr_t(*z_pos.tolist())

    dataset_path = DATA_PATH + '{}.csv'.format(args.name)
    csv_sub_data_path = CSV_DATA_PATH + '{}/'.format(args.name)

    # if directory doesn't exist create it
    if not os.path.exists(csv_sub_data_path):
        os.makedirs(csv_sub_data_path)

    if os.path.isfile(dataset_path):
        df = pd.read_csv(dataset_path)
        sample = int(df['sample_number'].max()) + 1

    csv_path = csv_sub_data_path + '{}.csv'.format(sample)

    print("Creating sample {} ({:.2f}i, {:.2f}t, {:.2f}l, {:.2f}f)".format(sample, intensity, end_time, size, f_stm))

    with open(csv_path, 'w', newline='') as f:
        wr = csv.writer(f)
        for i in range(len(x_pos)):    
            wr.writerow([ic[i], xc[i], yc[i], zc[i]])

    with open(dataset_path, 'a', newline='') as f:
        wr = csv.writer(f)

        if os.path.getsize(dataset_path) == 0:
            wr.writerow(['sample_number', 'category', 'intensity', 'duration', 'size', 'frequency', 'path'])
        
        wr.writerow([sample, category,intensity, end_time, size, f_stm, csv_path])

    itr += 1