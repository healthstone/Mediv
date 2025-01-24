#include "KafkaBroker.h"
#include "World.h"
#include "KafkaMgr.h"

#include <iostream>

#include "cppkafka/consumer.h"
#include "cppkafka/configuration.h"

using cppkafka::Consumer;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

void KafkaBrokerThread(const std::string &brokers, const std::string &commandsTopic, const std::string &externalMailTopic) {
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
                KafkaMgr::handleExternalMail(output.str());

                // Now commit the message
                consumerExternalMail.commit(externalMailMsg);
            }
        }

    }
}