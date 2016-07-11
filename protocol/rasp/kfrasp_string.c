/*
 *===========================================================================
 *
 *          Name: kfrasp_string.c
 *        Create: 2015年11月17日 星期二 13时54分55秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include "kfrasp_string.h"

const char *raspkey = "09d08113-74b4-4ada-a04e-17358b6d0203";

const char *m_head = "MsgHead";
const char *m_cmd = "ServiceCode";
const char *m_subcmd = "SrcSysID";
const char *m_tm = "transactionID";
const char *m_sign = "SrcSysSign";
const char *m_body = "MsgBody";
const char *m_result = "result";
const char *m_resultCode = "resultCode";
const char *m_resultData = "resultData";
const char *m_operation = "operation";
const char *m_ccmd = "cmd";
const char *m_source = "source";
const char *m_desc = "desc";

const char* KF_JSON_STRING(const char *str)
{
    static char m_json_str[64];
    snprintf(m_json_str, sizeof(m_json_str), ".%s", str);
    return m_json_str;
}

const char *kfrasp_m_key()
{
    return raspkey;
}

const char *kfrasp_m_head()
{
    return m_head;
}

const char *kfrasp_m_cmd()
{
    return m_cmd;
}

const char *kfrasp_m_subcmd()
{
    return m_subcmd;
}

const char *kfrasp_m_tm()
{
    return m_tm;
}

const char *kfrasp_m_sign()
{
    return m_sign;
}

const char *kfrasp_m_body()
{
    return m_body;
}

const char *kfrasp_m_result()
{
    return m_result;
}

const char *kfrasp_m_resultcode()
{
    return m_resultCode;
}

const char *kfrasp_m_resultdata()
{
    return m_resultData;
}

const char *kfrasp_m_operation()
{
    return m_operation;
}

const char *kfrasp_m_ccmd()
{
    return m_ccmd;
}

const char *kfrasp_m_source()
{
    return m_source;
}

const char *kfrasp_m_desc()
{
    return m_desc;
}
   
/*=============== End of file: kfrasp_string.c ==========================*/
