#define STD_FULL
#define VARS
#define NETWORKING

#include "pilib.h"
//#include "poll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"   // port we're listening on

// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::atomic_bool runtime = { true };

int main(void)
{

    char buf[256];    // buffer for client data
    int nbytes;

// SETUP
//**************************************************************

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, * ai, * p;

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

//**************************************************************
// END SETUP

    timeval intv = {1, 0}; // select() interval

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    FD_SET(listener, &master);  // add the listener to the master set

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    while (runtime) {
        read_fds = master; // copy it
        if (int sel = select((fdmax + 1), &read_fds, NULL, NULL, &intv) <= 0) {
            if (sel == 0) { // timed out
                continue;
            }
            perror("select");
            continue;   //or exit()
        }

        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // a fd is set
                if (i == listener) {    // listening fd is set -> new connection available

                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    if ((newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen)) < 0) {
                        perror("accept");
                        continue;
                    }
                    FD_SET(newfd, &master); // add new connection to master set
                    if (newfd > fdmax) {    // keep track of the max
                        fdmax = newfd;
                    }
                    // debug
                    printf("selectserver: new connection from %s on socket %d\n", 
                        inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), 
                        newfd);
                }
                else { // no new connection, a current connection is active
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) { // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        }
                        else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                    else { // got data to parse

                        // HANDLE RECV

                        //for (j = 0; j <= fdmax; j++) {
                        //    // send to everyone!
                        //    if (FD_ISSET(j, &master)) {
                        //        // except the listener and ourselves
                        //        if (j != listener && j != i) {
                        //            if (send(j, buf, nbytes, 0) == -1) {
                        //                perror("send");
                        //            }
                        //        }
                        //    }
                        //}
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}