#ifndef AUTOMTX_HPP
#define AUTOMTX_HPP
#include <iostream>
template<typename MutexT> class automtx
{
private:
    MutexT* _mtx;
public:
    automtx(MutexT* mutex) : _mtx(mutex)
    {
        _mtx->lock();
    }
    ~automtx()
    {
        _mtx->unlock();
    }
};
#ifdef DLIB_AUTO_MUTEX
#include "../lib/dlib/threads.h"
typedef automtx<dlib::mutex> automtxdlib;
#endif
#endif // AUTOMTX_HPP
