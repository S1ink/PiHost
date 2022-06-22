#include "server.h"

#include <thread>


template<class handler_t, class formatter_t>
void HttpServer::serveWorker1_0(HttpServer* inst) {
	static_assert(std::is_base_of<HttpServer::HttpHandler, handler_t>::value, "Template argument {handler_t} must inherit from HttpServer::HttpHandler");
	static_assert(std::is_base_of<HttpServer::HttpFormatter, formatter_t>::value, "Template argument {formatter_t} must inherit from HttpServer::HttpFormatter");

	handler_t handler(inst);
	formatter_t formatter(inst);

	inst->version = Version::HTTP_1_0;  //since this is a public function, the HTTP version could be different that that which was initialized with
    inst->prepServer();
    formatter.onServerStart();

    SOCK_T nsock; 
    int readlen;
    char buffer[10000], ipbuff[INET6_ADDRSTRLEN];
    sockaddr_storage naddr;
    socklen_t naddrlen = sizeof(naddr);
    timeval tbuff, checkup = { 1, 0 };
    fd_set master, fdbuff;
    FD_ZERO(&master);
    FD_ZERO(&fdbuff);
    FD_SET(inst->sockmain, &master);

    inst->l_online = true;   //server is (will be) online

    while (*(inst->online)) {
        tbuff = checkup;
        fdbuff = master;
        if (select((inst->sockmain + 1), &fdbuff, NULL, NULL, &tbuff) == -1) {    //wait for activity and perform checkup every timeval
            inst->lerror("Select error");
            continue;
            //filter error and exit if needed here
        }
        if ((nsock = accept(inst->sockmain, (sockaddr*)&naddr, &naddrlen)) < 0) {
            inst->lerror("Error accepting connection");
            continue;
        }
        getSockIp(nsock, ipbuff);
        formatter.onConnect(nsock, ipbuff);
        if ((readlen = recv(inst->sockmain, buffer, sizeof(buffer), 0)) < 0) {
            inst->lerror("Error recieving data");
        }
        else {
            handler.respond(nsock, ipbuff, readlen, std::string(buffer), &formatter);
            formatter.onDisconnect(nsock, ipbuff);
            memset(&buffer, 0, sizeof(buffer));
        }
        CLOSE_SOCK(nsock);
    }
    CLOSE_SOCK(inst->sockmain);
    formatter.onServerEnd();
}
template<class handler_t, class formatter_t>
void HttpServer::serveWorker1_1(HttpServer* inst) {
	static_assert(std::is_base_of<HttpServer::HttpHandler, handler_t>::value, "Template argument {handler_t} must inherit from HttpServer::HttpHandler");
	static_assert(std::is_base_of<HttpServer::HttpFormatter, formatter_t>::value, "Template argument {formatter_t} must inherit from HttpServer::HttpFormatter");

	handler_t handler(inst);
	formatter_t formatter(inst);

	inst->version = Version::HTTP_1_1;
    inst->prepServer();
    formatter.onServerStart();

    SOCK_T nsock;
    int readlen, rbuff, maxfd = inst->sockmain;
    char buffer[10000], ipbuff[INET6_ADDRSTRLEN];
    sockaddr_storage naddr;
    socklen_t naddrlen = sizeof(naddr);
    timeval tbuff, checkup = { 1, 0 };
    fd_set master, fdbuff;
    FD_ZERO(&master);
    FD_ZERO(&fdbuff);
    FD_SET(inst->sockmain, &master);

    inst->l_online = true;   //server is (will be) online

    while (*(inst->online)) {
        tbuff = checkup;
        fdbuff = master;
        if ((rbuff = select((maxfd + 1), &fdbuff, NULL, NULL, &tbuff)) <= 0) {  //wait up to 1 second for activiy, else continue (check for stop message)
            if (rbuff == -1) {
                inst->lerror("Select error");
            }
            continue;
        }
        for (int i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &fdbuff)) {
                if (i == inst->sockmain) {  //if our fd is ready
                    if ((nsock = accept(i, (sockaddr*)&naddr, &naddrlen)) < 0) {    //accept new connection
                        inst->lerror("Error accepting connection");
                        continue;
                    }
                    FD_SET(nsock, &master); //add connection fd to master
                    if (nsock > maxfd) {    //update the highest fd
                        maxfd = nsock;
                    }
                    getSockIp(nsock, ipbuff);
                    formatter.onConnect(nsock, ipbuff);
                }
                else {  //else a client fd is ready
                    if ((readlen = recv(i, buffer, sizeof(buffer), 0)) <= 0) {  //read from client fd
                        if (readlen == 0) {
                            getSockIp(i, ipbuff);
                            formatter.onDisconnect(i, ipbuff);
                        }
                        else {
                            inst->lerror("Error recieving data");
                        }
                        CLOSE_SOCK(i);
                        FD_CLR(i, &master);
                        if (i >= maxfd) {
                            maxfd -= 1;
                        }
                    }
                    else {  //respond
                        getSockIp(i, ipbuff);
                        handler.respond(i, ipbuff, readlen, std::string(buffer), &formatter);
                    }
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
        }
    }
    CLOSE_SOCK(inst->sockmain);
    formatter.onServerEnd();
}



template<class handler_t, class formatter_t>
void HttpServer::serve() {
    switch (this->version) {
    case Version::HTTP_1_0:
        HttpServer::serveWorker1_0<handler_t, formatter_t>(this);
        break;
    case Version::HTTP_1_1:
        HttpServer::serveWorker1_1<handler_t, formatter_t>(this);
        break;
    case Version::HTTP_2_0: //fall back to 1.1 but also print warning
		this->logs << "HTTP version (2.0) not supported - utilized fallback to HTTP/1.1\n";
        HttpServer::serveWorker1_1<handler_t, formatter_t>(this);
        break;
    default:
        this->logs << "HTTP version not supported - failed to start server\n";
    }
}
template<class handler_t, class formatter_t>
void HttpServer::serve1_0() {
	HttpServer::serveWorker1_0<handler_t, formatter_t>(this);
}
template<class handler_t, class formatter_t>
void HttpServer::serve1_1() {
	HttpServer::serveWorker1_1<handler_t, formatter_t>(this);
}

template<class handler_t, class formatter_t>
void HttpServer::serveThread() {
	switch (this->version) {
    case Version::HTTP_1_0:
        std::thread(HttpServer::serveWorker1_0<handler_t, formatter_t>, this).detach();
        break;
    case Version::HTTP_1_1:
        std::thread(HttpServer::serveWorker1_1<handler_t, formatter_t>, this).detach();
        break;
    case Version::HTTP_2_0: //fall back to 1.1 but also print warning
		this->logs << "HTTP version (2.0) not supported - utilized fallback to HTTP/1.1\n";
        std::thread(HttpServer::serveWorker1_1<handler_t, formatter_t>, this).detach();
        break;
    default:
        this->logs << "HTTP version not supported - failed to start server\n";
    }
}
template<class handler_t, class formatter_t>
void HttpServer::serveThread1_0() {
	std::thread(HttpServer::serveWorker1_0<handler_t, formatter_t>, this).detach();
}
template<class handler_t, class formatter_t>
void HttpServer::serveThread1_1() {
	std::thread(HttpServer::serveWorker1_1<handler_t, formatter_t>, this).detach();
}