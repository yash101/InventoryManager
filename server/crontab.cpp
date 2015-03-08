#include "crontab.hpp"
#include <time.h>
void Crontab::thread()
{
    size_t ttl_sec = 0;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ttl_sec += 1;
        _mtx_fptrs.lock();
        for(std::map<std::string, Cron>::const_iterator it = _fptrs.begin(); it != _fptrs.end(); ++it)
        {
            if(_fptrs[(*it).first].interval() % ttl_sec == 0) _fptrs[(*it).first]();
        }
        _mtx_fptrs.unlock();
    }
}

Crontab::Crontab() : stop(false)
{
    cthd = std::thread(&Crontab::thread, this);
}
