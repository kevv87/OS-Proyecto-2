from fileinput import close
from http import server
import socket 
import sys
from constants import *

def createServer(ip, port):

    if isinstance(port, int) and isinstance(ip, str):

        try:

            serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

            serverSocket.bind((ip, port))

            return serverSocket

        except:

            return "Error creating the server"

    else:

        return "ip or port incorrect"

def executeServer(serverSocket, messageSize):

    message = ""

    answer = "All right"

    print("Waiting clients")

    serverSocket.listen(1)
    
    while(message != "end"):

        try:
            connection, clientAddress = serverSocket.accept()

            print("Connection from: ", clientAddress)

            while(message != "end"):

                message = connection.recv(messageSize).decode()

                if(message):

                    print("Message received: ",message)

                    connection.send(answer.encode())

        except:

            print("Error accepting the connection from client")

        connection.close()

def runServer(ip, port, size):

    server = createServer(ip, port)

    executeServer(server, size)

runServer(LOCALHOST, PORT, BUFFER_SIZE)
