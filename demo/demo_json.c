/*
 *===========================================================================
 *
 *          Name: demo_json.c
 *        Create: 2015年10月22日 星期四 17时22分22秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>

#include "rt_print.h"
#include "json.h"

struct json *J;

int demo_generate_json_string(struct json *Js)
{
    int error = 0;

    json_push(Js, ".MsgHead");
    json_setstring(Js, "updateJudge", ".ServiceCode");
    json_setstring(Js, "0001", ".SrcSysID");
    json_setstring(Js, "200fc6722180482e08cf0cc48e684b18", ".SrcSysSign");
    json_setstring(Js, "1445507381196", ".transactionID");
    json_setnumber(Js, 23, ".MoreTF");

    json_pop(Js);

    json_push(Js, ".MsgBody[0]");

    json_setstring(Js, "000C43762058", ".PARAMS.mac");
    json_setstring(Js, "14.15.1019.1739.06c2d", ".PARAMS.version");

    json_pop(Js);

    char save[2048] = {0};
    int ret = json_printstring(Js, save, 2048, JSON_F_NONE, &error);
    if (ret <= 0) {
        rterror("error print string, %d", error);
        return -1;
    }
    rtinfo("String is: %s", save);

    return 0;
}

int demo_generate_json_parse(struct json *Js)
{
    struct json *JParse;
    int error = 0;
    JParse = json_open(JSON_F_NONE, &error);

    const char *parsedata = "{\"MsgBody\":[{\"PARAMS\":{\"mac\":\"000C43762058\",\"version\":\"14.15.1019.1739.06c2d\"}}],\"MsgHead\":{\"MoreTF\":28,\"ServiceCode\":\"updateJudge\",\"SrcSysID\":\"0001\",\"SrcSysSign\":\"200fc6722180482e08cf0cc48e684b18\",\"transactionID\":\"1445507381196\"}}";

    //const char *parsedata = "{\"MsgBody\":[{\"PARAMS\":{\"mac\":\"000C43762058\",\"version\":\"14.15.1019.1739.06c2d\"}}],\"MsgHead\":{\"ServiceCode\":\"updateJudge\",\"SrcSysID\":\"0001\",\"SrcSysSign\":\"200fc6722180482e08cf0cc48e684b18\",\"transactionID\":\"1445507381196\"}}";

    int lenparse = strlen(parsedata);

    error = json_loadstring(JParse, parsedata);
    if (error != 0) {
        rterror("json parse failed, %d", error);
        json_close(JParse);
        return -1;
    }

    rtinfo("msg body length: %d", json_count(JParse, ".MsgBody"));

    rtinfo("mac: %s", json_string(JParse, ".MsgBody[0].PARAMS.mac"));
    rtinfo("version: %s", json_string(JParse, ".MsgBody[0].PARAMS.version"));
    rtinfo("servicecode: %s", json_string(JParse, ".MsgHead.ServiceCode"));
    rtinfo("SrcSysID: %s", json_string(JParse, ".MsgHead.SrcSysID"));
    rtinfo("More TF: %.f", json_number(JParse, ".MsgHead.MoreTF"));

    json_close(JParse);

    return 0;
}

int main() {
    int error = 0;
    
    J = json_open(JSON_F_NONE, &error);
    if (!J) {
        rterror("Error open json %d", error);
        return -1;
    }

    demo_generate_json_string(J);
    demo_generate_json_parse(J);

    json_close(J);
    return 0;
}

/*=============== End of file: demo_json.c ==========================*/
