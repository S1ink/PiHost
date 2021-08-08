#include  "headers/info.h"

namespace pilib {
	const std::array<CPU::CoreData::States, 2> CPU::CoreData::s_idle = {
		States::IDLE, States::IOWAIT
	};
	const std::array<CPU::CoreData::States, 8> CPU::CoreData::s_active = {
		States::USER, States::NICE, States::SYSTEM,
		States::IRQ, States::SOFTIRQ, States::STEAL,
		States::GUEST, States::GUEST_NICE
	};

	void CPU::CoreData::update() {
		std::ifstream reader(locations::stats::cpu);
		reader >> this->title;
		for (uint i = 0; i < (uint)States::TOTAL; i++) {
			reader >> this->data[i];
		}
		reader.close();
	}
	CPU::CoreData::CoreData(void*) {
		update();
	}
	CPU::CoreData::CoreData(const CoreData& other) {
		std::cout << "caught you" << newline;
	}
	uint CPU::CoreData::getIdle() {
		uint ret = 0;
		for (uint i = 0; i < s_idle.size(); i++) {
			ret += this->data[(uint)s_idle[i]];
		}
		return ret;
	}
	uint CPU::CoreData::getActive() { //switch to list of add's?
		uint ret = 0;
		for (uint i = 0; i < s_active.size(); i++) {
			ret += this->data[(uint)s_active[i]];
		}
		return ret;
	}
	uint CPU::CoreData::getTotal() {
		uint ret = 0;
		for (uint i = 0; i < (uint)States::TOTAL; i++) {
			ret += this->data[i];
		}
		return ret;
	}
	uint CPU::CoreData::getState(States state) {
		return this->data[(uint)state];
	}

	void CPU::CoreData::readAll(std::vector<CoreData>& lines) {
		std::ifstream reader(locations::stats::cpu);
		std::string buffer;
		while (std::getline(reader, buffer)) {
			if (!buffer.compare(0, 3, "cpu")) {
				std::istringstream line(buffer);
				lines.emplace_back();
				CoreData& core = lines.back();
				line >> core.title;
				for (uint i = 0; i < (uint)States::TOTAL; i++) {
					line >> core.data[i];
				}
			}
			else {
				break;
			}
		}
		reader.close();
	}

	CPU::CPU() {
		std::ifstream reader(locations::stats::cpu);
		std::string buffer;
		while (std::getline(reader, buffer)) {
			if ((!buffer.compare(0, 3, "cpu")) && isdigit(buffer[3])) {
				this->c_cores += 1;
			}
			/*else {
				break;
			}*/
		}
		reader.close();
		cbuff1 = new CoreData[this->c_cores];
		cbuff2 = new CoreData[this->c_cores];
		std::cout << "pointers inited" << newline;
	}
	CPU::~CPU() {
		if (cbuff1 != nullptr) {
			delete[](cbuff1);
		}
		if (cbuff2 != nullptr) {
			delete[](cbuff2);
		}
	}

	float CPU::average(CoreData& first, CoreData& second) {
		float active = (float)(second.getActive() - first.getActive());
		return active / (active + ((float)(second.getIdle() - first.getIdle())));
	}
	void CPU::averageVec(std::vector<CoreData>& first, std::vector<CoreData>& second, UtilMap& out) {
		float active, total;
		for (uint i = 0; i < first.size(); i++) {
			active = (float)(second[i].getActive() - first[i].getActive());
			total = (float)(second[i].getTotal() - first[i].getTotal());
			out.emplace(std::make_pair(first[i].title, (100.f*active/total)));
		}
	}
	CPU::UtilMap CPU::averageVec(std::vector<CoreData>& first, std::vector<CoreData>& second) {
		UtilMap ret;
		float active, total;
		for (uint i = 0; i < first.size(); i++) {
			active = (float)(second[i].getActive() - first[i].getActive());
			total = (float)(second[i].getTotal() - first[i].getTotal());
			ret.emplace(std::make_pair(first[i].title, (100.f*active/total)));
		}
		return ret;
	}

	float CPU::temp() {
		float systemp;
		FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
		fscanf(thermal, "%f", &systemp);
		fclose(thermal);
		return (systemp/1000.f);
	}

	float CPU::percent(int seconds) {
		CoreData second, first(nullptr);
		std::this_thread::sleep_for(CHRONO::seconds(seconds));
		second.update();
		return average(first, second);
	}

	CPU::UtilMap CPU::percentMap(int seconds) {
		std::vector<CoreData> first, second;
		CoreData::readAll(first);
		std::this_thread::sleep_for(CHRONO::seconds(seconds));
		CoreData::readAll(second);
		return averageVec(first, second);
	}

	float CPU::searchUtil(const std::string& search, UtilMap& map) {
		auto res = map.find(search);
		if (res != map.end()) {
			return res->second;
		}
		return 0.f;
	}

//old
//********************************************************************************************

	namespace sys {
		namespace cpu {
			size_t getIdle(const LineParse& data) {
				return data.time[3] + data.time[4];
			}

			size_t getActive(const LineParse& data) {
				return data.time[0] + data.time[1] + data.time[2] + data.time[5] + data.time[6] + data.time[7] + data.time[8] + data.time[9];
			}

			void readMain(LineParse& container) {
				std::ifstream reader(locations::stats::cpu);
				reader >> container.title;
				for (int i = 0; i < parsable_states; i++) {
					reader >> container.time[i];
				}
				reader.close();
			}

