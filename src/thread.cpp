#include "thread.h"

#include <string.h>

#ifdef NDEBUG
#include <iostream>
#endif

ipclib::Result ipclib::Thread::run(void* theFunction(void*), void* theParameter) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: starting a new thread...";
    #endif

    int err;
    if( (err = pthread_create(&thread_id, NULL, theFunction, theParameter)) != 0 ) {
        #ifdef NDEBUG
        std::cout << "FAILED\n";
        #endif
        return Result(err, strerror(err));
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }
}

ipclib::Result ipclib::Thread::join(void** theReturnValue) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: joining on thread " << thread_id << "...";
    #endif

    int err;
    if( (err = pthread_join(thread_id, theReturnValue)) != 0 ) {
        #ifdef NDEBUG
        std::cout << "FAILED\n";
        #endif
        return Result(err, strerror(err));
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }
}

ipclib::Result ipclib::Thread::cancel() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: Sending a cancel request to " << thread_id << "...";
    #endif

    int err;
    if( (err = pthread_cancel(thread_id)) != 0 ) {
        #ifdef NDEBUG
        std::cout << "FAILED\n";
        #endif
        return Result(err, strerror(err));
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }
}
