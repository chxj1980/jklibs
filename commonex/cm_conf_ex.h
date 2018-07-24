//
// Created by v on 18-6-11.
//

#ifndef SRC_CM_CONF_EX_H
#define SRC_CM_CONF_EX_H

#include <string>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

int cm_config_read_ex(const char *filename, std::map<std::string, std::string> &data);
int cm_config_write_ex(const char *filename, std::map<std::string, std::string> data);

#ifdef __cplusplus
}
#endif

#endif //SRC_CM_CONF_EX_H
