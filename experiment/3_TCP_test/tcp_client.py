#!/usr/bin/env python

import socket

SERVER_IP = '172.20.10.10'
SERVER_PORT = 8888
BUFFER_SIZE = 1024

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((SERVER_IP, SERVER_PORT))

send_data = ""
while(send_data != "end"):
    send_data = input("Please input message you want to send: ")
    client_socket.send(send_data.encode("UTF-8"))
    
#data = s.recv(BUFFER_SIZE)
#s.close()

#print ("received data:".format(data))
