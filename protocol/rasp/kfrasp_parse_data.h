/*
 *===================================================================
 *
 *          Name: kfrasp_parse_data.h
 *        Create: 2015年11月17日 星期二 09时14分40秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus@roamter.com
 *
 *===================================================================
 */
#ifndef __KF_RASP_PARSE_DATA_H
#define __KF_RASP_PARSE_DATA_H

#include "json.h"
#include "rasp/kfrasp.h"

// Parse use these functions when parse down, from parse data.
// It will parse data to string/structs depends on resultCode and command/subcommand
// Parse to string if resultCode != success
// Parse to structs if resultCode == success

typedef struct {
    int     num;   // number identified which function use which process.
    // Process for parse, parse to data
    // data may be string and structs(defined in rasp/kfrasp_parse_data.h)
    int     (*process)(struct json *J, KFRaspBody *rb);
} KFRaspParseDataHandle;


// Choose the structs you need to call the parse,
// It will generate error string or real value to structs you point.
extern KFRaspParseDataHandle KFRaspParseDeviceInfo;
//extern KFRaspParseDataHandle KFRaspParseDeviceInfoResult;
extern KFRaspParseDataHandle KFRaspParseBindListsResult;
//extern KFRaspParseDataHandle KFRaspParseBindRequestResult;
extern KFRaspParseDataHandle KFRaspParseCommonResult;

#endif  // __KF_RASP_PARSE_DATA_H

/*=============== End of file: kfrasp_parse_data.h =====================*/
