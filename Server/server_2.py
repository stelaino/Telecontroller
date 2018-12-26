def drank(clientsock,clientaddress):
    while True:
        recvdata=clientsock.recv(buffsize).decode('utf-8')
        if recvdata =='exit' or not recvdata:
            break
        elif recvdata == '0':
            f=open("message.data","r")
            senddata=f.read(1)
            f.close()
            clientsock.send(senddata.encode())
    clientsock.close()
from socket import *
import threading
address='0.0.0.0'
port = 10087
buffsize=1024
s=socket(AF_INET,SOCK_STREAM)
s.bind((address,port))
s.listen(10087)
while True:
    clientsock,clientaddress=s.accept()
    print('connect from:',clientaddress)
    t=threading.Thread(target=drank,args=(clientsock,clientaddress))
    t.start()
s.close()