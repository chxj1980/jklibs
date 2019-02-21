//
// Created by v on 18-6-11.
//
#include <fstream>
#include <unistd.h>

#include "cm_msg.h"

cm_msg::cm_msg() {
    root.clear();
    reader = builder.newCharReader();
}

cm_msg::~cm_msg() {
    delete reader;
}

int cm_msg::parse(std::string json) {
    root.clear();
    int ret = reader->parse(json.c_str(), json.c_str() + json.length(), &root, NULL);
    return ret;
}

int cm_msg::serialize(std::string &json) {
    json = root.toStyledString();
    return 0;
}

std::string cm_msg::serialize() {
    return root.toStyledString();
}

int cm_msg::fileread(const char *filename) {
    if (access(filename, F_OK) != 0) {
        return -1;
    }
    std::ifstream ifs;
    ifs.open(filename);
    root.clear();

    int ret = Json::parseFromStream(builder, ifs, &root, NULL);
    if (!ret) {
        ifs.close();
        return -2;
    }
    ifs.close();
    return ret;
}

int cm_msg::filewrite(const char *filename) {
    std::ofstream ofs;
    ofs.open(filename);

    ofs << root;
    ofs.close();

    return 0;
}
