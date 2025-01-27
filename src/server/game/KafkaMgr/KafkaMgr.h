#ifndef __KAFKALMGR_H
#define __KAFKALMGR_H

#include "Define.h"

namespace ns {

    struct externalMailItem {
        int32 itemId;
        int32 itemCount;
    };

    struct externalMail {
        ObjectGuid::LowType receiver_guid;
        std::string subject;
        std::string body;
        std::vector<externalMailItem> items;
    };
}

class KafkaProcessCommand {
public:
    KafkaProcessCommand() = default;

    ~KafkaProcessCommand() = default;

    static void CommandPrint(void * /*callbackArg*/, std::string_view /*text*/) {};

    static void CommandFinished(void * /*callbackArg*/, bool /*bool*/) {};
};

class TC_GAME_API KafkaMgr {
private:
    KafkaMgr();

    ~KafkaMgr();

public:
    static KafkaMgr *instance();

    static void Initialize(std::string const &commandsTopicName, std::string const &externalMailTopicName);

    static void handleCommand(std::string const &command);

    static void handleCommandWithError(std::string const &command);

    static void handleExternalMail(std::string const &command);

    static void handleExternalMailWithError(std::string const &command);
};

#define sKafkaMgr KafkaMgr::instance()

#endif
