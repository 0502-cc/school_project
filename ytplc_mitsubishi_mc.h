/**************************************************************************

Author: CaoJian

Date:   2019-07-11

Description: YtPlc_Mitsubishi_Mc is a class of PLC control with Mitsubishi MELSEC protocol
             Already used in HeBei BOE

**************************************************************************/
#ifndef YTPLC_MITSUBISHI_MC_H
#define YTPLC_MITSUBISHI_MC_H
#include "ytplc.h"
#include <vector>
// 软元件代码
typedef enum
{
    e_plc_code_mitsubishi_d = 0xa8,
    e_plc_code_mitsubishi_m = 0x90,
    e_plc_code_mitsubishi_t = 0xc2,
    e_plc_code_mitsubishi_w = 0xb4
}TYP_PLC_CODE_MITSUBISHI_MC;

typedef enum
{
    top,
    top_result,
    bottom,
    bottom_result,
    recipe,
    aisle
}PLC_ID_TYPE;

class YtPlc_Mitsubishi_Mc : public YtPlc
{
public:
    YtPlc_Mitsubishi_Mc();

    //PLC写入指令，包含了报文拼接，socket发送，接收，结果确认
    bool PLCWrite(const int plc_code, const int* write_data, const int len, const int start_addr);

    //PLC去取指令，包含了报文拼接，socket发送，接收，结果解析
    bool PLCRead(const int plc_code, const int start_addr, const int len, int* recv_data, int& recv_size);

    bool YtPlc_Mitsubishi_Mc::PLCWriteBatch(const int plc_code, const int* write_data, const int len, const int start_addr);

    bool YtPlc_Mitsubishi_Mc::PLCWriteInt32(const int plc_code, const int32_t data, const int start_addr);

    bool YtPlc_Mitsubishi_Mc::PLCWrite32(const int plc_code, const int* write_data, const int len, const int start_addr);
    //初始化参数
    void InitParam();

    //获取RecipeNo、通道
    bool GetRecipe(const int read_addr, int& recipe);

    //获取ID
    bool GetID(const int read_addr, const int len, QString& id_string);

    //void PrintBuffer(const char* buffer, size_t size);
    //写入结果
    bool WriteResult(const int write_addr, const QString result);

    bool write_code(std::string str_code) {
        int len_code = str_code.length();
        int num[10];
        for (int i = 0; i < 10; i++) {
            if (i + 1 <= len_code / 2 + len_code % 2) {
                /*num[i] = ((unsigned int)(str_code[2 * i] << 8)) + ((unsigned int)str_code[2 * i + 1]);*/
                num[i] = ((unsigned int)str_code[2 * i]) + ((unsigned int)(str_code[2 * i + 1] << 8));
            }
            else
                num[i] = 0;
        }
        return PLCWrite(0xA8, &num[0], 10, 10160);
    };
};

#endif // YTPLC_MITSUBISHI_MC_H
