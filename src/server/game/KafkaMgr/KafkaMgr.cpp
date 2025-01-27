#include "KafkaMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Item.h"
#include "MailMgr.h"
#include "ObjectMgr.h"

#include <iostream>

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
    sWorld->QueueCliCommand(new CliCommandHolder(&kafkaCommand, command.c_str(), &KafkaProcessCommand::CommandPrint,
                                                 &KafkaProcessCommand::CommandFinished));
}

void KafkaMgr::handleCommandWithError(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED UNHANDLED command from KAFKA: {}", command);
}

void KafkaMgr::handleExternalMail(uint32 const &receiver_guid, std::string const &subject, std::string const &body,
                                  uint32 const &money, std::unordered_map<uint32, uint32> const &items) {
    std::vector<Item *> itemVector;
    if (!items.empty()) {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        for (auto const &itr: items) {
            if (!sObjectMgr->GetItemTemplate(itr.first))
                TC_LOG_ERROR("server.worldserver", "handleExternalMail::WRONG Item entry {} with count {}",
                             itr.first,
                             itr.second);
            else {
                if (Item *mailItem = Item::CreateItem(itr.first, itr.second)) {
                    itemVector.push_back(mailItem);
                    mailItem->SaveToDB(trans);
                    TC_LOG_INFO("server.worldserver", "externalMail> Adding {} of item with id {} for player_id {}",
                                itr.second, itr.first, receiver_guid);
                }
            }
        }
        CharacterDatabase.CommitTransaction(trans);
    }

    sMailMgr->SendMailWithItemsByGUID(0, receiver_guid, MAIL_NORMAL, subject, body, money, itemVector);
    itemVector.clear();
}

void KafkaMgr::handleExternalMailWithError(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED UNHANDLED externalMail from KAFKA: {}", command);
}