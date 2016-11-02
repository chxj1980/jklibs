//
// Created by v on 16-10-28.
//

#ifndef JKLIBS_PS_PARSE_H
#define JKLIBS_PS_PARSE_H
typedef struct _PS_PACK_HEAD
{
    NC_UN_INT32 start_code;//0-3
    NC_UN_INT16 sys_colock_ref_base;//4.3-4.5
    bool scr_marker1;//4.6
    NC_UN_INT32 scr_base1;//4.7-6.5
    bool scr_marker2;//6.6
    NC_UN_INT32 scr_base2;//6.7-8.5
    bool scr_marker3;//8.6
    NC_UN_INT16 scr_ext;//8.7-9.7
    bool scr_ext_marker;//9.8
    NC_UN_INT32 multiplex_rate;//10-12.6
    bool mulp_marker1;//12.7
    bool mulp_marker2;//12.8
    NC_UN_INT16 reserved;//13.1-13.5
    NC_UN_INT16 stuffingLenth;//13.6-13.8
} PS_PACK_HEAD;

typedef struct _PS_SYSTEM_HEAD
{
    NC_UN_INT32 start_code;//0-3
    NC_UN_INT16 head_length;//4-5
    bool marker1;//6.1
    NC_UN_INT32 rate_bound;//6.2-8.7
    bool marker2;//8.8
    NC_UN_INT16 audio_bound;//9.1-9.6
    bool fixed_flag;
    bool csps_flag;
    bool sys_audio_local_flag;
    bool sys_video_local_flag;
    bool marker3;//10.3
    NC_UN_INT16 video_bound;//10.4-10.8
    bool packet_rate_restric_flag;
    NC_UN_INT16 reserved;//11.2-11.8
    NC_UN_INT16 stream_id;//12
    bool P_STD_buffer_bound_scale;//13.3
    NC_UN_INT16 P_STD_buff_size_bound;//13.4-14.8
} PS_SYSTEM_HEAD;

typedef struct _PES_PACKET
{
    NC_UN_INT16 stream_id;// 3 | 110xxxxx for audio,1110xxxx for video;
    NC_UN_INT16 pesPacketLength;//4-5(2B)
    // If stream ID is Private Stream 2 or Padding Stream, skip to data bytes.
    NC_UN_INT16 alwaysFlag;
    NC_UN_INT16 scramblingControl;//6.3-6.4
    bool priority;//6.5
    bool alignment;//6.6
    bool copyrighted;//6.7
    bool original;//6.8

    // These flags are set if the corresponding data structure follows the datalength byte.

    bool PTS;
    bool DTS;
    bool ESCR;
    bool ESrate;
    bool DSMtrickMode;
    bool additional;
    bool CRC;
    bool extensionFlag;
    NC_UN_INT16 PESheadDataLength;

    unsigned char * pESData;
    NC_UN_INT16 ESDataLength;

    ES_DATA esData;

} PES_PACKET;

class NcPsParsedData
{
public:
    NcPsParsedData(){
        m_hasSysHead = false; m_readLenth = 0;

        memset(&PsPackHead, 0, sizeof(PS_PACK_HEAD));
        memset(&PsSysHead, 0, sizeof(PS_SYSTEM_HEAD));
        memset(&PesPacket, 0, sizeof(PES_PACKET));

    }
    ~NcPsParsedData(){}

public:
    PS_PACK_HEAD PsPackHead;
    PS_SYSTEM_HEAD PsSysHead;
    PES_PACKET PesPacket;

public:
    const bool HasPackHead(){
        return m_hasPackHead;
    }

    const bool HasSysHead(){
        return m_hasSysHead;
    }

    const bool HasPESPacket(){
        return m_hasPESPacket;
    }

    void SetPackHead(bool val){
        m_hasPackHead = val;
    }

    void SetSysHead(bool val){
        m_hasSysHead = val;
    }

    void SetPESPacket(bool val){
        m_hasPESPacket = val;
    }

    void AddReadLength(unsigned int addLength){ m_readLenth = m_readLenth + addLength; }
    const unsigned int ReadLenght(){ return m_readLenth; }

    bool IsVideoStream()
    {
        // Examples: Audio streams (0xC0-0xDF), Video streams (0xE0-0xEF)
        if (PesPacket.stream_id >= 0xE0 && PesPacket.stream_id <= 0xEF)
        {
            return true;
        }

        return false;
    }

    bool IsAudioStream()
    {
        if (PesPacket.stream_id >= 0xC0 && PesPacket.stream_id <= 0xDF)
        {
            return true;
        }

        return false;
    }

private:
    bool m_hasPackHead;
    bool m_hasSysHead;
    bool m_hasPESPacket;
    unsigned int m_readLenth;
};

class NcPsPacketParser
{
public:
    NcPsPacketParser();
    //NcPsPacketParser(const unsigned char * pBuff, unsigned int len);
    ~NcPsPacketParser();

    NC_PS_ERROR Parse(const unsigned char *pData, int len, NcPsParsedData &psParsedData);

    NC_PS_ERROR ParsePsPackHead(const unsigned char * pBuff, int len, PS_PACK_HEAD &psPackHead);

    NC_PS_ERROR ParsePsSystemHead(const unsigned char * pBuff, int len, PS_SYSTEM_HEAD &psSysHead);

    NC_PS_ERROR ParsePESPacket(const unsigned char * pBuff, int len, PES_PACKET &pesPacket);

    NC_PS_ERROR PickupPESToFile();

private:
    NC_PS_ERROR pickupMpeg2Data(const unsigned char * pBuff, int len, PES_PACKET &pesPacket);
    int createFileName();
private:
    const unsigned char * m_pBuff;
    unsigned int m_iLen;

    char m_file[256];
    bool m_bOutputPes;

    CESPacketAnalyzer * m_pEsAnalyzer;
};
#endif //JKLIBS_PS_PARSE_H
