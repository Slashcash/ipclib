#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

#include "result.h"

namespace ipclib {

    class Thread {
        private:
            pthread_t thread_id;

        public:
            Thread() {}

            Result run(void* theFunction(void*), void* theParameter = nullptr);
            Result join(void** theReturnValue = nullptr);
            Result cancel();
    };

}

#endif
