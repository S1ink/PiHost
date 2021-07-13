#include "pilib.h"

std::atomic_bool run = { true };

static void endCondition() {
	std::cin.ignore();
	run = false;
}

void work(std::ostream& output) {
	output << info::dateStamp() << ": work done?" << newline;
}

struct netdevice {
	std::string id;
	int basic_data[2];
	int full_data[16];
};

class netstat {
	void parse(std::vector<netdevice>& devices) {
		//std::vector<netdevice> devices;
		std::ifstream file(locations::stats::network);
		std::string buffer;
		while (std::getline(file, buffer)) {
			std::istringstream line(buffer);
			line >> buffer;
			if (buffer != "Inter-|" || buffer != "face") {
				devices.emplace_back(netdevice());
				netdevice& device = devices.back();
				device.id = buffer;
				for (int i = 0; i < 16; i++) {
					line >> device.full_data[i];
				}
				device.basic_data[0] = device.full_data[0];
				device.basic_data[1] = device.full_data[8];
			}
		}
	}
public:
	struct transfer {
		long long up;
		long long down;
	};

	static void ethernet(transfer& data) {
		//transfer data;
		std::string buffer;
		int lindex = 0;
		timing::StopWatch timer;
		std::ifstream input(locations::stats::network);
		while (1) {
			std::getline(input, buffer);
			std::istringstream line(buffer);
			line >> buffer;
			if (buffer == "eth0:") {
				long long raw[9];
				for (int i = 0; i < 9; i++) {
					line >> raw[i];
				}
				data.down = raw[0];
				data.up = raw[8];
				input.close();
				std::this_thread::sleep_for(CHRONO::seconds(1));
				double div = timer.getDuration();
				input.open(locations::stats::network);
				for (int i = 0; i <= lindex; i++) {
					std::getline(input, buffer);
				}
				std::istringstream line(buffer);
				line >> buffer;
				for (int i = 0; i < 9; i++) {
					line >> raw[i];
				}
				data.down = (raw[0] - data.down)/div;
				data.up = (raw[8] - data.up)/div;
				break;
			}
			else {
				lindex += 1;
			}
		}
		//return data;
	}
};

std::string bytePrefixes(long long& bytes) {
	std::ostringstream stream;
	if (bytes > 1000000000) {
		stream << bytes / 1000000000 << " GB";
	}
	else if (bytes > 1000000) {
		stream << bytes / 1000000 << " MB";
	}
	else if (bytes > 1000) {
		stream << bytes / 1000 << " KB";
	}
	else {
		stream << bytes << " B";
	}
	return stream.str();
}

int main(int argc, char* argv[]) {
	timing::StopWatch runtime;
	
	/*std::thread looping(threading::loopingThread<CHRONO::seconds::rep, CHRONO::seconds::period, void, std::ostream const&, threading::templatefunc>, std::ref(run), CHRONO::seconds(5), CHRONO::seconds(10), threading::streamWrapper, std::ofstream("/data/logs/work.txt", std::ios::app), work);
	endCondition();
	looping.join();*/

	std::thread end(endCondition);

	/*netstat::transfer nets;
	while (run) {
		netstat::ethernet(nets);
		std::cout << bytePrefixes(nets.down) << "/s down" << newline << bytePrefixes(nets.up) << "/s up" << newline << newline;
	}*/

	networking::Server webserver;
	webserver.prep();
	webserver.launch(std::ref(run), std::cout, networking::httpResponse);

	end.join();

	runtime.pTotal();
	return 0;
}

//NOTES
/* CPU States
user – time spent in user mode.
nice – time spent in user mode with low priority.
system – time spent in system mode.
idle – time spent in the idle task.
iowait –  time waiting for I/O to complete.
irq – time servicing hardware interrupts.
softirq – time servicing software interrupts.
steal – time spent in other operating systems when running in a virtualized environment.
guest – time spent running a virtual CPU for guest operating systems.
guest_nice – time spent running a low priority virtual CPU for guest operating systems.
* * * *
idle + iowait = time doing nothing (usr_hz)
all others = time spend utilized
*/