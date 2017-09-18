#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "posix_object.h"

#ifdef NDEBUG
#include <iostream>
#endif

namespace ipclib {

    template<class T> class SharedMemory : public PosixObject {
        public:
            enum Protection {
                READ_ONLY,
                WRITE_ONLY,
                READ_AND_WRITE
            };

        private:
            T* obj_address;
            Protection protection;

            Result deallocateResources();

        public:
            SharedMemory() : PosixObject() { obj_address = nullptr; }
            SharedMemory(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const Protection& theProtection = READ_AND_WRITE);

            Protection getProtection() const { return protection; }
            T& getValue() const { return *obj_address; }

            void setValue(const T& theValue) { *obj_address = theValue; }

            Result create(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const Protection& theProtection = READ_AND_WRITE);
            Result destroy();

            virtual ~SharedMemory() { deallocateResources(); }
    };

    template<class T> SharedMemory<T>::SharedMemory(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const Protection& theProtection): PosixObject() {
        create(theName, toCreate, toCreateExclusively, theProtection);
    }


    template<class T> Result SharedMemory<T>::deallocateResources() {
        #ifdef NDEBUG
        std::cout << "IPCLIB: deallocating resources for " << name << "...";
        #endif

        if( munmap(obj_address, sizeof(T)) == 0 ) {
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

    template <class T> Result SharedMemory<T>::create(const std::string& theName, const bool toCreate, const bool toCreateExclusively, const Protection& theProtection) {
        if( already_initialized ) deallocateResources();

        PosixObject::create(theName);

        #ifdef NDEBUG
        std::cout << "IPCLIB: starting initialization for " << name << "...";
        #endif

        int oflag;
        protection = theProtection;
        if( theProtection == READ_ONLY ) oflag = O_RDONLY;
        else oflag = O_RDWR;

        if( toCreate ) oflag = oflag | O_CREAT;
        if( toCreateExclusively ) oflag = oflag | O_EXCL;

        int fd;
        if( (fd = shm_open(posix_name.c_str(), oflag, DEFAULT_PERMISSION)) < 0 ) {
            Result res(errno, strerror(errno));
            #ifdef NDEBUG
            std::cout << "FAILED with error " << res.getError() << "\n";
            #endif
            initialization_result = res;
            return initialization_result;
        }

        if( ftruncate(fd, sizeof(T)) == -1 ) {
            Result res(errno, strerror(errno));
            #ifdef NDEBUG
            std::cout << "FAILED with error " << res.getError() << "\n";
            #endif
            initialization_result = res;
            return initialization_result;
        }

        int prot;
        if( theProtection == READ_ONLY ) prot = PROT_READ;
        if( theProtection == WRITE_ONLY ) prot = PROT_WRITE;
        else prot = PROT_READ | PROT_WRITE;

        if( (obj_address = (T*)(mmap(NULL, sizeof(T), prot, MAP_SHARED, fd, 0))) == MAP_FAILED ) {
            Result res(errno, strerror(errno));
            #ifdef NDEBUG
            std::cout << "FAILED with error " << res.getError() << "\n";
            #endif
            initialization_result = res;
            return initialization_result;
        }

        if( close(fd) == -1 ) {
            Result res(errno, strerror(errno));
            #ifdef NDEBUG
            std::cout << "FAILED with error " << res.getError() << "\n";
            #endif
            initialization_result = res;
            return initialization_result;
        }

        #ifdef NDEBUG
        std::cout << "SUCCESS\n";
        #endif
        initialization_result = Result(Result::SUCCESS);
        return initialization_result;
    }

    template<class T> Result SharedMemory<T>::destroy() {
        #ifdef NDEBUG
        std::cout << "IPCLIB: destroying shared memory " << name << "...";
        #endif

        if( shm_unlink(posix_name.c_str()) == 0 ) {
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

}
#endif
