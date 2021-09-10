#define INCLUDE_STD
#define VARS
#include "pilib.h"

CE_STR logs = "/data/logs/webserver.txt";

pilib::StopWatch runtime("Runtime", pilib::olstream(logs, std::ios::app), 0);

void on_exit() {
	runtime.end();
}

class ClientID : public pilib::HttpServer::Formatter {
	friend class pilib::HttpServer;
	template<class format_t> friend class pilib::HttpServer2;
private:
	std::string lroot;
	std::ofstream writer;
	void setRoot() {
		std::string path(this->getStream()->getPath());
		this->lroot = path.substr(0, path.find_last_of(s_dir));
		this->lroot.append("/clients/");
	}
	std::string cPath(const char* ip) {
		std::string ret = this->lroot;
		ret.append(ip);
		std::replace(ret.begin(), ret.end(), '.', '_');
		ret.append(".csv");
		return ret;
	}
	std::string iPath(const char* ip) {
		std::string ret = this->lroot;
		ret.append(ip);
		std::replace(ret.begin(), ret.end(), '.', '_');
		ret.append(".info");
		return ret;
	}
protected:
	ClientID() : Formatter() {}
	void update(pilib::HttpServer* outer) override {
		setRoot();
	}
public:
	ClientID(pilib::HttpServer* outer) : Formatter(outer) {
		setRoot();
	}

	void onRequest(int fd, const char* ip, int readlen, const pilib::Request* req = nullptr, const char* resource = nullptr) override {
		if (!pilib::exists(iPath(ip))) {
			this->writer.open(iPath(ip));
			this->writer << (req->getHeaders().find("User-Agent")) << newline;
			this->writer.close();
		}

		this->writer.open(cPath(ip), std::ios::app);
		this->writer << pilib::dateStamp() << comma << readlen << comma << resource << comma;
	}
	void onResponse(int fd, const char* ip, int sent, const pilib::Response* resp = nullptr, const char* resource = nullptr) override {
		this->writer << sent << newline;
		this->writer.close();
	}
};

std::string resourceMap(std::string&& root, const std::string& item) {
	std::string path = std::move(root);
	if ((item == "/") || (item == "/home")) {
		path.append("/index.html");
	}
	else if (!item.compare(0, 6, "/logs/")) {
		path = "/data" + item;
	}
	else if (!item.compare(0, 9, "/PiSHARE/")) {
		path = item;
	}
	else if (!item.compare(0, 6, "/data/")) {
		path = "/data/pihost" + item.substr(5);
	}
	else {
		path.append(item);
	}
	return path;
}

//add cli args for root and finder?
int main(int argc, char* argv[]) {
	runtime.setStart();
	pilib::progdir.setDir(argv[0]);
	pilib::sig_handle.setLog(logs);

	atexit(on_exit);

	pilib::HttpServer2<ClientID> server(logs, "/data/pihost/resources", resourceMap);
	server.serve();

	return 0;
}