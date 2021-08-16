#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <memory>
#include <thread>
#include <sstream>
#include <fstream>
#include "pivar.h"

class BaseSocket {
private:
	sockaddr_in address;
	int r_sock;
	int r_connection;
public:
	BaseSocket(int domain, int service, int protocol, int port, unsigned long interface) {
		address.sin_family = domain;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = htonl(interface);

		r_sock = socket(domain, service, protocol);
		check(r_sock);
	}

	virtual int netConnect(int sock, sockaddr_in address) = 0;
	void check(int item) {
		if (item < 0) {
			perror("Connection failed. ");
			exit(EXIT_FAILURE);
		}
	}
	sockaddr_in getAddress() {
		return address;
	}
	int getSock() {
		return r_sock;
	}
	int getConnection() {
		return r_connection;
	}
	void setConnection(int val) {
		r_connection = val;
	}
};

class BindingSocket: public BaseSocket {
public:
	BindingSocket(int domain, int service, int protocol, int port, unsigned long interface) : BaseSocket(domain, service, protocol, port, interface) {
		setConnection(netConnect(getSock(), getAddress()));
		check(getConnection());
	}

	int netConnect(int sock, sockaddr_in address) {
		return bind(sock, (sockaddr*)&address, sizeof(address));
	}
};

class ConnectingSocket : public BaseSocket {
public:
	ConnectingSocket(int domain, int service, int protocol, int port, unsigned long interface) : BaseSocket(domain, service, protocol, port, interface) {
		setConnection(netConnect(getSock(), getAddress()));
		check(getConnection());
	}

	int netConnect(int sock, sockaddr_in address) {
		return connect(sock, (sockaddr*)&address, sizeof(address));
	}
};

class ListeningSocket : public BindingSocket {
private:
	int backlog;
	int listening;
public:
	ListeningSocket(int domain, int service, int protocol, int port, unsigned int interface, int bklg) : BindingSocket(domain, service, protocol, port, interface) {
		backlog = bklg;
		startListening();
		check(listening);
	}

	void startListening() {
		listening = listen(getSock(), backlog);
	}
};

class BaseServer {
private:
	ListeningSocket* socket;
	virtual void acceptor() = 0;
	virtual void handler() = 0;
	virtual void responder() = 0;
public:
	BaseServer(int domain, int service, int protocol, int port, unsigned long interface, int bklg) {
		socket = new ListeningSocket(domain, service, protocol, port, interface, bklg);
	}
	virtual void launch() = 0;
	ListeningSocket* getSocket() {
		return socket;
	}
};

class SimpleServer : public BaseServer {
private:
	char buffer[30000];
	int new_socket;
	void acceptor() {
		sockaddr_in addr = getSocket()->getAddress();
		int addrlen = sizeof(addr);
		new_socket = accept(getSocket()->getSock(), (sockaddr*)&addr, (socklen_t*)&addrlen);
		read(new_socket, buffer, 30000);
	}
	void handler() {
		std::cout << buffer << newline;
	}
	void responder() {
		write(new_socket, "shut up", strlen("shut up"));
		close(new_socket);
	}
public:
	SimpleServer(): BaseServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10) {
		while (true) {
			std::string inp;
			std::cin >> inp;
			if (inp == "exit") {
				exit(0);
			}
			else {
				launch();
			}
		}
	}
	void launch() {
		std::cout << "w" << newline;
		acceptor();
		handler();
		responder();
		std::cout << "d" << newline;
	}
};

// * * * * * * * * * * * * * * *

class Server {
private:
	sockaddr_in address;
	int sock, nsock, maxlistens, addrlen;
public:
	Server(int backlog = 10, int domain = AF_INET, int service = SOCK_STREAM, int protocol = 0, int port = 80, ulong interface = INADDR_ANY) : maxlistens(backlog) {
		address.sin_family = domain;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = htonl(interface);

		addrlen = sizeof(address);

		if ((sock = socket(domain, service, protocol)) == 0) {
			error("Socket creation failure ");
		}
	}
	void bindServer() {
		if (bind(sock, (sockaddr*)&address, addrlen) < 0) {
			error("Binding socket failure ");
		}
	}
	void startListen() {
		if (listen(sock, maxlistens) < 0) {
			error("Listening initialization failure ");
		}
	}
	void launch(std::atomic_bool& condition, std::ostream& output, std::string& message) {
		char buffer[10000];
		while (condition) {
			if (!condition) {
				std::cout << "Exiting loop" << newline;
				break;
			}
			std::cout << "Waiting for connection... ";
			if ((nsock = accept(sock, (sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
				error("Accept failed ");
			}
			std::cout << "Connection recieved" << newline;
			read(nsock, buffer, sizeof(buffer));
			output << buffer << newline;
			write(nsock, message.c_str(), strlen(message.c_str()));
			std::cout << "Sent message" << newline;
			close(nsock);
			std::cout << "Closed socket" << newline;
		}
	}
	void error(const char* message) {
		perror(message);
		exit(EXIT_FAILURE);
	}
};

namespace http {
	/*HEADER FORMAT:
	* version_{status-code}_{status-message}
	* date_{date}
	* server_{name}
	* type_{type}
	* len_{length}
	* ~~~~~~~~~>
	* (\n)
	* CONTENT */

	namespace headerbase {
		CE_STR httpv = "HTTP/1.1 ";
		CE_STR date = "Date: ";
		CE_STR server = "Server: ";
		CE_STR type = "Content-Type: ";
		CE_STR len = "Content-Length: ";
	}
	
	namespace codes {
		//https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
		CE_STR continu = "100 Continue";
		CE_STR ok = "200 OK";
		CE_STR created = "201 Created";
		CE_STR accepted = "202 Accepted";
		CE_STR no_content = "204 No Content";
		CE_STR reset_content = "205 Reset Content";
		CE_STR partial_content = "206 Partial Content";
		CE_STR mult_choices = "300 Multiple Choices";
		CE_STR moved_perm = "301 Moved Permanently";
		CE_STR found = "302 Found";
		CE_STR bad_request = "400 Bad Request";
		CE_STR unauthorized = "401 Unauthorized";
		CE_STR pay_up = "402 Payment Required";
		CE_STR forbidden = "403 Forbidden";
		CE_STR not_found = "404 Not Found";
		CE_STR method_na = "405 Method Not Allowed";
		CE_STR not_acc = "406 Not Acceptable";
		CE_STR request_timeout = "408 Request Timeout";
		CE_STR conflict = "409 Conflict";
		CE_STR gone = "410 Gone";
		CE_STR len_req = "411 Length Required";
		CE_STR url_too_l = "414 URI Too Long";
		CE_STR unsupported_media = "415 Unsupported Media Type";
		CE_STR tp_egg = "418 I'm a teapot"; //*
	}
}

void httpResponse(std::ostream& output) {
	std::ifstream html("/data/pihost/build/Raspberry/Source/item.html");
	std::string buffer;
	std::ostringstream content;
	while (std::getline(html, buffer)) {
		content << buffer << newline;
	}
	output << http::headerbase::httpv << http::codes::ok << newline << http::headerbase::type << "text/html" << newline << http::headerbase::len << strlen(content.str().c_str()) << newline << newline << content.str();
}

std::atomic_bool run = { true };

void endCondition() {
	std::cin.ignore();
	run = false;
}

int main() {
	std::ostringstream requests, message;

	std::thread end(endCondition);

	Server server;
	server.bindServer();
	server.startListen();
	httpResponse(message);
	std::string msg = message.str();
	std::cout << msg << newline;
	server.launch(std::ref(run), requests, msg);

	end.join();
	return 0;
}