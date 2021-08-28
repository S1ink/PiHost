#define INCLUDE_STD
#define VARS
#include "pilib.h"

CE_STR logs = "/data/logs/webserver.txt";

pilib::StopWatch runtime(false, false);

void on_exit() {
	runtime.pTotal(pilib::olstream(logs, std::ios::app));
}

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

int main(int argc, char* argv[]) {
	runtime.setStart();
	pilib::progdir.setDir(argv[0]);
	pilib::sig_handle.setLog(logs);

	atexit(on_exit);

	pilib::HttpServer server(logs, "/data/pihost/resources", resourceMap);
	server.serve();

	return 0;
}