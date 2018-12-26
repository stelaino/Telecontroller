def drank(clientsock,clientaddress):
    while True:
        recvdata=clientsock.recv(buffsize).decode('utf-8')
        if recvdata =='exit' or not recvdata:
            break
        elif recvdata == '0' or '1':
            print('recv',recvdata)
            f=open("message.data","w")
            f.write(recvdata)
            f.close()
        elif recvdata == '3' or '4':
            print('recv',recvdata)
            f=open("message.data","w")
            f.write(recvdata)
            f.close()
    clientsock.close()
from socket import *
import threading
address="0.0.0.0"
port = 10086
buffsize=1024
s=socket(AF_INET,SOCK_STREAM)
s.bind((address,port))
s.listen(1000)
while True:
    clientsock,clientaddress=s.accept()
    print('connect from:',clientaddress)
    t=threading.Thread(target=drank,args=(clientsock,clientaddress))
    t.start()
s.close()
