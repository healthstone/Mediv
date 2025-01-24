#ifndef __KAFKABROKER_H
#define __KAFKABROKER_H

#include "Define.h"
#include <string>

void KafkaBrokerThread(const std::string &brokers, const std::string &commandsTopic, const std::string &externalMailTopic);

#endif