//
// Created by v on 16-10-28.
//

unsigned char ps_pack_start_code [4] = { 0x00, 0x00, 0x01, 0xba };
unsigned char ps_pack_end_code [4] = { 0x00, 0x00, 0x01, 0xb9 };

unsigned char sys_head_start_code [4] = { 0x00, 0x00, 0x01, 0xbb };

unsigned char packet_start_code_prefix[3] = { 0x00, 0x00, 0x01 };

static unsigned int s_iFrames;

static NC_UN_INT16 bytesToUint16(const unsigned char * p)
{
    NC_UN_INT16 value = 0;

    value = ((*p++) << 8);
    value = value + (*p);

    return value;
}

static NC_UN_INT32 bytesToUint32(const unsigned char * p)
{
    NC_UN_INT32 value = 0;

    value = ((*p++) << 24);
    value = value + ((*p++) << 16);
    value = value + ((*p++) << 8);
    value = value + (*p);

    return value;
}

static NC_UN_INT32 parseMuxRate(const unsigned char *p)
{
    NC_UN_INT32 tmp;

    tmp = *p++;
    tmp <<= 8;
    tmp += *(p++);
    tmp <<= 8;
    tmp += *(p++);
    tmp = (tmp & 0xfffffc) >> 2;

    return tmp;
}

static NC_UN_INT16 parseSCRbase(const unsigned char *p)
{
    NC_UN_INT32 tmp;

    tmp = *p++;
    tmp <<= 8;
    tmp += *(p++);
    tmp <<= 8;
    tmp += *p;
    tmp = (tmp & 0x3fff8) >> 3;

    return tmp;
}

static NC_UN_INT16 parseSCRextension(const unsigned char *p)
{
    NC_UN_INT16 tmp;

    tmp = *p++;
    tmp <<= 8;
    tmp += *p;
    tmp = (tmp & 0x3fe) >> 1;

    return tmp;
}

NcPsPacketParser::NcPsPacketParser()
        : m_pBuff(NULL), m_iLen(0), m_bOutputPes(false)
{
    m_pEsAnalyzer = new CESPacketAnalyzer();
}

//NcPsPacketParser::NcPsPacketParser(const unsigned char * pBuff, unsigned int len)
//	: m_pBuff(NULL), m_iLen(len), m_bOutputPes(false)
//{
//	m_pEsAnalyzer = new CESAnalyzer();
//}

NcPsPacketParser::~NcPsPacketParser()
{
    if (NULL != m_pEsAnalyzer)
    {
        delete m_pEsAnalyzer;
        m_pEsAnalyzer = NULL;
    }
}

NC_PS_ERROR NcPsPacketParser::Parse(const unsigned char *pData, int len, NcPsParsedData &psParsedData)
{
    NC_PS_ERROR result = OK;
    int buffLen = len;

    psParsedData.SetPackHead(false);
    psParsedData.SetSysHead(false);
    psParsedData.SetPESPacket(false);

    do
    {
        // Is PS packete start code ?
        if (memcmp(pData, ps_pack_start_code, sizeof(ps_pack_start_code)) == 0)
        {
            result = ParsePsPackHead(pData, len, psParsedData.PsPackHead);
            if (result != OK)
            {
                return result;
            }
            psParsedData.SetPackHead(true);

            psParsedData.AddReadLength(14 + psParsedData.PsPackHead.stuffingLenth);

            int buffLen = len -14 - psParsedData.PsPackHead.stuffingLenth;
            pData = pData + 14 + psParsedData.PsPackHead.stuffingLenth;

            if (memcmp(pData, sys_head_start_code, sizeof(sys_head_start_code)) == 0)
            {
                result = ParsePsSystemHead(pData, buffLen, psParsedData.PsSysHead);
                if (result != OK)
                {
                    return result;
                }

                psParsedData.SetSysHead(true);

                psParsedData.AddReadLength(6+psParsedData.PsSysHead.head_length);
            }
        }

        // Is PES packete start code prefix ?
        if (memcmp(pData, packet_start_code_prefix, sizeof(packet_start_code_prefix)) == 0)
        {
            result = ParsePESPacket(pData, buffLen, psParsedData.PesPacket);
            if (result != OK)
            {
                return result;
            }
            psParsedData.SetPESPacket(true);
        }
        else
        {
            pickupMpeg2Data(pData, len, psParsedData.PesPacket);
        }
    } while(0);

    return OK;
}

