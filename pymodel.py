#!/usr/bin/env python3
import sys
import os
import time
import struct
import subprocess
import tensorflow as tf
from tensorflow import keras
import numpy as np
import matplotlib.pyplot as plt
from subprocess import Popen, PIPE
import struct
import socket
import json
import csv
def encode_int_array(int_list):
    buf = struct.pack("!I" + "I" * len(int_list), len(int_list), *int_list)
    return buf
 
# # next create a socket object
# s = socket.socket()        
# print ("Socket successfully created")
 
# # reserve a port on your computer in our
# # case it is 12345 but it can be anything
# port = 12345               
 
# # Next bind to the port
# # we have not typed any ip in the ip field
# # instead we have inputted an empty string
# # this makes the server listen to requests
# # coming from other computers on the network
# s.bind(('', port))        
# print ("socket binded to %s" %(port))
 
# # put the socket into listening mode
# s.listen(5)    
# print ("socket is listening")   
# creating an empty list
lst = []
print("Welcome to GenHap")
# number of elements as input
print("\nInput your choice \n 1 for fire,\n 2 for ice,\n 3 for lightning,\n 4 for custom design")
n = int(input())
if n==1:
    lst =[25, 1, 2, 3]
elif n==2:
    lst =[0, 2, 3, 24]
elif n==3:
    lst =[10,25,2,15]
else:
    # iterating till the range
    for i in range(0, 4):
        print("Enter control parameter",i+1,":")
        ele = int(input())
        lst.append(ele) # adding the element
        
    print(lst)
new_model = tf.keras.models.load_model('saved_model/genhap_model')
new_model.summary()
testz = np.array([8,  8 , 5, 6])
testz = np.array(lst)
decodz = new_model.decoder(testz.reshape(1,4))
print(decodz.numpy().reshape((16,16)))
#plt.imshow(decodz.numpy().reshape((16,16)))
#plt.show()
out =[[]]
xlist =[]
ylist =[]
inten =[]
for idy, y in enumerate(decodz.numpy().reshape((16,16))):
  for idx, x in enumerate(y):
    if x > 0.0:
      print(idx, idy, x)
      out.append([idx,idy, x])
      xlist.append(idx)
      ylist.append(idy)
      inten.append(x)
# ultraleap_output = mapping_function(decodz) 
ultraleap_output = '12\t'
#print(type(out[0]))
#buf = struct.pack('%sf' % len(out), *out)
# lists = out.tolist()
# json_str = json.dumps(lists)
#data = json.dumps(json_str)
with open("sheetgenhap.csv","w+") as my_csv:
    csvWriter = csv.writer(my_csv,delimiter=',')
    csvWriter.writerows(out)
#data = json.dumps(str(out))
# while True:
# # Establish connection with client.
#   c, addr = s.accept()    
#   print ('Got connection from', addr )
#   # send a thank you message to the client. encoding to send byte type.
#   #socket.send()
#   c.send(data.encode())
 
#   #c.sendall(data2)
  
#   # reply = c.recv(port)
#   # print(reply)
#   # # c.send(data.encode())
 
#   # Close the connection with the client
#   c.close()
   
#   # Breaking once connection closed
#   break
#mapping function to map the image to sensation 
#subprocess.run(["./build/cpp/Streaming_Line", "first_arg"], input=buf)
#p = Popen(['./build/cpp/Streaming_Circle'], shell=True, stdout=PIPE, stdin=PIPE)
# for ii in range(10):
#     value = str(ii) + '\n'
#     value = bytes(value, 'UTF-8')  # Needed in Python 3.
#     p.stdin.write(value)
#     p.stdin.flush()
#     result = p.stdout.readline().strip()
#     print(result)
# p.stdin.write(bytes([3]))
# p.stdin.flush()
# result = p.stdout.readline().strip()
# print(result)
#username = input("Enter the type of sensation to generate?")
#print("Sensation " + username)
#message = '12\t'
#k = 12
#message = k.to_bytes(2, byteorder='big')