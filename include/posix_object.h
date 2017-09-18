#ifndef _POSIX_OBJECT_H_
#define _POSIX_OBJECT_H_

#include <string>

#include "result.h"

namespace ipclib {

    class PosixObject {
        protected:
            static const mode_t DEFAULT_PERMISSION = 0644;

            bool already_initialized;
            std::string name;
            std::string posix_name;
            Result initialization_result;

            PosixObject();
            std::string getPosixName() const { return posix_name; }

            void create(const std::string& theName);

        public:
            bool isAlreadyInitialized() const { return already_initialized; }
            std::string getName() const { return name; }
            Result getInitializationResult() const { return initialization_result; }

            ~PosixObject() {}
    };

}

#endif

