#ifndef _PSUNPACKET_H_
#define _PSUNPACKET_H_
#include "streamdef.h"
#include <tuple>
#include <string.h>
#include <netinet/in.h>
#include <tr1/tuple>

//using namespace std::tr1;
#ifndef AV_RB16
#   define AV_RB16(x)                           \
    ((((const unsigned char*)(x))[0] << 8) |          \
    ((const unsigned char*)(x))[1])
#endif


static inline unsigned long ff_parse_pes_pts(const unsigned char* buf) {
    return (unsigned long)(*buf & 0x0e) << 29 |
        (AV_RB16(buf+1) >> 1) << 15 |
        AV_RB16(buf+3) >> 1;
}

#define __int64 long

static unsigned __int64 get_pts(optional_pes_header* option)
{
    if(option->PTS_DTS_flags != 2 && option->PTS_DTS_flags != 3 && option->PTS_DTS_flags != 0)
    {
        return 0;
    }
    if((option->PTS_DTS_flags & 2) == 2)
    {
        unsigned char* pts = (unsigned char*)option + sizeof(optional_pes_header);
        return ff_parse_pes_pts(pts);
    }
    return 0;
}

static unsigned __int64 get_dts(optional_pes_header* option)
{
    if(option->PTS_DTS_flags != 2 && option->PTS_DTS_flags != 3 && option->PTS_DTS_flags != 0)
    {
        return 0;
    }
    if((option->PTS_DTS_flags & 3) == 3)
    {
        unsigned char* dts = (unsigned char*)option + sizeof(optional_pes_header) + 5;
        return ff_parse_pes_pts(dts);
    }
    return 0;
}

bool inline is_ps_header(ps_header_t* ps)
{
    if(ps->pack_start_code[0] == 0 && ps->pack_start_code[1] == 0 && ps->pack_start_code[2] == 1 && ps->pack_start_code[3] == 0xBA)
        return true;
    return false;
}
bool inline is_sh_header(sh_header_t* sh)
{
    if(sh->system_header_start_code[0] == 0 && sh->system_header_start_code[1] == 0 && sh->system_header_start_code[2] == 1 && sh->system_header_start_code[3] == 0xBB)
        return true;
    return false;
}
bool inline is_psm_header(psm_header_t* psm)
{
    if(psm->promgram_stream_map_start_code[0] == 0 && psm->promgram_stream_map_start_code[1] == 0 && psm->promgram_stream_map_start_code[2] == 1 && psm->promgram_stream_map_start_code[3] == 0xBC)
        return true;
    return false;
}
bool inline is_pes_video_header(pes_header_t* pes)
{
    if(pes->pes_start_code_prefix[0]==0 && pes->pes_start_code_prefix[1] == 0 && pes->pes_start_code_prefix[2] == 1 && pes->stream_id == 0xE0)
        return true;
    return false;
}
bool inline is_pes_audio_header(pes_header_t* pes)
{
    if(pes->pes_start_code_prefix[0]==0 && pes->pes_start_code_prefix[1] == 0 && pes->pes_start_code_prefix[2] == 1 && pes->stream_id == 0xC0)
        return true;
    return false;
}

bool inline is_pes_header(pes_header_t* pes)
{
    if(pes->pes_start_code_prefix[0]==0 && pes->pes_start_code_prefix[1] == 0 && pes->pes_start_code_prefix[2] == 1)
    {
        if(pes->stream_id == 0xC0 || pes->stream_id == 0xE0)
        {
            return true;
        }
    }
    return false;
}
PSStatus inline pes_type(pes_header_t* pes)
{
    if(pes->pes_start_code_prefix[0]==0 && pes->pes_start_code_prefix[1] == 0 && pes->pes_start_code_prefix[2] == 1)
    {
        if(pes->stream_id == 0xC0)
        {
            return ps_pes_audio;
        }
        else if(pes->stream_id == 0xE0)
        {
            return ps_pes_video;
        }
    }
    return ps_padding;
}

/*
_1 是否包含数据
_2 下一个PS状态
_3 数据指针
_4 数据长度
*/
typedef std::tr1::tuple<bool, PSStatus, pes_header_t*> pes_tuple;
/*
_1 是否包含数据
_2 数据类型
_3 PTS时间戳
_4 DTS时间戳
_5 数据指针
_6 数据长度
*/
typedef std::tr1::tuple<bool, unsigned char, unsigned __int64, unsigned __int64, char*, unsigned int> naked_tuple;

