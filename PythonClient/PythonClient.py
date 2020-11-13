import ctypes 
from ctypes import *
import socket, struct
from enum import IntEnum
from threading import Thread
from time import sleep
import sys

HOST = '127.0.0.1'
PORT = 11111

#lib=CDLL(r"G:\универ\4 курс\Леонов\Lab2_Con2\ClientServer2_2\Debug\SerializeLib.dll")
#lib = ctypes.cdll.LoadLibrary(r"G:\универ\4 курс\Леонов\Lab2_Con2\ClientServer2_2\Debug\SerializeLib.dll") 
#lib.getSerializeString.argtypes=POINTER(c_char_p), POINTER(c_char_p), POINTER(c_char_p), c_int, c_int
#lib.getSerializeString.restype=None
#lib.getM_HeaderFromString.argtypes=c_char_p, c_char_p, c_char_p, c_int, c_int
#lib.getM_HeaderFromString.restype=None

class Messages(IntEnum):
	M_CREATE=0
	M_EXIST=1
	M_NOUSER=2
	M_INIT=3
	M_EXIT=4
	M_GETDATA=5
	M_NODATA=6
	M_TEXT=7
	M_CONFIRM=8
	M_INCORRECT=9
	M_ACTIVE=10
	M_INACTIVE=11
	M_ABSENT=12

lib_dll=CDLL(r"G:\универ\4 курс\Леонов\Lab2_Con2\ClientServer2_2\Debug\SerializeLib.dll")
#lib_dll.getStringFromHeader.argtypes=[c_char_p, c_char_p, c_int, c_int]
#lib_dll.getStringFromHeader.restype=c_char_p
#lib_dll.getHeaderFromString.argtypes=[POINTER(POINTER(c_ubyte)), c_char_p]
#lib_dll.getHeaderFromString.restype=c_char_p
class Header():
	def __init__(self, m_To="", m_From="", m_Type=0, m_Size=0):
		self.m_To=m_To
		self.m_From=m_From
		self.m_Type=m_Type
		self.m_Size=m_Size

	def HeaderInit(self, header):
		self.m_To=header[0]
		self.m_From=header[1]
		self.m_Type=header[2]
		self.m_Size=header[3]

class Message():
	def __init__(self, To='', From="", Type=0, m_Data=''):
		self.m_Header=Header();
		self.m_Header.m_To=To;
		self.m_Header.m_From=From;
		self.m_Header.m_Type=Type;
		self.m_Header.m_Size=int(len(m_Data))
		self.m_Data=m_Data
	def SendData(self, s, password):
		buffer=ctypes.create_string_buffer(1024)
		lib_dll.getSerializeString(byref(buffer),self.m_Header.m_From.encode('utf-8'), self.m_Header.m_To.encode('utf-8'), self.m_Header.m_Type, self.m_Header.m_Size)
		
#		lib_dll.DeleteChar()
#		print(buffer)
		
		

		#lib_dll.getM_HeaderFromString(buffer.value, )
#		
#		buffer=ctypes.create_string_buffer(str.encode(original))
#		lib_dll.getStringFromHeader(b"TestFrom", b"TestFrom", 1, 2, byref(buffer))
#		print(buffer)
#		mem = POINTER(c_ubyte)()
#		size = c_int(0)
#		res=c_char_p()
#		lib_dll.getHeaderFromString(buffer,byref(res))
#		print(size.value,mem[0],mem[1],mem[2],mem[3])

		lenght=int(len(buffer.value))
		s.send(struct.pack('i',lenght))
		s.send(struct.pack(f'{lenght}s', buffer.value))

		if (self.m_Header.m_Type==Messages.M_TEXT):
			s.send(struct.pack(f'{self.m_Header.m_Size}s', self.m_Data.encode('utf-8')))
		if (self.m_Header.m_Type==Messages.M_INIT or  self.m_Header.m_Type==Messages.M_CREATE):
			s.send(struct.pack('i',int(len(password))))
			s.send(struct.pack(f'{int(len(password))}s', password.encode('utf-8')))
		del buffer
	def ReceiveData(self, s):
		self.m_Header = Header()
		lenght=struct.unpack('i', s.recv(4))
		buffer=struct.unpack(f'{lenght[0]}s', s.recv(lenght[0]))[0]

		frombuffer=ctypes.create_string_buffer(1024)
		tobuffer=ctypes.create_string_buffer(1024)
		typebuffer=c_int(0)
		sizebuffer=c_int(0)
		
		lib_dll.getM_HeaderFromString(buffer, byref(frombuffer), byref(tobuffer), byref(typebuffer), byref(sizebuffer))
