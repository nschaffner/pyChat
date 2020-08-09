## pyChat
This is a simple chat system that works for one pair of users:
- The chat server starts on host A and waits on a specified port for a chat client request
- The chat client starts on host B, specifying host A's hostname and port number on the command line
- The chat client will  prompt the user for a "handle", which is a one-word name that can be up to 10 characters
- The chat client will display this handle as a prompt on host B and will prepend it to all messages  sent to the chat server on host A (ex: "Client1> Hi!")
- The chat client on host A sends an initial  message to the chat server on host B, which causes a connection to be established between the two hosts
- The chat client and chat server messaging can then alternate sending and receiving messages
- Messages sent from the chat server will have the host A handle prepended to its sent messages
- Either the chat client or chat server can close the connection with the command "\quit"
- If the connection is closed, the chat server will remain open for new connections until it receives a SIGINT

### Deployment
After cloning this repository, please follow the steps below to run the server.py and client.c programs:

1) In the first terminal, log into flip1 and run the following command in the directory containing the server.py file:\
    chmod +x server.py 

2) Run the server.py program using the following command in the directory containing the server.py file:
    \python server.py <port #>

    - Please note you will choose a port # for the server to run on

3) In the second terminal, log into flip2 and run the following 2 commands in the directory containing the client.c file:\
    gcc -o client client.c\
    ./client flip1 <port #>

    - Please note that the port # will be the same number that you previously chose for the server to run on

### Notes
- The first message must be sent by the client. The server cannot send the first message
- The connection can be closed with the command "\quit", which can be sent by either the client or server
- If a connection is closed, the server.py program will continue to run and accept new connections. To stop this program, use SIGINT
- The maximum amount of characters that can be sent in a message is 500
- The server.py program was tested on flip1 and the client.c program was tested on flip2 of my university's UNIX servers