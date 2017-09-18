#ifndef _POSIX_SEMAPHORE_H_
#define _POSIX_SEMAPHORE_H_

#include <semaphore.h>

#include "posix_object.h"
#include "result.h"

namespace ipclib {

    class Semaphore : public PosixObject {
        private:
            sem_t* sem;

            Result deallocateResources();

        public:
            Semaphore() : PosixObject() { sem = nullptr; }
            Semaphore(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const unsigned int theValue = 1);

            int getValue();

            Result create(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const unsigned int theValue = 1);
            Result destroy();
            Result wait();
            Result wait(const long theSeconds, const long theNanoSeconds = 0);
            Result signal();

            virtual ~Semaphore() { deallocateResources(); }
    };

}

#endif
