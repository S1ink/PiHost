#include "server.h"

#ifdef _WIN32
#pragma push_macro("DELETE")
#pragma push_macro("ERROR")

#undef DELETE
#undef ERROR
#endif


void HttpServer::prepServer() {
    getSock();
    bindSock();
    startListen();
}
bool HttpServer::defLog() {
    return (&(this->logs.getStream()) == &std::cout);
}
void HttpServer::lerror(const char* message) {
    if (this->defLog()) {
        perror(message);
    }
    else {
        ((((this->logs <<= dateStamp()) <= " : Error - ") <= message) < newline);
    }
}
void HttpServer::ex_lerror(const char* message) {
    if (this->defLog()) {
        perror(message);
    }
    else {
        ((((this->logs <<= dateStamp()) <= " : Error - ") <= message) < newline);
    }
    exit(errno);
}



olstream* HttpServer::HttpFormatter::getStream() {
    return &(this->that->logs);
}
Version* HttpServer::HttpFormatter::getVer() {
    return &(this->that->version);
}

void HttpServer::HttpFormatter::update(HttpServer* outer) {
    this->that = outer;
}

void HttpServer::HttpFormatter::onServerStart() {
    ((((this->that->logs <<= dateStamp()) <= " : Webserver resources initialized - beginning to serve [") <= Versions::getString(this->that->version)) < "]\n\n");
}
void HttpServer::HttpFormatter::onConnect(int fd, const char* ip) {
    ((((this->that->logs <<= dateStamp()) <= " : Got connection from [") <= ip) < "]\n");
}
void HttpServer::HttpFormatter::onRequest(int fd, const char* ip, int readlen, const Request* req, const char* resource) {
    ((((((this->that->logs <<= dateStamp()) <= " : Got request from [") <= ip) <= "] - length: ") <= readlen) < newline);
}
void HttpServer::HttpFormatter::onResponse(int fd, const char* ip, int sent, const Response* resp, const char* resource) {
    ((((((((this->that->logs <<= dateStamp()) <= " : Sent {") <= resource) <= "} to [") <= ip) <= "] - length: ") <= sent) < newline);
}
void HttpServer::HttpFormatter::onDisconnect(int fd, const char* ip) {
    ((((this->that->logs <<= dateStamp()) <= " : Ended connection with [") <= ip) < "]\n\n");
}
void HttpServer::HttpFormatter::onServerEnd() {
    this->that->logs << withTime("Webserver stopped");
}



std::string HttpServer::HttpHandler::resourceMapper(std::string&& root, const std::string& item) {
    std::string path = std::move(root);
    if (item == "/") {
        path.append("/index.html");
    }
    //check for special specifier paths here
    else {
        path.append(item);
    }
    return path;
}
// std::string HttpServer::HttpHandler::resourceSupplier(const std::string& path) {
//     std::ifstream reader(path.c_str(), std::ios::binary);
//     return std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
// }
const char* HttpServer::HttpHandler::safeMime(const char* path) {
    const char* mime = getMegaMimeType(path);
    if (mime) {
        return mime;
    }
    return "text/plain";
}

