#include "Tools.hpp"
#include <sys/time.h>

uint64 astro::ticks(){
    static timeval t;
    gettimeofday(&t, 0);
    return (int64)(t.tv_sec * 1000l) + (int64)(t.tv_usec / 1000l);
}

uint64 astro::epoch(){
    static timeval t;
    gettimeofday(&t, 0);
    return t.tv_sec;
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
	#include <chrono>
	#include <thread>
	static inline void __usleep(int msec){
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}
#endif

void astro::sleep(uint64 t){
    __usleep(t);
}