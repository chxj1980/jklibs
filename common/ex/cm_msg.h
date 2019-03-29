//
// Created by v on 18-6-11.
//

#ifndef SRC_CMMSG_H
#define SRC_CMMSG_H

#include "json/json.h"

class cm_msg {
public:
    cm_msg();
    ~cm_msg();

    int parse(std::string json);

    int serialize(std::string &json);
    std::string serialize();

    int fileread(const char *filename);
    int filewrite(const char *filename);

    Json::Value    root;

private:
    Json::CharReader         *reader;
    Json::CharReaderBuilder   builder;

};

#endif //SRC_CMMSG_H
