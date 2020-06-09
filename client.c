#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Global variables */
#define BUFFER_SIZE 500

void error(const char *msg){                                                    /* Error function used for reporting issues */
    perror(msg); 
    exit(1); 
} 

/**************************************************************** 
* Name: getAddressInfo()
* Description: This function receives the server address and server port number as arguments in order to get the address information. It sets 
*               up the hints address info stucture, which is passed to the getaddrinfo() function. The getaddrinfo() function 
*               allocates and initalizes a linked list of addrinfo structures, one for each network address, each of which contains 
*               an address that can be specified in a call to bind or connect. If the call to the getaddrinfo() function is successful, 
*               result will contain a pointer to the start of the list addrinfo structures. This function will return a pointer to the
*               addrinfo structure back to main.
* Resources used: https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa
*                   https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo
*                   https://beej.us/guide/bgnet/html/multi/clientserver.html
*                   http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
****************************************************************/
struct addrinfo *getAddressInfo(char *inAdd, char *portNum){
    struct addrinfo hints;
    struct addrinfo *result;                                                    /* Will point to the results */
    int status = 0;

    memset(&hints, 0, sizeof(hints));                                           /* Ensure that the struct is empty */
    hints.ai_family = AF_INET;                                                  /* Designated IP v4 as the address family that this socket can communicate with */
    hints.ai_socktype = SOCK_STREAM;                                            /* TCP stream socket */

    status = getaddrinfo(inAdd, portNum, &hints, &result);                      /* I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */
    
    if(status != 0){                                                            /* Print out an error message and exit if the call to the getaddrinfo() function was unsuccessful */
        error("CLIENT: Error in getting address.\n");
    }

    return result;                                                              /* Return the response information */
}

/**************************************************************** 
* Name: createSocket()
* Description: This function receives a pointer to the addrinfo structure as an argument. This function will call the socket() function to 
*               create a socket, which will return a socket file descriptor. This function will return the socket file descriptor 
*               back to main.
*Resources used: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo
*                   https://beej.us/guide/bgnet/html/multi/clientserver.html
****************************************************************/
int createSocket(struct addrinfo *getInfo){
    int status = 0;

    status = socket(getInfo->ai_family, getInfo->ai_socktype, getInfo->ai_protocol);    /* I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */
    
    if(status == -1){                                                           /* Print out an error message and exit if the call to the socket() function was unsuccessful */
        error("CLIENT: Error in creating socket.\n");
    }

    return status;                                                              /* Return the socket file descriptor */
}

/**************************************************************** 
* Name: connectToSocket()
* Description: This function receives a socket file descriptor and a pointer to the addrinfo structure as arguments. This function 
*               will call the connection() function to create a connection. This function will not return any values back to main.
* Resources used: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo 
****************************************************************/
void connectToSocket(int socketFD, struct addrinfo *getInfo){
    int status = 0;

    status = connect(socketFD, getInfo->ai_addr, getInfo->ai_addrlen);          /* I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */
    
    if(status == -1){
        error("CLIENT: Error connecting.\n");
    }
}

/**************************************************************** 
* Name: establishHandshake()
* Description: This function receives a socket file descriptor, the server handle and the client handle as arguments. This function will call the
*               send() function and recv() function to set up a handshake for TCP connection. This function will not return any values back to main.
****************************************************************/
void establishHandshake(int socketFD, char *clientHandle, char *serverHandle){
    int charsWritten, charsRead;

    /* Send message to server to set up handshake for TCP connection */
    charsWritten = send(socketFD, clientHandle, strlen(clientHandle), 0);       /* Write to the server */

    if (charsWritten < 0){
        error("CLIENT: ERROR writing to socket");
    }

    /* Receive message from server to set up handshake for TCP connection */
    charsRead = recv(socketFD, serverHandle, 10, 0);                            /* Read data from the socket */

    if (charsRead < 0){
        error("CLIENT: ERROR reading from socket");
    }
}

