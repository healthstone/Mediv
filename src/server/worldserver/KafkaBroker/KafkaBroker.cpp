#include "KafkaBroker.h"
#include "KafkaMgr.h"
#include "World.h"

#include <nlohmann/json.hpp>
#include <iostream>

#include "cppkafka/consumer.h"
#include "cppkafka/configuration.h"

using cppkafka::Consumer;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

using json = nlohmann::json;

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
void fillExternalMailData(std::string const &jsonString, uint32 &receiver_guid, std::string &subject, std::string &body,
                          uint32 &money, std::unordered_map<uint32, uint32> &items) {
    json data;
    try {
        data = json::parse(jsonString);

        if (data.contains("receiver_guid") && data.contains("subject") && data.contains("body") &&
            data.contains("money")) {

            receiver_guid = data.at("receiver_guid");
            subject = data.at("subject");
            body = data.at("body");
            money = data.at("money");

            if (data.contains("items") && data.at("items").is_array()) {
                // iterate the array
                for (json::iterator it = data.at("items").begin(); it != data.at("items").end(); ++it) {
                    uint32 itemId = it->at("itemId");
                    uint32 itemCount = it->at("itemCount");
                    items[itemId] = itemCount;
                }
            }
        }
    } catch (json::parse_error &ex) {
        TC_LOG_ERROR("server.worldserver", "RECEIVED WRONG externalMail: {}", ex.what());
    }
}

void clearExternalMailData(std::string &jsonString, uint32 &receiver_guid, std::string &subject, std::string &body,
                           uint32 &money,
                           std::unordered_map<uint32, uint32> &items) {
    jsonString = "";
    receiver_guid = 0;
    subject = "";
    body = "";
    money = 0;
    items.clear();
}

void
KafkaBrokerThread(const std::string &brokers, const std::string &commandsTopic, const std::string &externalMailTopic) {
    std::string group_id = "1";

    // Construct the configuration
    Configuration config = {
            {"metadata.broker.list", brokers},
            {"group.id",             group_id},
            // Disable auto commit
            {"enable.auto.commit",   false}
    };

    // 1 TOPIC
    // Create the consumerCommands
    Consumer consumerCommands(config);

    // Print the assigned partitions on assignment
    consumerCommands.set_assignment_callback([](const TopicPartitionList &partitions) {
        std::cout << "consumerCommands Got assigned: " << partitions << std::endl;
    });

    // Print the revoked partitions on revocation
    consumerCommands.set_revocation_callback([](const TopicPartitionList &partitions) {
        std::cout << "consumerCommands Got revoked: " << partitions << std::endl;
    });

    // Subscribe to the topic
    consumerCommands.subscribe({commandsTopic});

    // 2 TOPIC
    // Create the consumerExternalMail
    Consumer consumerExternalMail(config);

    // Print the assigned partitions on assignment
    consumerExternalMail.set_assignment_callback([](const TopicPartitionList &partitions) {
        std::cout << "consumerExternalMail Got assigned: " << partitions << std::endl;
    });

    // Print the revoked partitions on revocation
    consumerExternalMail.set_revocation_callback([](const TopicPartitionList &partitions) {
        std::cout << "consumerExternalMail Got revoked: " << partitions << std::endl;
    });

    // Subscribe to the topic
    consumerExternalMail.subscribe({externalMailTopic});

    KafkaMgr::Initialize(commandsTopic, externalMailTopic);

    //allocate variables only one time
    std::string jsonString, subject, body;
    uint32 receiver_guid, money;
    std::unordered_map<uint32, uint32> attachmentMap;

    while (!World::IsStopped()) {
        // Try to consume a message
        Message commandsMsg = consumerCommands.poll();
        if (commandsMsg) {
            // If we managed to get a message
            if (commandsMsg.get_error()) {
                // Ignore EOF notifications from rdkafka
                if (!commandsMsg.is_eof()) {
                    KafkaMgr::handleCommandWithError(commandsMsg.get_error().to_string());
                }
            } else {
                std::ostringstream output;
                output << commandsMsg.get_payload();
                KafkaMgr::handleCommand(output.str());

                // Now commit the message
                consumerCommands.commit(commandsMsg);
            }
        }

        Message externalMailMsg = consumerExternalMail.poll();
        if (externalMailMsg) {
            // If we managed to get a message
            if (externalMailMsg.get_error()) {
                // Ignore EOF notifications from rdkafka
                if (!externalMailMsg.is_eof()) {
                    KafkaMgr::handleExternalMailWithError(externalMailMsg.get_error().to_string());
                }
            } else {
                std::ostringstream output;
                output << externalMailMsg.get_payload();
                jsonString = output.str();
                TC_LOG_INFO("server.worldserver", "RECEIVED externalMail from KAFKA: {}", jsonString);

                fillExternalMailData(jsonString, receiver_guid, subject, body, money, attachmentMap);
                KafkaMgr::handleExternalMail(receiver_guid, subject, body, money, attachmentMap);
                clearExternalMailData(jsonString, receiver_guid, subject, body, money, attachmentMap);

                // Now commit the message
                consumerExternalMail.commit(externalMailMsg);
            }
        }
    }
}