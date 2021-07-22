#include "Pilib/headers/include.h"
#include "PiLib/headers/pivar.h"

namespace sample {
    //special
    std::vector<std::string> split(const std::string& str, const std::string& delim = " ") noexcept {
        std::vector<std::string> tokens;
        std::string strCopy = str;
        std::size_t pos = 0;
        std::string token;

        while ((pos = strCopy.find(delim)) != std::string::npos) {
            token = strCopy.substr(0, pos);
            strCopy.erase(0, pos + delim.length());
            tokens.push_back(token);
        }

        if (strCopy.length() > 0) {
            tokens.push_back(strCopy);
        }

        return tokens;
    }

    std::vector<std::string> split(const std::string& str, const char delim = space) noexcept {
        std::istringstream stream(str);
        std::vector<std::string> result;
        std::string word;

        while (std::getline(stream, word, delim)) {
            result.push_back(word);
        }
        return result;
    }

    std::string concat(const std::vector<std::string>& strings, const std::string& delim = "") noexcept {
        std::string result;

        for (std::size_t i = 0; i < strings.size(); i++) {
            result += strings[i];

            if ((i + 1) != strings.size()) {
                result += delim;
            }
        }

        return result;
    }

    // request method
    enum class Method {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATCH
    };

    std::string to_string(Method method) {
        switch (method) {
        case Method::GET:
            return "GET";
        case Method::HEAD:
            return "HEAD";
        case Method::POST:
            return "POST";
        case Method::PUT:
            return "PUT";
        case Method::DELETE:
            return "DELETE";
        case Method::TRACE:
            return "TRACE";
        case Method::OPTIONS:
            return "OPTIONS";
        case Method::CONNECT:
            return "CONNECT";
        case Method::PATCH:
            return "PATCH";
        }
    }

    Method method_from_string(const std::string& method) noexcept {
        if (method == to_string(Method::GET)) {
            return Method::GET;
        }
        else if (method == to_string(Method::HEAD)) {
            return Method::HEAD;
        }
        else if (method == to_string(Method::POST)) {
            return Method::POST;
        }
        else if (method == to_string(Method::PUT)) {
            return Method::PUT;
        }
        else if (method == to_string(Method::DELETE)) {
            return Method::DELETE;
        }
        else if (method == to_string(Method::TRACE)) {
            return Method::TRACE;
        }
        else if (method == to_string(Method::OPTIONS)) {
            return Method::OPTIONS;
        }
        else if (method == to_string(Method::CONNECT)) {
            return Method::CONNECT;
        }
        else if (method == to_string(Method::PATCH)) {
            return Method::PATCH;
        }
    }

    //http version
    enum class Version {
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0
    };

    std::string to_string(Version version) {
        switch (version) {
        case Version::HTTP_1_0:
            return "HTTP/1.0";

        case Version::HTTP_1_1:
            return "HTTP/1.1";

        case Version::HTTP_2_0:
            return "HTTP/2.0";
        }
    }

    Version version_from_string(const std::string& version) noexcept {
        if (version == to_string(Version::HTTP_1_0)) {
            return Version::HTTP_1_0;
        }
        else if (version == to_string(Version::HTTP_1_1)) {
            return Version::HTTP_1_1;
        }
        else if (version == to_string(Version::HTTP_2_0)) {
            return Version::HTTP_2_0;
        }
    }

    //http header
    class Header {
    private:
        std::string key;
        std::string value;
    public:
        Header(const std::string& key, const std::string& value) noexcept : key(key), value(value) {}
        Header(const std::string&& key, const std::string& value) noexcept : key(std::move(key)), value(std::move(value)) {}

        void set_value(const std::string& value) noexcept {
            this->value = value;
        }

        const std::string& get_key() const noexcept {
            return this->key;
        }

        std::string serialize() const noexcept {
            std::string header;
            header += this->key;
            header += ": ";
            header += this->value;
            header += endline;

            return header;
        }

        static Header deserialize(const std::string& header) {
            std::vector<std::string> segments = split(header, " ");
            const std::string key = segments[0].substr(0, segments[0].size() - 1);
            segments.erase(segments.begin());
            const std::string value = concat(segments, " ");

            return Header(key, value);
        }

        static Header deserialize2(const std::string& header) {
            std::istringstream headstream(header);
            std::string key;
            std::string value;
            
            std::getline(headstream, key, ':');
            std::getline(headstream, value);

            value.erase(std::remove_if(std::begin(value), std::end(value), [](char c){return std::isspace(c);}));

            return Header(key, value);
        }
    };

    //http request
    class Request {
    private:
        Version version;
        Method method;
        std::string resource;
        std::map<std::string, Header> headers;
    public:
        Request(Method method, const std::string& resource, const std::map<std::string, Header>& headers, Version version = Version::HTTP_1_1) noexcept :
            version(version), method(method), resource(resource), headers(headers) {}

