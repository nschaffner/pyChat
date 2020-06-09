#!/bin/python

from socket import *
import sys

def establishHandshake(newSConnection, sHandle):
    # Receive the initial message sent by the client and set it to the cHandle variable to hold the client's handle. I utilized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
    cHandle = newSConnection.recv(512)
    # Send a message to the client containing the name of the server. I utilized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
    newSConnection.send(sHandle)

def startChat(newConnection, serverH):
    charsReceived = ""
    charsWritten = ""

    # While loop will continue to loop through until either the user types "\quit" or the server receives the message "\quit" from the client
    while 1:
        # Read data from socket, which will be stored in charsReceived. I utilized: https://stackoverflow.com/questions/509211/understanding-slice-notation to remove the newline character from the message
        charsReceived = newConnection.recv(501)[0:-1]
        charsWritten = ""

        # If the client entered "\quit", they will not send any data and charReceived will be empty
        if charsReceived == "":
            print "Connection closed. Wait for new connection."
            break

        # Print out the message from the client. I utilized: https://www.geeksforgeeks.org/python-output-formatting/
        print "{}".format(charsReceived)

        # Get input from the user and return the data in a string, which will be stored in charsWritten. While loop will repeat if input is not > 0 and < 500. I utilized: https://docs.python.org/2/library/functions.html#raw_input
        while len(charsWritten) == 0 or len(charsWritten) > 500:
            charsWritten = raw_input("{}> ".format(serverH))

        # If the user entered "\quit", print out a message to the user and send a message to the client informing them that the connection has been closed
        if charsWritten == "\quit":
            print "Connection closed. Wait for new connection."
            newConnection.send(charsWritten)
            break
    
        # Prepend the server's handle + "> " to the beginning of the user's message. I utilized: https://stackoverflow.com/questions/2711579/how-can-strings-be-concatenated
        charsWritten = serverH + "> " + charsWritten
        # Sent the prepended message to the client. I utilized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
        newConnection.send(charsWritten)

if __name__ == "__main__":
    serverHandle = ""

    # Verify whether the correct number of arguments have been provided. I utlized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
    if len(sys.argv) != 2:
        print "SERVER: Incorrect number of arguments"
        exit(1)

    # Get input from the user and return the data in a string, which will be stored in serverHandle. While loop will repeat if input is not > 0 and < 10. I utilized: https://docs.python.org/2/library/functions.html#raw_input
    while len(serverHandle) == 0 or len(serverHandle) > 10:
        serverHandle = raw_input("Please enter a handle that is 1 word and 10 characters or less: ")

    # Take the first argument from the command prompt as the port number. I utlized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
    portNum = sys.argv[1]
    # Create a socket file descriptor with the address of the socket and the type of socket, which is TCP. I utilized: https://stackoverflow.com/questions/8033552/python-socket-bind-to-any-ip 
    newServerSocket = socket(AF_INET, SOCK_STREAM)
    # Bind the server to an available IP address at specified port number. I utilized: https://stackoverflow.com/questions/8033552/python-socket-bind-to-any-ip and https://www.geeksforgeeks.org/simple-chat-room-using-python/
    newServerSocket.bind(('', int(portNum)))
    # Listen for 1 connection at a time. I utilized: https://stackoverflow.com/questions/8033552/python-socket-bind-to-any-ip 
    newServerSocket.listen(1)

    print "Server is running."
    
    while 1:
        # Accepts a connection request and stores 2 parameters: the socket object for that user and the IP address of the client that has just connected
        # I utilized: https://stackoverflow.com/questions/8033552/python-socket-bind-to-any-ip 
        newSocketConnection, newAddress = newServerSocket.accept()
        print "New connection created"

        establishHandshake(newSocketConnection, serverHandle)

        startChat(newSocketConnection, serverHandle)
    # Close the connection on the socket file descriptor. I utilized: https://www.geeksforgeeks.org/simple-chat-room-using-python/
    newSocketConnection.close()
