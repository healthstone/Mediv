#include "KafkaMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Item.h"
#include "MailMgr.h"
#include "ObjectMgr.h"
#include "World.h"

#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

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

void KafkaMgr::handleExternalMail(std::string const &command) {
    TC_LOG_INFO("server.worldserver", "RECEIVED externalMail from KAFKA: {}", command);

    json data;
    try {
        data = json::parse(command);

        /** Structure of json
        {
            "receiver_guid": 1,
            "subject": "subject",
            "body": "body",
            "money": 1000000,
            "items": [
                {
                  "itemId": 123,
                  "itemCount": 1
                }
            ]
        }
        **/
        if (data.contains("receiver_guid") && data.contains("subject") && data.contains("body") &&
            data.contains("money")) {

            uint32 receiver_guid = data.at("receiver_guid");
            std::string subject = data.at("subject");
            std::string body = data.at("body");
            uint32 money = data.at("money");
            std::vector<Item *> itemVector;

            TC_LOG_INFO("server.worldserver", "RECEIVED externalMail subject: {} and  body: {}", subject, body);

            if (data.contains("items") && data.at("items").is_array()) {
                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

                // iterate the array
                for (json::iterator it = data.at("items").begin(); it != data.at("items").end(); ++it) {
                    uint32 itemId = it->at("itemId");
                    uint32 itemCount = it->at("itemCount");
                    if (itemId) {
                        if (!sObjectMgr->GetItemTemplate(itemId))
                            TC_LOG_ERROR("server.worldserver", "RECEIVED WRONG Item entry {} with count {}", itemId,
                                         itemCount);
                        else {
                            if (Item *mailItem = Item::CreateItem(itemId, itemCount)) {
                                itemVector.push_back(mailItem);
                                mailItem->SaveToDB(trans);
                                TC_LOG_INFO("server.worldserver",
                                            "externalMail> Adding {} of item with id {} for player_id {}", itemCount,
                                            itemId, receiver_guid);
                            }
                        }
                    }
                }

                CharacterDatabase.CommitTransaction(trans);
            }

            sMailMgr->SendMailWithItemsByGUID(0, receiver_guid, MAIL_NORMAL, subject, body, money, itemVector);
            itemVector.clear();
        }
    } catch (json::parse_error &ex) {
        TC_LOG_ERROR("server.worldserver", "RECEIVED WRONG externalMail: {}", ex.what());
    }
}

void KafkaMgr::handleExternalMailWithError(std::string const &command) {
    TC_LOG_ERROR("server.worldserver", "RECEIVED UNHANDLED externalMail from KAFKA: {}", command);
}