			void readVector(std::vector<LineParse>& container) {
				std::ifstream reader(locations::stats::cpu);
				std::string linebuffer;
				while (std::getline(reader, linebuffer)) {
					if (!linebuffer.compare(0, 3, "cpu")) {
						std::istringstream stream(linebuffer);
						container.emplace_back(LineParse());
						LineParse& line = container.back();
						stream >> line.title;
						for (int i = 0; i < parsable_states; i++) {
							stream >> line.time[i];
						}
					}
				}
				reader.close();
			}

			void convertData(LineParse& snapshot1, LineParse& snapshot2, ActivityData& output) {
				float active = static_cast<float>(getActive(snapshot2) - getActive(snapshot1));
				float idle = static_cast<float>(getIdle(snapshot2) - getIdle(snapshot1));
				float total = active + idle;
				output.title = snapshot1.title;
				output.activity = 100.f * (active / total);
			}

			void convertVectorData(std::vector<LineParse>& snapshot1, std::vector<LineParse>& snapshot2, std::vector<ActivityData>& output) {
				float active, idle, total;
				for (unsigned int i = 0; i < snapshot1.size(); i++) {
					output.emplace_back(ActivityData());
					ActivityData& entry = output.back();
					active = static_cast<float>(getActive(snapshot2[i]) - getActive(snapshot1[i]));
					idle = static_cast<float>(getIdle(snapshot2[i]) - getIdle(snapshot1[i]));
					total = active + idle;
					entry.title = snapshot1[i].title;
					entry.activity = 100.f * (active / total);
				}
			}
		}

		float cpuTemp() {
			float systemp;
			FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

			fscanf(thermal, "%f", &systemp);
			fclose(thermal);
			systemp /= 1000;

			return systemp;
		}

		float gpuTemp() {
			char var[12];
			FILE* pipe = popen("vcgencmd measure_temp", "r");

			fscanf(pipe, "%s", var);
			float temp = (((float)var[5] - 48) * 10 + ((float)var[6] - 48) + ((float)var[8] - 48) / 10);
			pclose(pipe);

			return temp;
		}

		float cpuPercent(CHRONO::duration<CHRONO::seconds::rep, CHRONO::seconds::period> interval) {
			cpu::LineParse time1, time2;
			readMain(time1);
			std::this_thread::sleep_for(interval);
			readMain(time2);
			float active = static_cast<float>(getActive(time2) - getActive(time1));
			float total = active + (static_cast<float>(getIdle(time2) - getIdle(time1)));
			return 100.f * (active / total);
		}
	}

	char* dateStamp() {
		time_t now = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
		char* t = ctime(&now);
		t[strlen(t) - 1] = '\0';
		return t;
	}

	char* dateStamp(time_t* tme) {
		char* t = ctime(tme);
		t[strlen(t) - 1] = '\0';
		return t;
	}
}

//struct netdevice {
//	std::string id;
//	int basic_data[2];
//	int full_data[16];
//};
//
//class netstat {
//	void parse(std::vector<netdevice>& devices) {
//		//std::vector<netdevice> devices;
//		std::ifstream file(locations::stats::network);
//		std::string buffer;
//		while (std::getline(file, buffer)) {
//			std::istringstream line(buffer);
//			line >> buffer;
//			if (buffer != "Inter-|" || buffer != "face") {
//				devices.emplace_back(netdevice());
//				netdevice& device = devices.back();
//				device.id = buffer;
//				for (int i = 0; i < 16; i++) {
//					line >> device.full_data[i];
//				}
//				device.basic_data[0] = device.full_data[0];
//				device.basic_data[1] = device.full_data[8];
//			}
//		}
//	}
//public:
//	struct transfer {
//		long long up;
//		long long down;
//	};
//
//	static void ethernet(transfer& data) {
//		//transfer data;
//		std::string buffer;
//		int lindex = 0;
//		pilib::StopWatch timer;
//		std::ifstream input(locations::stats::network);
//		while (1) {
//			std::getline(input, buffer);
//			std::istringstream line(buffer);
//			line >> buffer;
//			if (buffer == "eth0:") {
//				long long raw[9];
//				for (int i = 0; i < 9; i++) {
//					line >> raw[i];
//				}
//				data.down = raw[0];
//				data.up = raw[8];
//				input.close();
//				std::this_thread::sleep_for(CHRONO::seconds(1));
//				double div = timer.getDuration();
//				input.open(locations::stats::network);
//				for (int i = 0; i <= lindex; i++) {
//					std::getline(input, buffer);
//				}
//				std::istringstream line(buffer);
//				line >> buffer;
//				for (int i = 0; i < 9; i++) {
//					line >> raw[i];
//				}
//				data.down = (raw[0] - data.down) / div;
//				data.up = (raw[8] - data.up) / div;
//				break;
//			}
//			else {
//				lindex += 1;
//			}
//		}
//		//return data;
//	}
//};
//
//std::string bytePrefixes(long long& bytes) {
//	std::ostringstream stream;
//	if (bytes > 1000000000) {
//		stream << bytes / 1000000000 << " GB";
//	}
//	else if (bytes > 1000000) {
//		stream << bytes / 1000000 << " MB";
//	}
//	else if (bytes > 1000) {
//		stream << bytes / 1000 << " KB";
//	}
//	else {
//		stream << bytes << " B";
//	}
//	return stream.str();
//}