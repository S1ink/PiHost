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
        const std::unordered_map<std::string, Version> Versions::typemap = {
            {"HTTP/1.0", Version::HTTP_1_0},
            {"HTTP/1.1", Version::HTTP_1_1},
            {"HTTP/2.0", Version::HTTP_2_0},
        };
        const std::unordered_map<Version, std::string> Versions::stringmap = {
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
        const std::unordered_map<std::string, Method> Methods::typemap = {
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
        const std::unordered_map<Method, std::string> Methods::stringmap = {
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
        const std::unordered_map<std::string, Code> Codes::typemap = {
            {"100 Continue", Code::CONTINUE}, {"200 OK", Code::OK}, 
            {"201 Created", Code::CREATED}, {"202 Accepted", Code::ACCEPTED}, 
            {"204 No Content", Code::NO_CONTENT}, {"205 Reset Content", Code::RESET_CONTENT}, 
            {"206 Partial Content", Code::PARTIAL_CONTENT}, 
            {"300 Multiple Choices", Code::MULTIPLE_CHOICES}, 
            {"301 Moved Permanently", Code::MOVED_PERMANENTLY}, 
            {"302 Found", Code::FOUND}, {"400 Bad Request", Code::BAD_REQUEST}, 
            {"401 Unauthorized", Code::UNAUTHORIZED}, 
            {"402 Payment Required", Code::PAYMENT_REQUIRED}, 
            {"403 Forbidden", Code::FORBIDDEN},  {"404 Not Found", Code::NOT_FOUND}, 
            {"405 Method Not Allowed", Code::METHOD_NOT_ALLOWED}, 
            {"406 Not Acceptable", Code::NOT_ACCEPTABLE}, 
            {"408 Request Timeout", Code::REQUEST_TIMEOUT}, {"409 Conflict", Code::CONFLICT}, 
            {"410 Gone", Code::GONE}, {"411 Length Required", Code::LENGTH_REQUIRED}, 
            {"414 URI Too Long", Code::URI_TOO_LONG}, 
            {"415 Unsupported Media Type", Code::UNSUPPORTED_MEDIA_TYPE},
            {"418 I'm a teapot", Code::IM_A_TEAPOT}, 
            {"500 Internal Server Error", Code::INTERNAL_SERVER_ERROR},
            {"501 Not Implemented", Code::NOT_IMPLEMENTED}, {"502 Bad Gateway", Code::BAD_GATEWAY}, 
            {"503 Service Unavailable", Code::SERVICE_UNAVAILABLE}, 
            {"504 Gateway Timeout", Code::GATEWAY_TIMEOUT},
            {"505 HTTP Version Not Supported", Code::HTTP_VERSION_NOT_SUPPORTED}, 
            {"510 Not Extended", Code::NOT_EXTENDED}, 
            {"511 Network Authentication Required", Code::NETWORK_AUTHICATION_REQIRED},
        };
        const std::unordered_map<Code, std::string> Codes::stringmap = {
            {Code::CONTINUE, "100 Continue"}, {Code::OK, "200 OK"},
            {Code::CREATED, "201 Created"}, {Code::ACCEPTED, "202 Accepted"},
            {Code::NO_CONTENT, "204 No Content"}, {Code::RESET_CONTENT, "205 Reset Content"},
            {Code::PARTIAL_CONTENT, "206 Partial Content"},
            {Code::MULTIPLE_CHOICES, "300 Multiple Choices"},
            {Code::MOVED_PERMANENTLY, "301 Moved Permanently"},
            {Code::FOUND, "302 Found"}, {Code::BAD_REQUEST, "400 Bad Request"},
            {Code::UNAUTHORIZED, "401 Unauthorized"},
            {Code::PAYMENT_REQUIRED, "402 Payment Required"},
            {Code::FORBIDDEN, "403 Forbidden"}, {Code::NOT_FOUND, "404 Not Found"},
            {Code::METHOD_NOT_ALLOWED, "405 Method Not Allowed"},
            {Code::NOT_ACCEPTABLE, "406 Not Acceptable"},
            {Code::REQUEST_TIMEOUT, "408 Request Timeout"}, {Code::CONFLICT, "409 Conflict"},
            {Code::GONE, "410 Gone"}, {Code::LENGTH_REQUIRED, "411 Length Required"},
            {Code::URI_TOO_LONG, "414 URI Too Long"},
            {Code::UNSUPPORTED_MEDIA_TYPE, "415 Unsupported Media Type"},
            {Code::IM_A_TEAPOT, "418 I'm a teapot"},
            {Code::INTERNAL_SERVER_ERROR, "500 Internal Server Error"},
            {Code::NOT_IMPLEMENTED, "501 Not Implemented"}, {Code::BAD_GATEWAY, "502 Bad Gateway"},
            {Code::SERVICE_UNAVAILABLE, "503 Service Unavailable"},
            {Code::GATEWAY_TIMEOUT, "504 Gateway Timeout"},
            {Code::HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported"},
            {Code::NOT_EXTENDED, "510 Not Extended"},
            {Code::NETWORK_AUTHICATION_REQIRED, "511 Network Authentication Required"},
        };
        Code Codes::getType(const std::string& str) {
            std::istringstream extract(str);
            int code;
            extract >> code;
            if (code) {
                return Code(code);
            }
            else {
                auto search = typemap.find(str);
                return search->second;
            }
        }
        Code Codes::getType(int code) {
            return Code(code);
        }
        std::string Codes::getString(Code code) {
            auto search = stringmap.find(code);
            return search->second;
        }
        std::string Codes::getString(int code) {
            auto search = stringmap.find(Code(code));
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

        Response::Response(Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version) : responsecode(responsecode), body(body), version(version) {
            for (int i = 0; i < headers.size(); i++) {
                this->headers.insert(std::pair<std::string, std::string>(headers[i].getKey(), headers[i].getValue()));
            }
        }

        Response::Response(const std::string& response) {
            std::istringstream rstream(response);
            std::string buffer;
            std::getline(rstream, buffer, space);
            this->responsecode = Codes::getType(buffer);
            std::getline(rstream, buffer);
            pilib::clearEnd(buffer);
            this->version = Versions::getType(buffer);
            while (std::getline(rstream, buffer)) {
                pilib::clearEnd(buffer);
                if (buffer.empty()) {
                    break;
                }
                Segment line(std::move(buffer));
                this->headers.insert(std::pair<std::string, std::string>(line.getKey(), line.getValue()));
            }
            this->body = response.substr(rstream.tellg());
        }

        std::string Response::getSerialized() {
            std::ostringstream buffer;
            buffer << Codes::getString(this->responsecode) << space << Versions::getString(this->version) << endline;
            for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
                buffer << Segment::getSerialized(it->first, it->second) << endline;
            }
            buffer << endline << this->body << endline;
            return buffer.str();
        }

        void Response::lateConstruct(Code responsecode, std::vector<Segment>& headers, const std::string& body) {
            this->responsecode = responsecode;
            this->body = body;
            for (int i = 0; i < headers.size(); i++) {
                this->headers.insert(std::pair<std::string, std::string>(headers[i].getKey(), headers[i].getValue()));
            }
        }

        Code* Response::intCode() {
            return &(this->responsecode);
        }
        Version* Response::intVersion() {
            return &(this->version);
        }
        std::string* Response::intBody() {
            return &(this->body);
        }
        std::map<std::string, std::string>* Response::intHeaders() {
            return &(this->headers);
        }

        std::vector<Segment> Response::getHeaders() {
            std::vector<Segment> vec;
            for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
                vec.emplace_back(std::move(Segment(it->first, it->second)));
            }
            return vec;
        }

        uint Response::bodyLen() {
            return this->body.length();
        }

        std::string Response::getSerialized(Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version) {
            std::ostringstream buffer;
            buffer << Codes::getString(responsecode) << space << Versions::getString(version) << endline;
            for (int i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            buffer << endline << body << endline;
            return buffer.str();
        }
        void Response::getSerialized(std::ostream& buffer, Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version) {
            buffer << Codes::getString(responsecode) << space << Versions::getString(version) << endline;
            for (int i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            buffer << endline << body << endline;
        }

        std::string HttpHandler::rPath(const char* item) {
            std::string temp = root;
            if (item = "/") {
                temp.append(resources::home);
                return temp;
            }
            //check for PiSHARE path 
            temp.append(item);
            return temp;
        }

        void HttpHandler::respond(int socket, const std::string& input) {
            Request req(input);
            Response response;
            std::vector<Segment> headers;
            switch (*req.intMethod()) {
            case Method::GET:

            case Method::HEAD:

            case Method::CONNECT:
            case Method::DELETE:
            case Method::OPTIONS:
            case Method::PATCH:
            case Method::POST:
            case Method::PUT:
            case Method::TRACE:
            default:
                std::ifstream reader(rPath("/error.html"));
                std::string body((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::NOT_IMPLEMENTED).c_str());
                headers.emplace_back(Segment("Type", "text/html"));
                std::ostringstream buf;
                buf << body.length();
                headers.emplace_back(Segment("Length", buf.str()));
                response.lateConstruct(Code::NOT_IMPLEMENTED, headers, body);
            }
            std::string ret = response.getSerialized();
            this->state.sent = send(socket, ret.c_str(), ret.length(), 0);
        }

        HttpServer::HttpServer(const char* root, int max_accepts, Version version) : BaseServer(NULL, "http", max_accepts), handler(root, version), version(version) {
            getServerAddr();
            pilib::debug("HttpServer created - got address successfully!");
        }

        void HttpServer::prepServer() {
            getSock();
            bindSock();
            startListen();
        }

        void HttpServer::serve(const std::atomic_bool& rc, std::ostream& out) {
            switch (this->version) {
            case Version::HTTP_1_0:
                serve1_0(rc, out);
                break;
            case Version::HTTP_1_1:
                serve1_1(rc, out);
                break;
            case Version::HTTP_2_0:
            default:
                out << "HTTP version not supported." << newline;
            }
        }

        void HttpServer::serve1_0(const std::atomic_bool& rc, std::ostream& out) {
            prepServer();
            int nsock;
            while (rc) {
                char buffer[10000];
                sockaddr_storage naddr;
                socklen_t naddrlen = sizeof(naddr);
                if ((nsock = accept(intSock(), (sockaddr*)&naddr, &naddrlen)) < 0) {
                    perror("Error accepting connection");
                    continue;
                }
                /*char ip[INET_ADDRSTRLEN];
                inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
                out << "Got connection from: " << ip << newline;*/
                if (recv(nsock, buffer, sizeof(buffer), 0) < 0) {
                    perror("Recv error");
                }
                //out << "Got request from: " << ip << newline << buffer << newline;
                this->handler.respond(nsock, std::string(buffer));
                close(nsock);
                //out << "Closed connection to: " << ip << newline;
            }
            close(intSock());
        }

        void HttpServer::serve1_1(const std::atomic_bool& rc, std::ostream& out) {
            prepServer();
            int nsock, readlen;
            while (rc) {
                char buffer[10000];
                sockaddr_storage naddr;
                socklen_t naddrlen = sizeof(naddr);
                if ((nsock = accept(intSock(), (sockaddr*)&naddr, &naddrlen)) < 0) {
                    perror("Error accepting connection");
                    continue;
                }
                /*char ip[INET_ADDRSTRLEN];
                inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
                out << "Got connection from: " << ip << newline;*/
                while ((readlen = recv(nsock, buffer, sizeof(buffer), 0)) > 0) {
                    //out << "Got request from: " << ip << " {len: " << readlen << "}:" << newline << buffer << newline;
                    if (!rc) {
                        send(nsock, NULL, 0, 0);
                    }
                    else {
                        this->handler.respond(nsock, std::string(buffer));
                        memset(&buffer, 0, sizeof(buffer));
                    }
                }
                if (readlen < 0) {
                    perror("Recv error");
                }
                //out << "Connection ended: " << ip << " {check: " << readlen << "}" << newline;
                close(nsock);
            }
            close(intSock());
        }
    }

    void errorPage(const char* file, pilib::http::Code code, std::ostream& output) {
        std::ifstream reader(file);
        std::string buffer((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
        pilib::replace(buffer, "{{code}}", pilib::http::Codes::getString(code).c_str());
        output << buffer;
    }

    void sendHtml(std::ostream& output) {
        std::ostringstream content;
        errorPage("/data/pihost/resources/error.html", pilib::http::Code::NOT_IMPLEMENTED, content);
        output << pilib::http::Versions::getString(pilib::http::Version::HTTP_1_1) 
            << pilib::http::Codes::getString(pilib::http::Code::OK) << newline << "Type: " 
            << http::types::text::html << newline << "Length: " << strlen(content.str().c_str()) << newline << newline << content.str();
    }
}