/**************************************************************** 
* Name: startChat()
* Description: This function receives a socket file descriptor, the client handle and the server handle. It will take the input that the 
*               user types into the chat client side and send that message to the chat server side. It will also receive the messages 
*               sent from the chat server side and display those messages to the user. The chat client and chat server messages will 
*               alternate sending and receiving messages. Either the chat client or chat server can close the connection with the command "\quit".
*               This function will not return any values back to main.
* Resources used: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo 
*                   https://www.geeksforgeeks.org/fgets-gets-c-language/
****************************************************************/
void startChat(int socketFD, char *clientHandle, char *serverHandle){
    char input[BUFFER_SIZE];                                                    
    char prompt[2] = "> ";
    char prependedInput[512];                                                   /* Variable to hold the client's message prepended with the client's handle + "> " */
    char output[BUFFER_SIZE];
    int writtenStatus = 0;
    int readStatus = 0;

    memset(input, 0, sizeof(input));                                            /* Clear out the array */
    memset(output, 0, sizeof(output));                                          /* Clear out the array */
    memset(prependedInput, 0, 512);                                             /* Clear out the array */

    fgets(input, BUFFER_SIZE, stdin);                                           /* Clear stdin. I referenced: https://www.geeksforgeeks.org/fgets-gets-c-language/ */

    while(1){                                                                   /* While loop will continue to loop through until either the user types "\quit" or receives the message "\quit" from the server */
        printf("%s> ", clientHandle);
        fgets(input, BUFFER_SIZE, stdin);                                       /* Read data from stdin and store it in input. I referenced: https://www.geeksforgeeks.org/fgets-gets-c-language/ */

        if(strcmp(input, "\\quit\n") == 0){                                     /* If the user typed in "\quit", the program will break from the while loop and close the connection */
            break;
        }

        strcat(prependedInput, clientHandle);                                   /* Concatenate prependedInput with the client's handle */
        strcat(prependedInput, prompt);                                         /* Concatenate the string copied to prependedInput with "> " */
        strcat(prependedInput, input);                                          /* Concatenate the string copied to prependedInput with the string received from the client */

        writtenStatus = send(socketFD, prependedInput, strlen(prependedInput), 0);  /* Write to the server */

        if(writtenStatus == -1){
            error("Error occurred. Message not sent.\n");
        }

        readStatus = recv(socketFD, output, BUFFER_SIZE, 0);                    /* Read data from the socket */

        if(readStatus == -1){
            error("Error occurred. Message not received.\n");
        }
        else if(strcmp(output, "\\quit") == 0){
            printf("Server closed connection.\n");                              /* Inform user that the server closed the connection */
            break;
        }
        else{
            printf("%s\n", output);                                             /* Print out the message from the server */
        }

        memset(input, 0, sizeof(input));                                        /* Clear out the array */                                 
        memset(output, 0, sizeof(output));                                      /* Clear out the array */
        memset(prependedInput, 0, 512);                                         /* Clear out the array */
    }

    close(socketFD);                                                            /* Close the connection on the socket file descriptor. I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */
    printf("Connection has been closed.\n");
}

/* Please note, I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */
int main(int argc, char *argv[]){
    int socketFD, charsWritten, charsRead;
    int badLen = 1;                                                             /* Variable to hold 0 or 1 denoting correct/incorrect length of the chatclient username */
    char serverHandle[10];                                                      /* Array to hold the server handle */
    char clientHandle[10];                                                      /* Array to hold the client handle */

    if (argc != 3){                                                             /* Verify if the correct number of arguments were used. There should be 3 arguments, which are the command, the server address and the port # */
        fprintf(stderr,"CLIENT: Incorrect number of arguments.\n"); 
        exit(1);                                                                /* Set the exit value to 1 */
    }

    while(badLen){                                                              /* While loop to ensure that the user enters a handle that is between 1 and 10 characters */
        printf("Please enter a handle that is 1 word and 10 characters or less: ");
        scanf("%s", clientHandle);                                              /* Read input from stdin and place it into the array clientHandle */
        badLen = 0;

        if(strlen(clientHandle) > 10 || strlen(clientHandle) == 0){             /* If the user did not input any characters or input more than 10 characters, the program will re-prompt the user */
            badLen = 1;
        }
    }

    struct addrinfo *addInfo = getAddressInfo(argv[1], argv[2]);                /* Call the getAddressInfo() function and return a pointer to addrinfo structure */

    socketFD = createSocket(addInfo);                                           /* Call the createSocket() function to create a socket and return a socket file descriptor */

    connectToSocket(socketFD, addInfo);                                         /* Call the connectToSocket() function to set up a connection */

    establishHandshake(socketFD, clientHandle, serverHandle);                   /* Call the establishHandshake() function to set up a handshake for TCP connection */

    startChat(socketFD, clientHandle, serverHandle);                            /* Call the startChat() function to allow the client and server to send/receive messages between each other */

    freeaddrinfo(addInfo);                                                      /* Free the linked list allocated by getaddrinfo(). I utilized Beej's Guide: https://beej.us/guide/bgnet/html/multi/syscalls.html#getaddrinfo */

    return 0;
}
