#define INCLUDE_ALL
#define PILIB_FULL
#include "pilib.h"

std::atomic_bool run = { true };
pilib::StopWatch runtime(false);
uint fails = 0;
pilib::lstream logger;
std::string totest = "webservertest";

void sigIgnore(int signum) {
	std::ostringstream siglog;
	siglog << "{MANAGER} ";
	pilib::SigHandle::ignoreBase(signum, siglog);
	logger.openOutput();
	logger << siglog.str();
	logger.close();
}

void sigTerminate(int signum) {
	std::ostringstream siglog;
	siglog << "{MANAGER} ";
	pilib::SigHandle::terminateBase(signum, siglog);
	logger.openOutput();
	logger << siglog.str();
	logger.close();
	if (signum == 11) {	//check for signals that "spam"
		exit(signum);
	}
	else {
		run = false;
	}
}

//void endProg(const std::atomic_bool& rc, const char* process, uint wait = 10) {
//	std::atomic_bool& con = const_cast<std::atomic_bool&>(rc);
//	std::string kill("sudo killall -2 ");
//	kill.append(process);
//	while (con) {
//		std::this_thread::sleep_for(CHRONO::seconds(wait));
//	}
//	system(kill.c_str());
//}

void on_exit() {
	logger.openOutput();
	logger << "\n********************************************\n";
	if (run) {
		std::string kill("sudo killall -2 ");
		kill.append(totest);
		logger << "Killing webserver: '" << kill << "'\n";
		system(kill.c_str());
	}
	logger << "Total Fails: " << fails << newline;
	logger << "Runtime: " << runtime.getDuration() << " seconds\n";
	logger.close();
}

int of_exec(const char* command, pilib::lstream& output) {
	FILE* pipe = popen(command, "r");
	if (!pipe) {
		output.openOutput();
		output << "popen function failed";
		output.close();
	}
	else {
		char buffer[128];
		while (fgets(buffer, 128, pipe) != NULL) {
			output.openOutput();
			output << buffer;
			output.close();
		}
	}
	return pclose(pipe);
}

int main(int argc, char* argv[], char* envp[]) {
	runtime.setStart();
	pilib::proot.setDir(argv[0]);

	logger.setFile("/data/test/new_webservertest.txt", std::ios::app);	//get relative path working
	atexit(on_exit);
	pilib::SigHandle::get().setup(sigIgnore, sigTerminate);	//fix this

	if (argc > 1) {
		std::istringstream arg(argv[1]);
		std::string buffer;
		std::getline(arg, buffer, '=');
		if (buffer == "pname") {
			arg >> totest;
		}
	}
	std::ostringstream command;
	command << "sudo " << pilib::proot.getDirSlash() << totest;

	std::thread checker(endProg, std::ref(run), totest.c_str(), 10);

	logger.openOutput();
	logger << pilib::dateStamp() << " : Management service started!\n\n";
	logger.close();

	while (run) {
		logger.openOutput();
		logger << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
		logger.close();
		int ret = of_exec(command.str().c_str(), logger);
		if ((ret != 0) || (ret != 2)) {
			fails += 1;
		}
		logger.openOutput();
		logger << "\nCurrent Fails: " << fails << newline;
		logger << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
		logger.close();
	}
	checker.join();
	return 0;
}