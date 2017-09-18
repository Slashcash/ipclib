#include "msg_queue.h"

#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#include <iostream>
#endif

ipclib::MsgQueue::MsgQueue(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const bool isNonBlocking, const Protection& theProtection) : PosixObject() {
    create(theName, toCreate, toCreateExclusively, isNonBlocking, theProtection);
}

long ipclib::MsgQueue::getMaxMsgSize() {
    mq_attr attribute;
    if( getPosixAttribute(&attribute) ) return attribute.mq_msgsize;
    else return ATTRIBUTE_ERROR;
}

long ipclib::MsgQueue::getMaxMsg() {
    mq_attr attribute;
    if( getPosixAttribute(&attribute) ) return attribute.mq_maxmsg;
    else return ATTRIBUTE_ERROR;
}

long ipclib::MsgQueue::getMsgNumber() {
    mq_attr attribute;
    if( getPosixAttribute(&attribute) ) return attribute.mq_curmsgs;
    else return ATTRIBUTE_ERROR;
}

bool ipclib::MsgQueue::isNonBlocking() {
    mq_attr attribute;
    if( getPosixAttribute(&attribute) ) {
        if( attribute.mq_flags == O_NONBLOCK ) return true;
        else return false;
    }

    else return ATTRIBUTE_ERROR;
}

ipclib::Result ipclib::MsgQueue::create(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const bool isNonBlocking, const Protection& theProtection) {
    if( already_initialized ) deallocateResources();

    PosixObject::create(theName);

    #ifdef NDEBUG
    std::cout << "IPCLIB: starting initialization for " << name << "...";
    #endif

    protection = theProtection;
    int oflag;
    if( theProtection == READ_ONLY ) oflag = O_RDONLY;
    else if( theProtection == WRITE_ONLY ) oflag = O_WRONLY;
    else oflag = O_RDWR;

    if( toCreate ) oflag = oflag | O_CREAT;
    if( toCreateExclusively ) oflag = oflag | O_EXCL;
    if( isNonBlocking ) oflag = oflag | O_NONBLOCK;

    if( (msg_queue = mq_open(posix_name.c_str(), oflag, DEFAULT_PERMISSION, NULL)) == (mqd_t)(-1) ) {
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

ipclib::Result ipclib::MsgQueue::deallocateResources() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: deallocating resources for " << name << "...";
    #endif

    if( mq_close(msg_queue) == 0 ) {
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

ipclib::Result ipclib::MsgQueue::getPosixAttribute(mq_attr* theAttribute) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: retrieving the POSIX attribute for message queue " << name << "...";
    #endif

    if( mq_getattr(msg_queue, theAttribute) == 0 ) {
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

ipclib::Result ipclib::MsgQueue::destroy() {
    #ifdef NDEBUG
    std::cout << "IPCLIB: destroying message queue " << name << "...";
    #endif

    if( mq_unlink(posix_name.c_str()) == 0 ) {
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

ipclib::Result ipclib::MsgQueue::send(const std::string& theMsg) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: sending message " << theMsg << " on " << name << "...";
    #endif

    if( mq_send(msg_queue, theMsg.c_str(), theMsg.size()+1, 0) == 0 ) {
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

ipclib::Result ipclib::MsgQueue::receive(std::string& theBuffer) {
    theBuffer.clear();

    long buffersize;
    if( (buffersize = getMaxMsgSize()) == ATTRIBUTE_ERROR ) return Result(-1, "Error in retrieving message buffer size");
    else buffersize++;

    char* buffer = new char[buffersize];

    #ifdef NDEBUG
    std::cout << "IPCLIB: receiving message on "<< name << "...";
    #endif

    if( mq_receive(msg_queue, buffer, buffersize, NULL) == -1 ) {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        delete [] buffer;
        return res;
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        theBuffer = buffer;
        delete [] buffer;
        return Result(Result::SUCCESS);
    }
}

ipclib::Result ipclib::MsgQueue::send(const std::string& theMsg, const long theSeconds, const long theNanoSeconds) {
    #ifdef NDEBUG
    std::cout << "IPCLIB: sending message " << theMsg << " with a timeout on " << name << "...";
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

    if( mq_timedsend(msg_queue, theMsg.c_str(), theMsg.size(), 0, &tm) == 0 ) {
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

ipclib::Result ipclib::MsgQueue::receive(std::string& theBuffer, const long theSeconds, const long theNanoSeconds) {
    theBuffer.clear();

    long buffersize;
    if( (buffersize = getMaxMsgSize()) == ATTRIBUTE_ERROR ) return Result(ATTRIBUTE_ERROR, "Error in retrieving message buffer size");
    else buffersize++;

    char* buffer = new char[buffersize];

    #ifdef NDEBUG
    std::cout << "IPCLIB: receiving message with a timeout on "<< name << "...";
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

    if( mq_timedreceive(msg_queue, buffer, buffersize, NULL, &tm) == -1 ) {
        Result res(errno, strerror(errno));
        #ifdef NDEBUG
        std::cout << "FAILED with error " << res.getError() << "\n";
        #endif
        delete [] buffer;
        return res;
    }

    else {
        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        theBuffer = buffer;
        delete [] buffer;
        return Result(Result::SUCCESS);
    }
}
