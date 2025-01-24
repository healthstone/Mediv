#include "KafkaMgr.h"
#include "Log.h"
#include "World.h"

KafkaMgr *KafkaMgr::instance() {
    static KafkaMgr instance;
    return &instance;
}

KafkaMgr::KafkaMgr() = default;

KafkaMgr::~KafkaMgr() = default;

void KafkaMgr::Initialize(const std::string &commandsTopicName, std::string const &externalMailTopicName) {
    TC_LOG_INFO("server.worldserver", "Consuming commands messages from topic {}", commandsTopicName);
    TC_LOG_INFO("server.worldserver", "Consuming externalMail messages from topic {}", externalMailTopicName);
}

void KafkaMgr::handleCommand(std::string const &command) {
    TC_LOG_INFO("server.worldserver", "RECEIVED command from KAFKA: {}", command);

    KafkaProcessCommand kafkaCommand;
    sWorld->QueueCliCommand(new CliCommandHolder(&kafkaCommand, command.c_str(), &KafkaProcessCommand::CommandPrint, &KafkaProcessCommand::CommandFinished));
}

void KafkaMgr::handleCommandWithError(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED UNHANDLED command from KAFKA: {}", command);
}

void KafkaMgr::handleExternalMail(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED externalMail from KAFKA: {}", command);
}

void KafkaMgr::handleExternalMailWithError(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED UNHANDLED externalMail from KAFKA: {}", command);
}