void HttpServer::HttpHandler::respond(const int socket, const char* ip, const int readlen, const std::string& input, HttpFormatter* formatter) {
    this->request_buff.parse(input);
    HeaderList headers;
    std::ifstream reader;
    std::string body, path = std::move(this->resourceMapper(that->root, this->request_buff.getResource()));

    formatter->onRequest(socket, ip, readlen, &this->request_buff, path.c_str());   //change Request so that we can pass a const in here

    (this->response.intHeaders())->reset();

    headers.add(   //headers that apply to all
        Segment("Server", "Custom C++ HTTP Server (Raspberry Pi)")
    );

    switch (this->request_buff.getMethod()) {
    case Method::GET:   //requesting resource
    {
        reader.open(path.c_str(), std::ios::binary);    //attempt to open resource
        if (reader.is_open()) { //if resource exists
            body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
            headers.add({
                {"Content-Type", safeMime(path.c_str())},
                {"Content-Length", std::to_string(body.length())}
            });
            if (that->version == Version::HTTP_1_0) {   //close connection if http 1.0
                headers.add("Connection", "close");
            }
            else {
                headers.add("Connection", "keep-alive");
            }
            this->response.update(Code::OK, headers, body);
        }
        else {  //send 404
            reader.open(this->resourceMapper(that->root, "/error.html"));
            if (reader.is_open()) { //check if error page exists
                body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                replace(body, "{{code}}", Codes::getString(Code::NOT_FOUND).c_str());
                headers.add(
                    Segment("Content-Type", "text/html")
                );
            }
            else {  //if not send plain text
                body = "{Error page not found} - Error: 404 Not Found";
                headers.add(
                    Segment("Content-Type", "text/plain")
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
                {"Content-Type", safeMime(path.c_str())},
                {"Content-Length", std::to_string(body.length())}
                });
            this->response.update(Code::OK, headers);
        }
        else {  //else send 404
            reader.open(this->resourceMapper(that->root, "/error.html"));
            if (reader.is_open()) { //check error page exists
                body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
                replace(body, "{{code}}", Codes::getString(Code::NOT_FOUND).c_str());
                headers.add(
                    Segment("Content-Type", "text/html")
                );
            }
            else {  //if not send text
                body = "{Error page not found} - Error: 404 Not Found";
                headers.add(
                    Segment("Content-Type", "text/plain")
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
    case Method::POST:  // POST is used when the exact url is not known, or a general action is being commanded
    case Method::PUT:   // PUT is used when the exact url is known, and can modify prexisting objects
    case Method::TRACE: //methods not supported
    {
        reader.open(this->resourceMapper(that->root, "/error.html"));
        if (reader.is_open()) { //check if page exists
            body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
            replace(body, "{{code}}", Codes::getString(Code::METHOD_NOT_ALLOWED).c_str());
            headers.add(
                Segment("Content-Type", "text/html")
            );
        }
        else {  //else send plain text
            body = "{Error page not found} - Error: 405 Method Not Allowed";
            headers.add(
                Segment("Content-Type", "text/plain")
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
        reader.open(this->resourceMapper(that->root, "/error.html"));
        if (reader.is_open()) { //check if page exists
            body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
            replace(body, "{{code}}", Codes::getString(Code::BAD_REQUEST).c_str());
            headers.add(
                Segment("Content-Type", "text/html")
            );
        }
        else {  //else send plain text
            body = "{Error page not found} - Error: 400 Bad Request";
            headers.add(
                Segment("Content-Type", "text/plain")
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
        reader.open(this->resourceMapper(that->root, "/error.html"));
        if (reader.is_open()) { //check if page exists
            body = std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>()));
            replace(body, "{{code}}", Codes::getString(Code::NOT_IMPLEMENTED).c_str());
            headers.add(
                Segment("Content-Type", "text/html")
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
    formatter->onResponse(socket, ip, send(socket, ret.c_str(), ret.length(), 0), &(this->response), path.c_str());
    return;
}



HttpServer::HttpServer(
    const olstream& logger,
    const char* root,
    std::atomic<bool>* control,
    Version version,
    const char* port,
    int max_clients
) : BaseServer(NULL, port, max_clients), root(root), version(version) {
    if (control != nullptr) {
        this->online = control;
    }
    else {
        this->online = &(this->l_online);
        this->l_online = false;
    }
    this->logs = logger;
    this->logs.setModes(std::ios::app);
    getServerAddr();
}
HttpServer::HttpServer(
    olstream&& logger,
    const char* root,
    std::atomic<bool>* control,
    Version version,
    const char* port,
    int max_clients
) : BaseServer(NULL, port, max_clients), root(root), version(version) {
    if (control != nullptr) {
        this->online = control;
    }
    else {
        this->online = &(this->l_online);
        this->l_online = false;
    }
    this->logs = std::move(logger);
    this->logs.setModes(std::ios::app);
    getServerAddr();
}

void HttpServer::setLog(const std::ios::openmode modes) {
    this->logs.setModes(modes);
}
void HttpServer::setLog(const char* file) {
    this->logs.setStream(file);
}
void HttpServer::setLog(const char* file, const std::ios::openmode modes) {
    this->logs.setStream(file, modes);
}
void HttpServer::setLog(std::ostream* stream) {
    this->logs.setStream(stream);
}
void HttpServer::setLog(const olstream& target) {
    this->logs = target;
}
void HttpServer::setLog(olstream&& target) {
    this->logs = std::move(target);
}

void HttpServer::stop() {   //will not change external vars, but still alerts running serve functions to stop
    this->l_online = false;
    this->online = &l_online;
}
void HttpServer::estop() {
    CLOSE_SOCK(this->sockmain);
    this->logs << withTime("Server socket closed out of loop (emergency stop)\n");
}


#ifdef _WIN32
#pragma pop_macro("ERROR")
#pragma pop_macro("DELETE")
#endif