#include "posix_object.h"

ipclib::PosixObject::PosixObject() {
    already_initialized = false;
}

void ipclib::PosixObject::create(const std::string& theName) {
    name = theName;
    posix_name = "/" + theName;
    already_initialized = true;
}
