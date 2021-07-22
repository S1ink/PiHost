#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <memory>
#include <thread>
#include <sstream>
#include <fstream>
#include "Pilib/headers/pivar.h"
#include "PiLib/headers/utility.h"

void sendHtml(std::string& request, std::ostream& output) {
    std::ifstream html("/data/pihost/build/Raspberry/Source/item.html");
    std::string buffer;
    std::ostringstream content;
    while (std::getline(html, buffer)) {
        content << buffer << newline;
    }
    output << http::http1 << http::codes::ok << newline << http::type << http::types::text::html << newline << http::len << strlen(content.str().c_str()) << newline << newline << content.str();
}

void sendBinary(std::string& request, std::ostream& output) {
    {
        std::string buffer;
        std::fstream file;
        file.open("/PiSHARE/Media/Audio/MCU/05 The First Bite Is the Deepest.m4a", std::ios::in | std::ios::binary);
        file.seekg(0, std::ios::end);
        uint size = file.tellg();
        buffer.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(&buffer[0], size);
        file.close();
        output << http::http1 << http::codes::ok << newline << http::type << "audio/aac" << newline << http::len << buffer.size() << newline << newline << buffer;
    }
}

void errorPage(const char* file, const char* code, std::ostream& output) {
    std::ifstream reader(file);
    std::string buffer;
    std::ostringstream contents;
    while (std::getline(reader, buffer)) {
        contents << buffer << newline;
    }
    buffer = contents.str();
    pilib::replace(buffer, "{{code}}", code);
    output << buffer;
}

void simpleRequest(std::string& request, std::ostream& output) {
    std::istringstream req(request);
    std::ostringstream resource;
    std::string compare, file = http::resources::root;
    std::ifstream reader;
    std::getline(req, compare, space);
    if (compare == http::req::get) {
        std::getline(req, compare, space);
        file.append(compare);
        reader.open(file);
        if (reader) {
            while (std::getline(reader, compare)) {
                resource << compare << newline;
            }
            compare = file.substr(file.find_last_of(".") + 1);
            if (compare == "html") {
                compare = http::types::text::html;
            }
            else if (compare == "txt") {
                compare = http::types::text::plain;
            }
            output << http::http1 << http::codes::ok << newline << http::type << compare << newline << http::len << resource.str().length() << newline << newline << resource.str();
        }
        else {
            file += "index.html";
            reader.open(file);
            while (std::getline(reader, compare)) {
                resource << compare << newline;
            }
            output << http::http1 << http::codes::ok << newline << http::type << http::types::text::html << newline << http::len << resource.str().length() << newline << newline << resource.str();
        }
    }
    else if (compare == http::req::head) {
        output << http::http1 << http::codes::ok << newline << http::type << http::types::text::html << newline << http::len << 0;
    }
    else {
        errorPage("/data/pihost/build/Raspberry/Source/error.html", http::codes::not_impl, resource);
        output << http::http1 << http::codes::not_impl << newline << http::type << http::types::text::html << newline << http::len << resource.str().length() << newline << newline << resource.str();
    }
}

class Server {
private:
	sockaddr_in address;
	int sock, nsock, max_users, addrlen, valread, opt = 1;
public:
	typedef void(*responsehandler)(std::string&, std::ostream&);
	std::string root;

    Server(int connections, int domain, int service, int protocol, int port, ulong interface) {
        root = http::resources::root;
        max_users = connections;
        address.sin_family = domain;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = htonl(interface);
        addrlen = sizeof(address);
        if ((sock = socket(domain, service, protocol)) == 0) {
            perror("Socket creation failure");
        }
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    }	
    
    void bindServer() {
        if (bind(sock, (sockaddr*)&address, addrlen) < 0) {
            perror("Binding socket failure ");
        }
    }
    
    void startListen() {
        if (listen(sock, max_users) < 0) {
            perror("Listening initialization failure ");
        }
    }
    
    void prep() {
        bindServer();
        startListen();
    }
    
    void launch(std::atomic_bool& condition, std::ostream& output, Server::responsehandler handler) {
        while (condition) {
            char buffer[10000];
            std::string request;
            std::ostringstream message;
            if ((nsock = accept(sock, (sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed ");
            }
            output << "Connection recieved (" << pilib::dateStamp() << ")" << newline;
            valread = read(nsock, buffer, sizeof(buffer));
            output << newline << buffer;
            request = buffer;
            handler(request, message);
            write(nsock, message.str().c_str(), strlen(message.str().c_str()));
            output << "Sent message";
            close(nsock);
            output << " -> Closed socket" << newline << newline;
            if (!condition) {
                output << "Exit detected" << newline;
                break;
            }
        }
        close(sock);
    }

    std::string fullPath(const char* file) {
        return std::string(root + file);
    }
    
    void basicHeader(std::ostream& output, const char* type, std::ostringstream& resource) {
        output << http::http1 << http::codes::ok << newline << http::type << type << newline << http::len << resource.str().length() << newline << newline << resource.str();
    }

    void internalHandler(std::string& request, std::ostream& output) {
        std::istringstream input(request);
        std::ostringstream resource;
        std::ifstream reader;
        std::string buffer;
        std::getline(input, buffer, space);
        if (buffer == "GET") {
            std::getline(input, buffer, space);
            //determine file or folder, type, available or not, then send to correct place
        }
        else if (buffer == "HEAD") {
            output << http::http1 << http::codes::ok << newline << http::type << http::types::text::plain << newline << http::len << 0;
        }
        else {
            errorPage(fullPath("/error.html").c_str(), http::codes::not_impl, resource);
            basicHeader(output, http::types::text::html, resource);
        }
    }
};