#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <pthread.h>

#include "Log.hpp"
#include "../Core.hpp"

static pthread_mutex_t lock;

static inline std::string getTimeStamp(){
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);
    return std::to_string(localTime->tm_hour)+":"+std::to_string(localTime->tm_min)+":"+std::to_string(localTime->tm_sec);
}

bool astro::log(const std::string &_format, ...){
    pthread_mutex_lock(&lock);
    std::string format = "["+getTimeStamp()+"] "+_format;
    va_list arg;
    bool done;
    va_start (arg, _format);
    done = vfprintf(stdout, format.c_str(), arg);
    va_end (arg);
    pthread_mutex_unlock(&lock); 
    return done;
}

void __ASTRO_init_log(){
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("fatal error: couldn't initialize __ASTRO_init_log's mutex-lock\n"); 
        astro::Core::exit(1);
    }
}

void __ASTRO_end_log(){
    pthread_mutex_destroy(&lock); 
}
