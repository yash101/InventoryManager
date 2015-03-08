#ifndef CRONTAB_HPP
#define CRONTAB_HPP
#include "../lib/dlib/threads.h"
#include <map>
#include <thread>
#include <mutex>
//Holds a single cron entry
void doNothing() {}
class Cron
{
private:
    void (*_fptr)();
    size_t _seconds;
public:
    Cron(void (*fptr)(), size_t seconds) : _fptr(fptr), _seconds(seconds) {}
    Cron() : _fptr(doNothing), _seconds(1) {}
    size_t& interval() { return _seconds; }
    void setCallback(void (*fptr)()) { _fptr = fptr; }

    void operator()() { _fptr(); }
};

//This is the actual handler
class Crontab
{
private:
    void thread();
    std::mutex _mtx_fptrs;
    std::map<std::string, Cron> _fptrs;
    bool stop;
    std::thread cthd;
public:
    Crontab();
};
#endif // CRONTAB_HPP