NC_PS_ERROR NcPsPacketParser::ParsePsPackHead(const unsigned char * pBuff, int len, PS_PACK_HEAD &psPackHead)
{
    const unsigned char *p = pBuff;
    int tmp;//32bit temp

    memset(&psPackHead, 0, sizeof(PS_PACK_HEAD));

    //if (memcmp(p, ps_pack_start_code, sizeof(ps_pack_start_code)) != 0)
    //{
    //	return NOT_CORRECT_SATR_CODE;
    //}

    psPackHead.start_code = bytesToUint32(p);

    p += 4;
    tmp = *p;
    if ( ((tmp >> 6) & 3) != 1)//always set 01
    {
        return NOT_MPEG2;
    }

    psPackHead.sys_colock_ref_base = ((*p) >> 3) & 0x7;
    psPackHead.scr_marker1 = ((*p) >> 2) & 0x1;
    psPackHead.scr_base1 = parseSCRbase(p);

    p += 2;
    psPackHead.scr_marker2 = ((*p) >> 2) & 0x1;
    psPackHead.scr_base2 = parseSCRbase(p);

    p += 2;
    psPackHead.scr_marker3 = ((*p) >> 2) & 0x1;

    psPackHead.scr_ext = parseSCRextension(p);//p->8

    // get mux rate
    p += 2;
    psPackHead.multiplex_rate = parseMuxRate(p);

    p = pBuff + 13;

    psPackHead.stuffingLenth = (*p) & 0x07;

    return OK;
}

NC_PS_ERROR NcPsPacketParser::ParsePsSystemHead(const unsigned char * pBuff, int len, PS_SYSTEM_HEAD &psSysHead)
{
    NC_PS_ERROR result = OK;

    const unsigned char * p = pBuff;

    memset(&psSysHead, 0, sizeof(PS_SYSTEM_HEAD));

    //if (memcmp(p, sys_head_start_code, sizeof(sys_head_start_code)) != 0)
    //{
    //	return NOT_CORRECT_SATR_CODE;
    //}

    psSysHead.start_code = bytesToUint32(p);

    p = p + 4;
    psSysHead.head_length = bytesToUint16(p);

    p++;
    psSysHead.marker1 = (*p) & 0x80;
    psSysHead.rate_bound = (*p) & 0x7F;
    psSysHead.rate_bound = psSysHead.rate_bound << 8;
    p++;
    psSysHead.rate_bound = psSysHead.rate_bound + (*p);
    psSysHead.rate_bound = psSysHead.rate_bound << 8;
    p++;
    psSysHead.rate_bound = psSysHead.rate_bound + (*p)&0xFE;
    psSysHead.rate_bound = psSysHead.rate_bound >> 1;

    p++;
    //psSysHead.audio_bound =

    p = pBuff + 12;
    psSysHead.stream_id = (*p);

    return result;
}

NC_PS_ERROR NcPsPacketParser::ParsePESPacket(const unsigned char * pBuff, int len, PES_PACKET &pesPacket)
{
    NC_PS_ERROR result = OK;
    const unsigned char * p = pBuff;
    const unsigned char * pDataStart = NULL;
    const unsigned char * pDataEnd = NULL;
    NC_UN_INT16 dataCount = 0;
    NC_UN_INT16 buffLen = 0;

    NC_UN_INT16 flags;

    memset(&pesPacket, 0, sizeof(PES_PACKET));

    //if (memcmp(p, packet_start_code_prefix, sizeof(packet_start_code_prefix)) != 0)
    //{
    //	return NOT_CORRECT_PES_PREFIX;
    //}

    p += 3; // skip start_code_prefix to stream_id
    pesPacket.stream_id = *p++;

    pesPacket.pesPacketLength = *p++;
    pesPacket.pesPacketLength <<= 8;
    pesPacket.pesPacketLength += *p++;// p point to [Always set to 10]

    pDataEnd = p + pesPacket.pesPacketLength;

    if (pesPacket.stream_id == NC_PS_PADDING_STREAM)
    {
        return PADDING_STREAM_SKIP;
    }

    flags = *(p++) << 8;
    flags += *(p++);

    pesPacket.alwaysFlag = (flags >> 14) & 0x3;

    if (pesPacket.alwaysFlag != 0x2)
    {
        return BAD_STREAM;
    }

    pesPacket.PESheadDataLength = *(p++);

    pDataStart = p + pesPacket.PESheadDataLength;

    dataCount = pDataEnd - pDataStart;
    buffLen = len - (pDataStart - pBuff);

    dataCount = dataCount > buffLen ? buffLen : dataCount;

    if (m_bOutputPes)
    {
        pickupMpeg2Data(pDataStart, dataCount, pesPacket);
    }

    this->m_pEsAnalyzer->Analyse(pDataStart, dataCount, pesPacket.esData);
    if (1)
    {
    }

    return result;
}

NC_PS_ERROR NcPsPacketParser::pickupMpeg2Data(const unsigned char * pBuff, int len, PES_PACKET &pesPacket)
{
    if (!m_bOutputPes)
    {
        return OK;
    }

    FILE *fp = fopen(m_file, "ab");
    if (fp == NULL)
    {
        return OTHER_ERROR;
    }

    fwrite(pBuff, sizeof(unsigned char), len, fp);

    fclose(fp);

    return OK;
}

NC_PS_ERROR NcPsPacketParser::PickupPESToFile()
{
    m_bOutputPes = true;

    createFileName();

    return OK;
}