TTP3121 - TCPIP PROG
ASSIGNMENT 2
Group - Yap Tan
File Repository System

DESCRIPTION


We use Virtualbox to create Kali linux virtual machine to do this system. 
This is an alternative way to write a C program and run it between on client and server.
It is using TCP connection.


TASK DISTRIBUTION
-----------------


Yap Chuen Zhen  -> Leader of the project
                -> Main Programmer
                -> testing the system
				
Tan Yong Soon -> Responsible for communication of server
                -> Error Checking

How to Run:
-----------------

Compile client.c and server.c (gcc filename.c -o filename)
Run both files on different terminal/pc (server.c must be run before client.c)
Client and Server Folder is for store the Files.
For example:

Terminal 1: ./server

Terminal 2: ./client serverHostName
		 eg ./client 127.0.0.1