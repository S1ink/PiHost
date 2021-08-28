#pragma once

#include "../STD.h"
#include "../IO.h"
#include "../Utility.h"

namespace pilib {
	class SigHandle {	
	public:
		typedef void(*__sigaction_t)(int, siginfo_t*, void*);
		enum class Sigs {
			ERR, HUP, INT, QUIT, ILL, TRAP, ABRT, BUS, FPE, KILL, USR1,
			SEGV, USR2, PIPE, ALRM, TERM, STKFLT, CHLD, CONT, STOP, STP,
			TTIN, TTOU, URG, XCPU, XFSZ, VTALRM, PROF, WINCH, IOA, PWR, SYSC
		};

		static SigHandle& get();

		static void printIgnore(int signum);
		static void printExit(int signum);
		static void printInfo(siginfo_t* data);

		static void sigIgnore(int signum);
		static void sigExit(int signum);
		static void advIgnore(int signum, siginfo_t* data, void* ext);
		static void advExit(int signum, siginfo_t* data, void* ext);

		void setLog(const std::_Ios_Openmode modes);
		void setLog(const char* file);
		void setLog(const char* file, const std::_Ios_Openmode modes);
		void setLog(std::ostream* stream);
		void setLog(olstream&& target);	//this function fakes being a member, it really should only be static
		void setLog(const olstream& target);
		void reset(std::initializer_list<Sigs> exit = def_ex, std::initializer_list<Sigs> ingore = def_ign);
		void setup(__sighandler_t exit, __sighandler_t ignore = printIgnore);
		void setup(__sighandler_t exit, std::initializer_list<Sigs> exl, std::initializer_list<Sigs> ignl, __sighandler_t ignore = printIgnore);
#ifdef __linux__
		void setup(__sigaction_t exit, __sigaction_t ignore = advIgnore);
		void setup(__sigaction_t exit, std::initializer_list<Sigs> exl, std::initializer_list<Sigs> ignl, __sigaction_t ignore = advIgnore);
		void setadv();
#endif
	private:
		static const std::array<const char*, 31> sigmap;
		static const std::initializer_list<Sigs> def_ign, def_ex;	//questionable preference?

		std::vector<Sigs> ign_sigs, exit_sigs;

		SigHandle();
		SigHandle(const SigHandle&) = delete;
		//SigHandle(SigHandle&& other);

		void sigSet(__sighandler_t exit, __sighandler_t ignore = printIgnore);
#ifdef __linux__
		void sigSet(__sigaction_t exit, __sigaction_t ignore = advIgnore);
		void sigSet(bool adv = 0);
#else
		void sigSet();
#endif
	};

	extern SigHandle& sig_handle;
}