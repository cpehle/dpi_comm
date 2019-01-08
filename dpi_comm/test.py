import socket
import struct
import sys
import time

def send(s):
    for i in range(100):        
        s.send(struct.pack('!q', i))

def recv(s):
    for i in range(100):
        x = s.recv(8)
        print(struct.unpack('!q', x))

if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', int(sys.argv[1]))
    s.connect(server_address)

    time.sleep(5)

    if sys.argv[2] == "send":
        send(s)

    if sys.argv[2] == "recv":
        recv(s)

    s.close()

    