class PSPacket
{
public:
    PSPacket()
    {
        m_status = ps_padding;
        m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
    }
    void PSWrite(char* pBuffer, unsigned int sz)
    {
        if(m_nPSWrtiePos + sz < MAX_PS_LENGTH)
        {
            memcpy((m_pPSBuffer + m_nPSWrtiePos), pBuffer, sz);
            m_nPSWrtiePos += sz;
        }
        else
        {
            m_status = ps_padding;
            m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
        }

    }
    void RTPWrite(char* pBuffer, unsigned int sz)
    {
        char* data = (pBuffer + sizeof(RTP_header_t));
        unsigned int length =  sz - sizeof(RTP_header_t);
        if(m_nPSWrtiePos + length < MAX_PS_LENGTH)
        {
            memcpy((m_pPSBuffer + m_nPSWrtiePos), data, length);
            m_nPSWrtiePos += length;
        }
        else
        {
            m_status = ps_padding;
            m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
        }
    }
	pes_tuple pes_payload()
    {
        if(m_status == ps_padding)
        {
            for(; m_nPESIndicator<m_nPSWrtiePos; m_nPESIndicator++)
            {
                m_ps = (ps_header_t*)(m_pPSBuffer + m_nPESIndicator);
                if(is_ps_header(m_ps))
                {
                    m_status = ps_ps;
                    break;
                }
            }
        }
        if(m_status == ps_ps)
        {
            for(; m_nPESIndicator<m_nPSWrtiePos; m_nPESIndicator++)
            {
                m_sh = (sh_header_t*)(m_pPSBuffer + m_nPESIndicator);
                m_pes = (pes_header_t*)(m_pPSBuffer + m_nPESIndicator);
                if(is_sh_header(m_sh))
                {
                    m_status = ps_sh;
                    break;
                }
                else if (is_pes_header(m_pes))
                {
                    m_status = ps_pes;
                    break;
                }
            }
        }
        if(m_status == ps_sh)
        {
            for(; m_nPESIndicator<m_nPSWrtiePos; m_nPESIndicator++)
            {
                m_psm = (psm_header_t*)(m_pPSBuffer + m_nPESIndicator);
                m_pes = (pes_header_t*)(m_pPSBuffer + m_nPESIndicator);
                if(is_psm_header(m_psm))
                {
                    m_status = ps_psm;//冲掉s_sh状态
                    break;
                }
                if(is_pes_header(m_pes))
                {
                    m_status = ps_pes;
                    break;
                }
            }
        }
        if(m_status == ps_psm)
        {
            for(; m_nPESIndicator<m_nPSWrtiePos; m_nPESIndicator++)
            {
                m_pes = (pes_header_t*)(m_pPSBuffer + m_nPESIndicator);
                if(is_pes_header(m_pes))
                {
                    m_status = ps_pes;
                    break;
                }
            }
        }
        if(m_status == ps_pes)
        {
            //寻找下一个pes 或者 ps
            unsigned short PES_packet_length = ntohs(m_pes->PES_packet_length);
            if((m_nPESIndicator + PES_packet_length + sizeof(pes_header_t)) < m_nPSWrtiePos)
            {
                char* next = (m_pPSBuffer + m_nPESIndicator + sizeof(pes_header_t) + PES_packet_length);
                pes_header_t* pes = (pes_header_t*)next;
                ps_header_t* ps = (ps_header_t*)next;
                m_nPESLength = PES_packet_length + sizeof(pes_header_t);
                memcpy(m_pPESBuffer, m_pes, m_nPESLength);
                if(is_pes_header(pes) || is_ps_header(ps))
                {
                    PSStatus status = ps_padding;
                    if(is_ps_header(ps))
                    {
                        status = m_status = ps_ps;
                    }
                    else
                    {
                        status = pes_type(pes);
                    }
                    int remain = m_nPSWrtiePos - (next - m_pPSBuffer);
                    memcpy(m_pPSBuffer, next, remain);
                    m_nPSWrtiePos = remain; m_nPESIndicator = 0;
                    m_ps = (ps_header_t*)m_pPSBuffer;
                    m_pes = (pes_header_t*)m_pPSBuffer;
                    return pes_tuple(true, status, (pes_header_t*)m_pPESBuffer);
                }
                else
                {
                    m_status = ps_padding;
                    m_nPESIndicator = m_nPSWrtiePos = 0;
                }
            }
        }
        return pes_tuple(false, ps_padding, NULL);
    }
    naked_tuple naked_payload()
    {
        naked_tuple tuple = naked_tuple(false, 0, 0, 0, NULL, 0);
        do 
        {
            pes_tuple t = pes_payload();
            if(! std::tr1::get<0>(t))
            {
                break;
            }
            PSStatus status = std::tr1::get<1>(t);
            pes_header_t* pes = std::tr1::get<2>(t);
            optional_pes_header* option = (optional_pes_header*)((char*)pes + sizeof(pes_header_t));
            if(option->PTS_DTS_flags != 2 && option->PTS_DTS_flags != 3 && option->PTS_DTS_flags != 0)
            {
                break;
            }
            unsigned __int64 pts = get_pts(option);
            unsigned __int64 dts = get_dts(option);
            unsigned char stream_id = pes->stream_id;
            unsigned short PES_packet_length = ntohs(pes->PES_packet_length);
            char* pESBuffer = ((char*)option + sizeof(optional_pes_header) + option->PES_header_data_length);
            int nESLength = PES_packet_length - (sizeof(optional_pes_header) + option->PES_header_data_length);
            memcpy(m_pESBuffer + m_nESLength, pESBuffer, nESLength);
            m_nESLength += nESLength;
            if(stream_id == 0xE0 && (status == ps_ps || status == ps_pes_audio))
            {
                tuple = naked_tuple(true, 0xE0, pts, dts, m_pESBuffer, m_nESLength);
                m_nESLength = 0;
            }
            else if(stream_id == 0xC0)
            {
                tuple = naked_tuple(true, 0xC0, pts, dts, m_pESBuffer, m_nESLength);
                m_nESLength = 0;
            }
        } while (false);
        return tuple;
    }
private:
    PSStatus      m_status;                     //当前状态
    char          m_pPSBuffer[MAX_PS_LENGTH];   //PS缓冲区
    unsigned int  m_nPSWrtiePos;                //PS写入位置
    unsigned int  m_nPESIndicator;              //PES指针
private:
    char          m_pPESBuffer[MAX_PES_LENGTH]; //PES缓冲区
    unsigned int  m_nPESLength;                 //PES数据长度
private:
 	ps_header_t*  m_ps;                         //PS头
 	sh_header_t*  m_sh;                         //系统头
 	psm_header_t* m_psm;                        //节目流头
 	pes_header_t* m_pes;                        //PES头
private:
    char         m_pESBuffer[MAX_ES_LENGTH];    //裸码流
    unsigned int m_nESLength;                   //裸码流长度
};

#endif