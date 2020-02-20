#include <stdio.h>
#include "Log.hpp"

// TODO: add some threading to this
bool astro::log(const std::string &format, ...){
    va_list arg;
    bool done;

    va_start (arg, format);
    done = vfprintf (stdout, format.c_str(), arg);
    va_end (arg);

    return done;
}