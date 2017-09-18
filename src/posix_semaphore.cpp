#include "posix_semaphore.h"

#include <string.h>
#include <fcntl.h>

#ifdef NDEBUG
#include <iostream>
#endif

ipclib::Semaphore::Semaphore(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const unsigned int theValue) : PosixObject() {
    create(theName, toCreate, toCreateExclusively, theValue);
}

int ipclib::Semaphore::getValue() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: retrieving value for semaphore " << name << "...";
    #endif

    int value;
    if( sem_getvalue(sem, &value) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        if( value <= 0 ) return 0;
        else return value;
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return -1;
    }
}

ipclib::Result ipclib::Semaphore::create(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const unsigned int theValue) {
    if( already_initialized ) deallocateResources();

    PosixObject::create(theName);

    #ifdef NDEBUG
    std::cout << "IPCLIB: starting initialization for " << name << "...";
    #endif

    int oflag = 0;
    if( toCreate ) oflag = oflag | O_CREAT;
    if( toCreateExclusively ) oflag = oflag | O_EXCL;

    if( (sem = sem_open(posix_name.c_str(), oflag, DEFAULT_PERMISSION, theValue)) == SEM_FAILED ) {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        initialization_result = res;
        return initialization_result;
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif

        initialization_result = Result(Result::SUCCESS);
        return initialization_result;
    }
}

ipclib::Result ipclib::Semaphore::destroy() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: destroying semaphore " << name << "...";
    #endif

    if( sem_unlink(posix_name.c_str()) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }
}

ipclib::Result ipclib::Semaphore::wait() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: waiting on " << name << "...";
    #endif

    if( sem_wait(sem) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }
}

ipclib::Result ipclib::Semaphore::wait(const long theSeconds, const long theNanoSeconds) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: waiting with a timeout on " << name << "...";
    #endif

    timespec tm;

    if(  clock_gettime(CLOCK_REALTIME, &tm) == -1) {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }

    tm.tv_sec = tm.tv_sec + theSeconds;
    tm.tv_nsec = tm.tv_nsec + theNanoSeconds;

    if( sem_timedwait(sem, &tm) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }
}

ipclib::Result ipclib::Semaphore::signal() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: signaling on " << name << "...";
    #endif

    if( sem_post(sem) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }
}

ipclib::Result ipclib::Semaphore::deallocateResources() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: deallocating resources for " << name << "...";
    #endif

    if( sem_close(sem) == 0 ) {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        return Result(Result::SUCCESS);
    }

    else {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        return res;
    }
}