        std::string serialize() const noexcept {
            std::string request;
            request += to_string(this->method);
            request += space;
            request += this->resource;
            request += space;
            request += to_string(this->version);
            request += endline;

            for (const std::pair<const std::string, Header>& header : this->headers)
            {
                request += header.second.serialize();
            }

            request += endline;
            return request;
        }

        static Request deserialize(const std::string& request) {
            std::vector<std::string> lines = split(request, std::string(endline));

            if (lines.size() < 1) {
                throw std::runtime_error("HTTP Request ('" + std::string(request) + "') consisted of " + std::to_string(lines.size()) + " lines, should be >= 1.");
            }

            std::vector<std::string> segments = split(lines[0], " ");

            if (segments.size() != 3) {
                throw std::runtime_error("First line of HTTP request ('" + std::string(request) + "') consisted of " + std::to_string(segments.size()) + " space separated segments, should be 3.");
            }

            const Method method = method_from_string(segments[0]);
            const std::string resource = segments[1];
            const Version version = version_from_string(segments[2]);
            std::map<std::string, Header> headers;

            for (std::size_t i = 1; i < lines.size(); i++) {
                if (lines[i].size() > 0) {
                    const Header header = Header::deserialize(lines[i]);
                    headers.insert(std::make_pair(header.get_key(), header));
                }
            }

            return Request(method, resource, headers, version);
        }
    };

    //http response
    class Response {
    private:
        int responseCode;
        Version version;
        std::map<std::string, Header> headers;
        std::string body;
    public:
        constexpr static int OK = 200;
        constexpr static int CREATED = 201;
        constexpr static int ACCEPTED = 202;
        constexpr static int NO_CONTENT = 203;
        constexpr static int BAD_REQUEST = 400;
        constexpr static int FORBIDDEN = 403;
        constexpr static int NOT_FOUND = 404;
        constexpr static int REQUEST_TIMEOUT = 408;
        constexpr static int INTERNAL_SERVER_ERROR = 500;
        constexpr static int BAD_GATEWAY = 502;
        constexpr static int SERVICE_UNAVAILABLE = 503;

        Response(int responseCode, Version version, const std::map<std::string, Header>& headers, const std::string& body) noexcept :
            responseCode(responseCode), headers(headers), body(body) {}

        int get_response_code() const noexcept {
            return this->responseCode;
        }

        const std::string& get_body() const noexcept {
            return this->body;
        }

        const std::map<std::string, Header> get_headers() const noexcept {
            return this->headers;
        }

        static Response deserialize(const std::string& response) noexcept {
            std::vector<std::string> segments = split(response, std::string(endline) + std::string(endline));

            std::string headerSegment = segments[0];
            segments.erase(segments.begin());

            std::string body = concat(segments);

            std::vector<std::string> headerLines = split(headerSegment, std::string(endline));

            const std::string& responseCodeLine = headerLines[0];

            std::vector<std::string> responseCodeSegments = split(responseCodeLine, " ");

            Version version = version_from_string(responseCodeSegments[0]);
            int responseCode = std::stoi(responseCodeSegments[1]);

            headerLines.erase(headerLines.begin());

            std::map<std::string, Header> headers;

            for (const std::string& line : headerLines) {
                const Header header = Header::deserialize(line);
                headers.insert(std::make_pair(header.get_key(), header));
            }

            return Response(responseCode, version, headers, body);
        }
    };
}

//main
//int main(int argc, char* argv[]) {
//    if (argc != 4) {
//        std::cout << "USAGE: ./http.elf [HOST-NAME] [RESOURCE] [OUTPUT-FILE]" << std::endl;
//        return -1;
//    }
//
//    std::string host(argv[1]);
//    std::string resource(argv[2]);
//    std::string outputFile(argv[3]);
//
//    sample::Header hostHdr = sample::Header("Host", host);
//    sample::Header dntHdr = sample::Header("DNT", "1");
//
//    std::map<std::string, sample::Header> headers;
//    headers.insert(std::make_pair(hostHdr.get_key(), hostHdr));
//    headers.insert(std::make_pair(dntHdr.get_key(), dntHdr));
//
//    sample::Request request(sample::Method::GET, resource, headers);
//
//    std::string httpRequest = request.serialize();
//
//    int socketFileDesc = Sockets::C::Client::connect(std::string(host), 80);
//
//    int error = send(socketFileDesc, httpRequest.c_str(), httpRequest.size(), 0);
//
//    if (error == -1) {
//        throw std::runtime_error("Failed to send data to " + std::string(host) + ":80.");
//    }
//
//    char buffer[8192];
//
//    int numBytes = recv(socketFileDesc, buffer, 8192, 0);
//
//    if (numBytes == -1) {
//        throw std::runtime_error("Failed to receive data from " + std::string(host) + ":80.");
//    }
//
//    close(socketFileDesc);
//
//    sample::Response response = sample::Response::deserialize(std::string(buffer));
//
//    std::ofstream filestream(outputFile);
//
//    filestream << response.get_body();
//
//    filestream.close();
//
//    return 0;
//}