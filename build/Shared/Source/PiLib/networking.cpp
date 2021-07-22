#include "headers/networking.h"

namespace pilib {
    BaseServer::~BaseServer() {
        freeaddrinfo(list);
    }

    void BaseServer::getServerAddr() {
        int status;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        hints.ai_socktype = socktype;
        hints.ai_flags = flags;

        if ((status = getaddrinfo(ip, port, &hints, &list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(status));
            this->~BaseServer();
        }
    }
    void BaseServer::getSock() {
        if ((sockmain = socket(list->ai_family, list->ai_socktype, list->ai_protocol)) < 0) {
            perror("Error creating socket");
            this->~BaseServer();
        }
        if (reuse) {
            int yes = 1;
            if (setsockopt(sockmain, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
                perror("Error setting socket reusable");
                this->~BaseServer();
            }
        }
    }
    void BaseServer::bindSock() {
        if (bind(sockmain, list->ai_addr, list->ai_addrlen) < 0) {
            perror("Error binding socket");
            this->~BaseServer();
        }
    }
    void BaseServer::startListen() {
        if (listen(sockmain, connections) < 0) {
            perror("Error initing listen");
            this->~BaseServer();
        }
    }

    addrinfo* BaseServer::intAddr() {
        return list;
    }
    int BaseServer::intSock() {
        return sockmain;
    }

    void BaseServer::getAddrs(const char* ip, const char* port, addrinfo* hints, addrinfo** list) {
        int addr;
        if ((addr = getaddrinfo(ip, port, hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }
    void BaseServer::getAddrs(addrinfo** list, const char* ip, const char* port, int family, int socktype, int flags) {
        int addr;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        hints.ai_socktype = socktype;
        hints.ai_flags = flags;

        if ((addr = getaddrinfo(ip, port, &hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }
    void BaseServer::getAddr4(addrinfo** list, const char* ip, const char* port, int socktype, int flags) {
        int addr;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = socktype;
        hints.ai_flags = flags;

        if ((addr = getaddrinfo(ip, port, &hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }
    void BaseServer::getAddr6(addrinfo** list, const char* ip, const char* port, int socktype, int flags) {
        int addr;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET6;
        hints.ai_socktype = socktype;
        hints.ai_flags = flags;

        if ((addr = getaddrinfo(ip, port, &hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }
    void BaseServer::getServerAddr(addrinfo** list, const char* port, int family, int socktype) {
        int addr;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        hints.ai_socktype = socktype;
        hints.ai_flags = AI_PASSIVE;

        if ((addr = getaddrinfo(NULL, "80", &hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }
    void BaseServer::getServerAddr(addrinfo** list) {
        int addr;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if ((addr = getaddrinfo(NULL, "80", &hints, list)) != 0) {
            fprintf(stderr, "Error getting address: %s\n", gai_strerror(addr));
        }
    }

    void BaseServer::addrListVec(addrinfo* list, std::vector<addrinfo>& vec) {
        for (addrinfo* a = list; a != NULL; a = a->ai_next) {
            vec.push_back(*a);
        }
        freeaddrinfo(list);
    }

    int BaseServer::getSock(addrinfo* addr, bool reuse) {
        int sock;
        if ((sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
            perror("Error creating socket");
        }
        if (reuse) {
            uint8_t yes = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
                perror("Error setting socket reusable");
            }
        }
        return sock;
    }
    int BaseServer::getSock(const addrinfo& addr, bool reuse) {
        int sock;
        if ((sock = socket(addr.ai_family, addr.ai_socktype, addr.ai_protocol)) < 0) {
            perror("Error creating socket");
        }
        if (reuse) {
            uint8_t yes = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
                perror("Error setting socket reusable");
            }
        }
        return sock;
    }
    int BaseServer::bindNewSock(addrinfo* addr, bool reuse) {
        int sock;
        if ((sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
            perror("Error creating socket");
        }
        if (reuse) {
            uint8_t yes = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
                perror("Error setting socket reusable");
            }
        }
        if (bind(sock, addr->ai_addr, addr->ai_addrlen) < 0) {
            perror("Error binding socket");
        }
        return sock;
    }
    void BaseServer::bindSock(int sock, addrinfo* addr) {
        if (bind(sock, addr->ai_addr, addr->ai_addrlen) < 0) {
            perror("Error binding socket");
        }
    }

    void BaseServer::startListen(int sock, int connections) {
        if (listen(sock, connections) < 0) {
            perror("Error initing listen");
        }
    }

    namespace http {
        const std::map<std::string, Method> Methods::typemap = {
            {"GET", Method::GET},
            {"HEAD", Method::HEAD},
            {"POST", Method::POST},
            {"PUT", Method::PUT},
            {"DELETE", Method::DELETE},
            {"TRACE", Method::TRACE},
            {"OPTIONS", Method::OPTIONS},
            {"CONNECT", Method::CONNECT},
            {"PATCH", Method::PATCH},
        };
        const std::map<Method, std::string> Methods::stringmap = {
            {Method::GET, "GET"},
            {Method::HEAD, "HEAD"},
            {Method::POST, "POST"},
            {Method::PUT, "PUT"},
            {Method::DELETE, "DELETE"},
            {Method::TRACE, "TRACE"},
            {Method::OPTIONS, "OPTIONS"},
            {Method::CONNECT, "CONNECT"},
            {Method::PATCH, "PATCH"},
        };

        Method Methods::getType(const std::string& str) {
            auto search = typemap.find(str);
            return search->second;
        }
        std::string Methods::getString(Method method) {
            auto search = stringmap.find(method);
            return search->second;
        }

        const std::map<std::string, Version> Versions::typemap = {
            {"HTTP/1.0", Version::HTTP_1_0},
            {"HTTP/1.1", Version::HTTP_1_1},
            {"HTTP/2.0", Version::HTTP_2_0},
        };
        const std::map<Version, std::string> Versions::stringmap = {
            {Version::HTTP_1_0, "HTTP/1.0"},
            {Version::HTTP_1_1, "HTTP/1.1"},
            {Version::HTTP_2_0, "HTTP/2.0"},
        };

        Version Versions::getType(const std::string& str) {
            auto search = typemap.find(str);
            return search->second;
        }
        std::string Versions::getString(Version version) {
            auto search = stringmap.find(version);
            return search->second;
        }

        Segment::Segment(const std::string& segment) {
            std::istringstream headstream(segment);
            std::getline(headstream, this->key, ':');   //check direct access works
            std::getline(headstream, this->value);
            value.erase(std::remove_if(std::begin(this->value), std::end(this->value), [](char c) {return std::isspace(c); }), std::end(this->value));
        }
        Segment::Segment(const std::string&& segment) {
            std::istringstream headstream(std::move(segment));
            std::getline(headstream, this->key, ':');
            std::getline(headstream, this->value);
            value.erase(std::remove_if(std::begin(this->value), std::end(this->value), [](char c) {return std::isspace(c); }), std::end(this->value));
        }

        std::string Segment::getKey() {
            return this->key;
        }
        std::string Segment::getValue() {
            return this->value;
        }

        std::string* Segment::intKey() {
            return &(this->key);
        }
        std::string* Segment::intValue() {
            return &(this->value);
        }

        std::string Segment::getSerialized() {
            std::ostringstream segment;
            segment << this->key << ": " << this->value << endline;
            return segment.str();
        }
        std::string Segment::getSerialized(const std::string& key, const std::string& value) {
            std::ostringstream segment;
            segment << key << ": " << value << endline;
            return segment.str();
        }
        Segment Segment::getDeserialized(const std::string& segment) {
            std::istringstream headstream(segment);
            std::string key;
            std::string value;
            std::getline(headstream, key, ':');
            std::getline(headstream, value);
            value.erase(std::remove_if(std::begin(value), std::end(value), [](char c) {return std::isspace(c); }), std::end(value));
            return Segment(std::move(key), std::move(value));
        }

        Request::Request(Method method, const std::string& resource, std::vector<Segment>& headers, Version version) : method(method), resource(resource), version(version) {
            for (int i = 0; i < headers.size(); i++) {
                this->headers.insert(std::pair<std::string, std::string>(headers[i].getKey(), headers[i].getValue()));
            }
        }

        Request::Request(const std::string& request) {
            std::istringstream rstream(request);
            std::string buffer;
            std::getline(rstream, buffer, space);
            this->method = Methods::getType(buffer);
            std::getline(rstream, this->resource, space);
            std::getline(rstream, buffer);
            pilib::clearEnd(buffer);
            this->version = Versions::getType(buffer);
            while (std::getline(rstream, buffer)) {
                //check for blank line (ex. POST -> deal with the payload)
                pilib::clearEnd(buffer);
                Segment line(std::move(buffer));
                this->headers.insert(std::pair<std::string, std::string>(line.getKey(), line.getValue()));
            }
        }

        std::string Request::getSerialized() {
            std::ostringstream buffer;
            buffer << Methods::getString(this->method) << space << this->resource << space << Versions::getString(this->version) << endline;
            for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
                buffer << Segment::getSerialized(it->first, it->second) << endline;
            }
            return buffer.str();
        }

        Version* Request::intVersion() {
            return &(this->version);
        }
        Method* Request::intMethod() {
            return &(this->method);
        }
        std::string* Request::intResource() {
            return &(this->resource);
        }
        std::map<std::string, std::string>* Request::intHeaders() {
            return &(this->headers);
        }

        std::vector<Segment> Request::getHeaders() {
            std::vector<Segment> vec;
            for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
                vec.emplace_back(std::move(Segment(it->first, it->second)));
            }
            return vec;
        }

        std::string Request::getSerialized(Method method, const std::string& resource, std::vector<Segment>& headers, Version version) {
            std::ostringstream buffer;
            buffer << Methods::getString(method) << space << resource << space << Versions::getString(version) << endline;
            for (int i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            return buffer.str();
        }

        void Request::getSerialized(std::ostream& buffer, Method method, const std::string& resource, std::vector<Segment> headers, Version version) {
            buffer << Methods::getString(method) << space << resource << space << Versions::getString(version) << endline;
            for (int i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
        }
    }

    void errorPage(const char* file, const char* code, std::ostream& output) {
        std::ifstream reader(file);
        std::string buffer((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
        pilib::replace(buffer, "{{code}}", code);
        output << buffer;
    }

    void sendHtml(std::ostream& output) {
        std::ostringstream content;
        errorPage("/data/pihost/resources/error.html", http::codes::not_impl, content);
        output << http::http11 << http::codes::ok << newline << http::type << http::types::text::html << newline << http::len << strlen(content.str().c_str()) << newline << newline << content.str();
    }

    std::string HttpHandler::fullPath(const char* item) {
        std::string temp = root;
        if (item = "/") {
            temp.append(http::resources::home);
            return temp;
        }
        temp.append(item);
        return temp;
    }

    HttpServer::HttpServer(const char* root, int max_accepts) : BaseServer(NULL, "http", max_accepts), handler(root) {
        getServerAddr();
        pilib::debug("HttpServer created - got address successfully!");
    }

    void HttpServer::prepServer() {
        getSock();
        bindSock();
        startListen();
    }

    void HttpServer::server1_0(const std::atomic_bool& rc, std::ostream& out) {
        int nsock;
        prepServer();
        while (rc) {
            char buffer[10000];
            sockaddr_storage naddr;
            socklen_t naddrlen = sizeof(naddr);
            if ((nsock = accept(intSock(), (sockaddr*)&naddr, &naddrlen)) < 0) {
                perror("Error accepting connection");
                continue;
            }
            char ip[INET_ADDRSTRLEN];
            inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
            out << "Got connection from: " << ip << newline;
            recv(nsock, buffer, sizeof(buffer), 0);
            out << "Got request from: " << ip << newline << buffer << newline;
            //handle
            close(nsock);
            out << "Closed connection to: " << ip << newline;
        }
        close(intSock());
    }

    void HttpServer::serve(const std::atomic_bool& rc, std::ostream& out) {
        int nsock, readlen;
        prepServer();
        while (rc) {
            char buffer[10000];
            sockaddr_storage naddr;
            socklen_t naddrlen = sizeof(naddr);
            if ((nsock = accept(intSock(), (sockaddr*)&naddr, &naddrlen)) < 0) {
                perror("Error accepting connection");
                continue;
            }
            char ip[INET_ADDRSTRLEN];
            inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
            out << "Got connection from: " << ip << newline;
            while ((readlen = recv(nsock, buffer, sizeof(buffer), 0)) > 0) {
                out << "Got request from: " << ip << " {len: " << readlen << "}:" << newline << buffer << newline;
                if (!rc) {
                    send(nsock, NULL, 0, 0);
                }
                else {
                    std::string request = buffer;
                    http::Request req(request);
                    debug(http::Methods::getString(*req.intMethod()));
                    debug(*req.intResource());
                    debug(http::Versions::getString(*req.intVersion()));
                    std::ostringstream write;
                    sendHtml(write);
                    send(nsock, write.str().c_str(), strlen(write.str().c_str()), 0); //handle buffer
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
            if (readlen < 0) {
                perror("Recv error");
            }
            out << "Connection ended: " << ip << " {check: " << readlen << "}" << newline;
            close(nsock);
        }
        close(intSock());
    }
}