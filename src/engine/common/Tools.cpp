#include <sys/time.h>
#include <chrono>
#include <thread>

#include "Tools.hpp"

uint64 astro::ticks(){
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64 astro::epoch(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

#ifdef _WIN32
    #include <windows.h>
	static inline void __usleep(__int64 usec){
	    HANDLE timer;
	    LARGE_INTEGER ft;

	    ft.QuadPart = -(10*(usec*1000));
	    timer = CreateWaitableTimer(NULL, TRUE, NULL);
	    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	    WaitForSingleObject(timer, INFINITE);
	    CloseHandle(timer);
	}
#else
	static inline void __usleep(int msec){
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}
#endif

void astro::sleep(uint64 t){
    __usleep(t);
}