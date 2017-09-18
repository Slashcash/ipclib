#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include <mqueue.h>
#include <string>

#include "posix_object.h"
#include "result.h"

namespace ipclib {

    class MsgQueue : public PosixObject {
        public:
            enum Protection {
                READ_ONLY,
                WRITE_ONLY,
                READ_AND_WRITE
            };

        private:
            static const long ATTRIBUTE_ERROR = -1;

            mqd_t msg_queue;
            Protection protection;

            Result deallocateResources();
            Result getPosixAttribute(mq_attr* theAttribute);

        public:
            MsgQueue() : PosixObject() {}
            MsgQueue(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const bool isNonBlocking = false, const Protection& theProtection = READ_AND_WRITE);

            Protection getProtection() const { return protection; }
            long getMaxMsgSize();
            long getMaxMsg();
            long getMsgNumber();
            bool isNonBlocking();
            bool isEmpty() { return !getMsgNumber(); }

            Result create(const std::string& theName, const bool toCreate = true, const bool toCreateExclusively = false, const bool isNonBlocking = false, const Protection& theProtection = READ_AND_WRITE);
            Result destroy();
            Result send(const std::string& theMsg);
            Result send(const std::string& theMsg, const long theSeconds, const long theNanoSeconds = 0);
            Result receive(std::string& theBuffer);
            Result receive(std::string& theBuffer, const long theSeconds, const long theNanoSeconds = 0);

            virtual ~MsgQueue() { deallocateResources(); }
    };

}

#endif
