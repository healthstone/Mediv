#ifndef __KAFKALMGR_H
#define __KAFKALMGR_H

#include "Define.h"

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

    static void handleExternalMail(uint32 const &receiver_guid, std::string const &subject,
                                   std::string const &body,
                                   uint32 const &money, std::unordered_map<uint32, uint32> const &items);

    static void handleExternalMailWithError(std::string const &command);
};

#define sKafkaMgr KafkaMgr::instance()

#endif
