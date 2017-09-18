#ifndef _RESULT_H_
#define _RESULT_H_

#include <string>

namespace ipclib {

    class Result {
        private:
            int error;
            std::string description;

        public:
            static const int SUCCESS = 0;

            Result(const int theError = SUCCESS, const std::string& theDescription = "");

            int getError() const { return error; }
            std::string getDescription() const { return description; }
            bool isSuccesful() const { return !error; }

            void set(const int theError, const std::string& theDescription) { error = theError; description = theDescription; }
            void setError(const int theError) { error = theError; }
            void setDescription(const std::string& theDescription) { description = theDescription; }

            operator bool() const { return isSuccesful(); }
    };

}

#endif
