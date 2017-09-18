#include "result.h"

#include <string>

ipclib::Result::Result(const int theError, const std::string& theDescription) {
    error = theError;
    description = theDescription;
}
