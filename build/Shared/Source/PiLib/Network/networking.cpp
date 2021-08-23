#include "networking.h"

namespace pilib {
    void getSockIp(int socket, char* ip) {
        sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        getpeername(socket, (sockaddr*)&addr, &addrlen);
        if (addr.ss_family == AF_INET) {
            inet_ntop(AF_INET, &(((sockaddr_in*)&addr)->sin_addr), ip, INET_ADDRSTRLEN);
        }
        else {
            inet_ntop(AF_INET6, &(((sockaddr_in6*)&addr)->sin6_addr), ip, INET6_ADDRSTRLEN);
        }
    }

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
            if (search != typemap.end()) {
                return search->second;
            }
            return Version::ERROR;
        }
        std::string Versions::getString(Version version) {
            auto search = stringmap.find(version);
            if (search != stringmap.end()) {
                return search->second;
            }
            return std::string();
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
            if (search != typemap.end()) {
                return search->second;
            }
            return Method::ERROR;
        }
        std::string Methods::getString(Method method) {
            auto search = stringmap.find(method);
            if (search != stringmap.end()) {
                return search->second;
            }
            return std::string();
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
                if (search != typemap.end()) {
                    return search->second;
                }
                return Code::ERROR;
            }
        }
        Code Codes::getType(int code) { //find a way to check param
            return Code(code);
        }
        std::string Codes::getString(Code code) {
            auto search = stringmap.find(code);
            if (search != stringmap.end()) {
                return search->second;
            }
            return std::string();
        }
        std::string Codes::getString(int code) {
            auto search = stringmap.find(Code(code));
            if (search != stringmap.end()) {
                return search->second;
            }
            return std::to_string(code);
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
        Segment::Segment(std::pair<std::string, std::string> pair) {
            this->key = pair.first;
            this->value = pair.second;
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
            segment << this->key << ": " << this->value;
            return segment.str();
        }
        std::string Segment::getSerialized(const std::string& key, const std::string& value) {
            std::ostringstream segment;
            segment << key << ": " << value;
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

        HeaderList::HeaderList(const size_t size) {
            this->headers.reserve(size);
        }
        HeaderList::HeaderList(const std::string& body) {
            this->add(body);
        }

        void HeaderList::add(const Segment& header) {
            this->headers.emplace_back(header);
        }
        void HeaderList::add(const std::string& key, const std::string& value) {
            this->headers.emplace_back(key, value);
        }
        void HeaderList::add(const std::vector<Segment>& list) {
            this->headers.reserve((list.size()) + (this->headers.size()));
            this->headers.insert(this->headers.end(), list.begin(), list.end());
        }
        void HeaderList::add(const std::string& body) {
            std::istringstream text(body);
            std::string buffer;
            while (std::getline(text, buffer)) {
                pilib::clearEnd(buffer);
                this->headers.emplace_back(buffer);
            }
        }

        void HeaderList::reserve(size_t size) {
            this->headers.reserve(size);
        }
        void HeaderList::reset() {
            this->headers.clear();
            this->headers.shrink_to_fit();
        }

        std::string HeaderList::find(std::string key) {
            auto map = HeaderList::headerMap(this->headers);
            auto itr = map.find(key);
            if (itr != map.end()) {
                return itr->second;
            }
            return std::string();
        }
        std::string HeaderList::allHeaders() {
            std::ostringstream buffer;
            for (uint i = 0; i < this->headers.size(); i++) {
                buffer << this->headers[i].getSerialized() << endline;
            }
            return buffer.str();
        }

        std::vector<Segment>* HeaderList::intHeaders() {
            return &(this->headers);
        }

        std::unordered_map<std::string, std::string> HeaderList::headerMap(std::vector<Segment>& headers) {
            std::unordered_map<std::string, std::string> ret;
            for (uint i = 0; i < headers.size(); i++) {
                ret.insert(std::make_pair(headers[i].getKey(), headers[i].getValue()));
            }
            return ret;
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
            if ((this->method != Method::ERROR) && (this->version != Version::ERROR)) { //class error var for future reference?
                while (std::getline(rstream, buffer)) {
                    //check for blank line (ex. POST -> deal with the payload)
                    pilib::clearEnd(buffer);
                    this->headers.add(buffer);  //IMPLEMENT CHECKS!!!
                }
            }
        }

        std::string Request::getSerialized() {
            std::ostringstream buffer;
            buffer << Methods::getString(this->method) 
                << space << this->resource 
                << space << Versions::getString(this->version) 
                << endline
                << this->headers.allHeaders();
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
        HeaderList* Request::intHeaders() {
            return &(this->headers);
        }

        std::string Request::getSerialized(Method method, const std::string& resource, std::vector<Segment>& headers, Version version) {
            std::ostringstream buffer;
            buffer << Methods::getString(method) << space << resource << space << Versions::getString(version) << endline;
            for (uint i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            return buffer.str();
        }

        void Request::getSerialized(std::ostream& buffer, Method method, const std::string& resource, std::vector<Segment>& headers, Version version) {
            buffer << Methods::getString(method) << space << resource << space << Versions::getString(version) << endline;
            for (uint i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
        }

        Response::Response(const std::string& response) {   //implement checks when ready to use this form of initialization
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
                this->headers.add(buffer);
            }
            this->body = response.substr(rstream.tellg());
        }

        void Response::update(Code responsecode, const std::vector<Segment>& headers, const std::string& body) {
            this->responsecode = responsecode;
            this->body = body;
            this->headers.add(headers);
        }
        void Response::update(Code responsecode, HeaderList& headers, const std::string& body) {
            this->responsecode = responsecode;
            this->body = body;
            this->headers.add(*headers.intHeaders());
        }

        std::string Response::getSerialized() {
            std::ostringstream buffer;
            buffer << Versions::getString(this->version) << space << Codes::getString(this->responsecode) << endline;
            buffer << this->headers.allHeaders();
            if (this->body.length() > 0) {
                buffer << endline << this->body << endline;
            }
            return buffer.str();
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
        HeaderList* Response::intHeaders() {
            return &(this->headers);
        }

        uint Response::bodyLen() {
            return this->body.length();
        }

        std::string Response::getSerialized(Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version) {
            std::ostringstream buffer;
            buffer << Codes::getString(responsecode) << space << Versions::getString(version) << endline;
            for (uint i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            buffer << endline << body << endline;
            return buffer.str();
        }
        void Response::getSerialized(std::ostream& buffer, Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version) {
            buffer << Codes::getString(responsecode) << space << Versions::getString(version) << endline;
            for (uint i = 0; i < headers.size(); i++) {
                buffer << headers[i].getSerialized() << endline;
            }
            buffer << endline << body << endline;
        }

        std::string HttpHandler::find(const char* item) {
            std::string path = root;
            if (std::string(item) == "/") {
                path.append("/index.html");
            }
            //check for special specifier paths here
            else {
                path.append(item);
            }
            return path;
        }

        //fix logging -> pass all output to external || create a format struct that can be parsed so that logging can be selective based on outside env (which func)
        void HttpHandler::respond(const int socket, const std::string& input) { 
            Request req(input); //CHECK FOR VALID HTTP
            HeaderList headers;
            std::ifstream reader;
            std::string body, path = find((*req.intResource()).c_str());

            (this->response.intHeaders())->reset();
            std::cout << path << newline;   //continues line from before this func in serve_beta()

            headers.add(   //headers that apply to all
                (Segment){"Server", "Custom C++ HTTP Server (Raspberry Pi)"}
            );

            switch (*(req.intMethod())) {
            case Method::GET:   //requesting resource
            {
                reader.open(path.c_str(), std::ios::binary);    //attempt to open resource
                if (reader.is_open()) { //if resource exists
                    body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                    headers.add({
                        {"Content-Type", getMegaMimeType(path.c_str())},
                        {"Content-Length", std::to_string(body.length())}
                    });
                    if (this->version == Version::HTTP_1_0) {   //close connection if http 1.0
                        headers.add("Connection", "close");
                    }
                    else {
                        headers.add("Connection", "keep-alive");
                    }
                    this->response.update(Code::OK, headers, body);
                }
                else {  //send 404
                    reader.open(find("/error.html"));
                    if (reader.is_open()) { //check if error page exists
                        body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                        pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::NOT_FOUND).c_str());
                        headers.add(
                            (Segment){"Content-Type", "text/html"}
                        );
                    }
                    else {  //if not send plain text
                        body = "{Error page not found} - Error: 404 Not Found";
                        headers.add(
                            (Segment){ "Content-Type", "text/plain" }
                        );
                    }
                    headers.add({
                        {"Content-Length", std::to_string(body.length())},
                        {"Connection", "close"},
                    });
                    this->response.update(Code::NOT_FOUND, headers, body);
                    
                }
                break;
            }
            break;
            case Method::HEAD:  //requesting header only
            {
                reader.open(path, std::ios::binary);    //attempt to open resource
                if (reader.is_open()) { //if exists
                    body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                    headers.add({
                        {"Content-Type", getMegaMimeType(path.c_str())},
                        {"Content-Length", std::to_string(body.length())}
                    });
                    this->response.update(Code::OK, headers);
                }
                else {  //else send 404
                    reader.open(find("/error.html"));
                    if (reader.is_open()) { //check error page exists
                        body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                        pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::NOT_FOUND).c_str());
                        headers.add(
                            (Segment){"Content-Type", "text/html"}
                        );
                    }
                    else {  //if not send text
                        body = "{Error page not found} - Error: 404 Not Found";
                        headers.add(
                            (Segment) {"Content-Type", "text/plain"}
                        );
                    }
                    headers.add({
                        {"Content-Length", std::to_string(body.length())},
                        {"Connection", "close"},
                    });
                    this->response.update(Code::NOT_FOUND, headers);
                }
                break;
            }
            break;
            case Method::CONNECT:
            case Method::DELETE:
            case Method::OPTIONS:
            case Method::PATCH:
            case Method::POST:
            case Method::PUT:
            case Method::TRACE: //methods not supported
            {
                reader.open(find("/error.html"));
                if (reader.is_open()) { //check if page exists
                    body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                    pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::METHOD_NOT_ALLOWED).c_str());
                    headers.add(
                        (Segment){"Content-Type", "text/html"}
                    );
                }
                else {  //else send plain text
                    body = "{Error page not found} - Error: 405 Method Not Allowed";
                    headers.add(
                        (Segment){"Content-Type", "text/plain"}
                    );
                }   
                headers.add({
                    {"Allow", "GET, HEAD"}, //only GET and HEAD are suppored
                    {"Content-Length", std::to_string(body.length())},
                    {"Connection", "close"},
                });
                this->response.update(Code::METHOD_NOT_ALLOWED, headers, body);
                break;
            }
            case Method::ERROR: //method is the first thing that is parsed, so an error means that the request was invalid
            {
                reader.open(find("/error.html"));
                if (reader.is_open()) { //check if page exists
                    body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                    pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::BAD_REQUEST).c_str());
                    headers.add(
                        (Segment) {
                        "Content-Type", "text/html"
                    }
                    );
                }
                else {  //else send plain text
                    body = "{Error page not found} - Error: 400 Bad Request";
                    headers.add(
                        (Segment) {
                        "Content-Type", "text/plain"
                    }
                    );
                }
                headers.add({
                    {"Content-Length", std::to_string(body.length())},
                    {"Connection", "close"},
                    });
                this->response.update(Code::BAD_REQUEST, headers, body);
                break;
            }
            default:    //defaults to 501 NOT IMPLEMENTED
            {
                reader.open(find("/error.html"));
                if (reader.is_open()) { //check if page exists
                    body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                    pilib::replace(body, "{{code}}", pilib::http::Codes::getString(Code::NOT_IMPLEMENTED).c_str());
                    headers.add(
                        (Segment){"Content-Type", "text/html"}
                    );
                }
                else {  //else send plain text
                    body = "{Error page not found} - Error: 501 Not Implemented";
                }
                headers.add({
                    {"Content-Length", std::to_string(body.length())},
                    {"Connection", "close"},
                });
                this->response.update(Code::NOT_IMPLEMENTED, headers, body);
                break;
            }
            }

            std::string ret = this->response.getSerialized();
            //debug(ret);
            this->sent = send(socket, ret.c_str(), ret.length(), 0);
            std::cout << pilib::dateStamp() << " : Sent {" << this->sent << "} bytes ";
            return;
        }

        Version HttpHandler::getVer() {
            return this->version;
        }

        HttpServer::HttpServer(Version version, const char* root, int max_clients) : BaseServer(NULL, "http", max_clients), handler(root, version) {
            getServerAddr();
        }

        void HttpServer::prepServer() {
            getSock();
            bindSock();
            startListen();
        }

        void HttpServer::serve(const std::atomic_bool& rc, std::ostream& out) {
            switch (this->handler.getVer()) {
            case Version::HTTP_1_0:
                s_serve1_0(rc, out);
                break;
            case Version::HTTP_1_1:
                s_serve1_1(rc, out);
                break;
            case Version::HTTP_2_0:
            default:
                out << "HTTP version not supported." << newline;
            }
        }

        //add timestamps to outputs
        void HttpServer::s_serve1_0(const std::atomic_bool& rc, std::ostream& out) {
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
                char ip[INET_ADDRSTRLEN];
                inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
                out << "Got connection from: " << ip << newline;
                if (recv(nsock, buffer, sizeof(buffer), 0) < 0) {
                    perror("Recv error");
                }
                out << "Got request from: " << ip << newline << buffer << newline;
                this->handler.respond(nsock, std::string(buffer));
                close(nsock);
                out << "Closed connection to: " << ip << newline;
            }
            close(intSock());
        }

        //add timestamps to outputs
        void HttpServer::s_serve1_1(const std::atomic_bool& rc, std::ostream& out) {
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
                char ip[INET_ADDRSTRLEN];
                inet_ntop(naddr.ss_family, &(((sockaddr_in*)&naddr)->sin_addr), ip, sizeof(ip));
                out << "Got connection from: " << ip << newline;
                while ((readlen = recv(nsock, buffer, sizeof(buffer), 0)) > 0) {
                    out << "Got request from: " << ip << " {len: " << readlen << "}:" << newline << buffer << newline;
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
                out << "Connection ended: " << ip << " {check: " << readlen << "}" << newline;
                close(nsock);
            }
            close(intSock());
        }

        void HttpServer::serve_beta(const std::atomic_bool& rc) {
            prepServer();
            pilib::debug("WebServer created sucessfully - starting...\n");

            int nsock, readlen, maxfd = intSock();
            char buffer[10000];
            char ipbuff[INET6_ADDRSTRLEN];

            sockaddr_storage naddr;
            socklen_t naddrlen = sizeof(naddr);

            timeval tbuff, checkup = { 1, 0 };
            fd_set master, fdbuff;
            FD_ZERO(&master);
            FD_ZERO(&fdbuff);
            FD_SET(intSock(), &master);

            while (rc) {
                tbuff = checkup;
                fdbuff = master;
                if (select((maxfd + 1), &fdbuff, NULL, NULL, &tbuff) == -1) {
                    perror("Select error");
                    continue;
                    //filter error and exit if needed
                }
                for (int i = 0; i <= maxfd; i++) {
                    if (FD_ISSET(i, &fdbuff)) {
                        if (i == intSock()) {
                            if ((nsock = accept(i, (sockaddr*)&naddr, &naddrlen)) < 0) {
                                perror("Error accepting connection");
                                continue;
                            }
                            FD_SET(nsock, &master);
                            if (nsock > maxfd) {
                                maxfd = nsock;
                            }
                            getSockIp(nsock, ipbuff);
                            std::cout << pilib::dateStamp() << " : Got connection from [" << ipbuff << "]\n";
                        }
                        else {
                            if ((readlen = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
                                if (readlen == 0) {
                                    getSockIp(i, ipbuff);
                                    std::cout << pilib::dateStamp() << " : Connection ended [" << ipbuff << "]\n\n";
                                }
                                else {
                                    perror("Recv error");
                                }
                                close(i);
                                FD_CLR(i, &master);
                            }
                            else {
                                if (!rc) {
                                    send(i, NULL, 0, 0);
                                }
                                else {
                                    getSockIp(i, ipbuff);
                                    std::cout << pilib::dateStamp() << " : Got request from [" << ipbuff << "] {length: " << readlen << "}: ";
                                    this->handler.respond(i, std::string(buffer));
                                    std::cout << "to [" << ipbuff << "]\n";
                                    memset(&buffer, 0, sizeof(buffer)); //may need to include this above also
                                }
                            }

                        }
                    }
                    
                }
            }
            close(intSock());
        }
    }
}