#		print(frombuffer.value, tobuffer.value, typebuffer.value, sizebuffer.value)

		self.m_Header.m_From=frombuffer.value
		self.m_Header.m_To=tobuffer.value
		self.m_Header.m_Size=sizebuffer.value
		self.m_Header.m_Type=typebuffer.value

		if self.m_Header.m_Type==Messages.M_TEXT:
			self.m_Data=struct.unpack(f'{self.m_Header.m_Size}s', s.recv(self.m_Header.m_Size+1))[0]
		del frombuffer
		del tobuffer
		del typebuffer
		del sizebuffer
		return self.m_Header
def SendMessage(m_Socket, To, From, Type=Messages.M_TEXT, Data='', password=''):
	msg=Message(To, From, Type, Data)
	msg.SendData(m_Socket, password)
def ReceiveMessage(m_Socket):
    msg=Message()
    hMsg = msg.ReceiveData(m_Socket)
    return hMsg
def socketStart(m_Socket):
    m_Socket.connect((HOST, PORT))
def socketEnd(m_Socket):
    m_Socket.close()

global connected

def myThread(Login):
    while connected==True:
        mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)        
        socketStart(mySocket)
        SendMessage(mySocket, "@SERVER", Login, Messages.M_GETDATA)
        msg=Message()
        hMsg=msg.ReceiveData(mySocket)
        socketEnd(mySocket)
        if (hMsg.m_Type == Messages.M_TEXT):
            print(msg.m_Data, '\n')
        sleep(1)



head=Header()

while head.m_Type!=Messages.M_CONFIRM:
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		n=int(input("0. Create Account\n1. Use Account\n"))
		Login=input("Enter Login\n")
		password=input("Enter Password\n")
		if n==0:
			socketStart(s)
			SendMessage(s, "@SERVER", Login, Messages.M_CREATE,'', password)
		if n==1:
			socketStart(s)
			SendMessage(s, "@SERVER", Login, Messages.M_INIT, '', password)
		head = ReceiveMessage(s)
		socketEnd(s)
		#print(head.m_Type)
		if head.m_Type==Messages.M_INCORRECT:
			print("Sorry, wrong password")
		elif head.m_Type==Messages.M_ACTIVE:
			print("Sorry, this user is already connected. You cannot run one account on different clients")
		elif head.m_Type==Messages.M_EXIST:
			print("Sorry, this user already exists")
		elif head.m_Type==Messages.M_NOUSER:
			print("Sorry, no such user was found")
		elif head.m_Type==Messages.M_CONFIRM:
			print("You have successfully connected to the server\n")
			connected=True
			th = Thread(target=myThread, args=(Login,))
			th.start()
			#th.join()
		else:
			print("Something went wrong")
	


while connected==True:
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		n=int(input("Press 0 for send Message\nPress 1 for Exit\n"))
		if n==0:
			to=str(input("Enter Login of client\n"))
			stroka=input("Enter your Message\n")
			socketStart(s)
			SendMessage(s, to, Login, Messages.M_TEXT, stroka)
			hmsg=ReceiveMessage(s)
			socketEnd(s)
			if hmsg.m_Type==Messages.M_INACTIVE:
				print("Your message will be delivered as soon as the user connects")
			elif hmsg.m_Type==Messages.M_ABSENT:
				print("The user you want to send a message to is not listed")
			elif hmsg.m_Type==Messages.M_CONFIRM:
				print("The message was delivered successfully")
			else:
				print("Something went wrong")
		if n==1:
			socketStart(s)
			SendMessage(s, "@SERVER", Login, Messages.M_EXIT)
			hmsg=ReceiveMessage(s)
			socketEnd(s)
			if hmsg.m_Type==Messages.M_CONFIRM:
				print("You have successfully disconnected")
				connected=False
			elif hmsg.m_Type==Messages.M_INACTIVE:
				print("You have been disconnected by the server")
				connected=False
			else:
				print("An error has occurred")

