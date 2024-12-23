#include "ytplc_mitsubishi_mc.h"

YtPlc_Mitsubishi_Mc::YtPlc_Mitsubishi_Mc()
{

}

bool YtPlc_Mitsubishi_Mc::PLCWriteBatch(const int plc_code, const int* write_data, const int len, const int start_addr)
{
    //const int len = 6;  // 假设你总是想写入六个元素

    // 长度从CPU监视定时器L到指令结束
    write_buff_[7] = (len * 2 + 12) & 0xff;          //请求数据长度L
    write_buff_[8] = ((len * 2 + 12) & 0xff00) >> 8; //请求数据长度H

    // 字单位的批量写入
    write_buff_[15] = start_addr & 0xff;         // 起始软元件
    write_buff_[16] = (start_addr >> 8) & 0xff;  // 起始软元件
    write_buff_[17] = (start_addr >> 16) & 0xff; // 起始软元件
    write_buff_[18] = plc_code;  // 软元件代码
    write_buff_[19] = len & 0xff;         // 软元件点数
    write_buff_[20] = (len & 0xff00) >> 8;// 软元件点数

    // 软元件点数的范围
    for (int i = 0; i < len; i++)
    {
        write_buff_[21 + i * 2] = write_data[i] & 0xff;
        write_buff_[21 + i * 2 + 1] = (write_data[i] & 0xff00) >> 8;
    }

    // 发送与接收
    char recv_buff[1024]{ 0 };
    int recv_buff_size = 0;
    if (!Transfer(write_buff_, 21 + len * 2, recv_buff, recv_buff_size))
        return false;

    // 结果确认 @《三菱Q_L系列通信协议参考》3.1.2 P67
    // 条件1：总长度 11
    // 条件2：副头部 D0 00
    // 条件3：结束代码 00 00
    if (recv_buff_size < 11)
    {
        return false;
    }
    if (recv_buff[0] != (char)0xd0 || recv_buff[1] != (char)0x00 || recv_buff[9] != (char)0x00 || recv_buff[10] != (char)0x00)
    {
        return false;
    }
    return true;
}

//32位传输
bool YtPlc_Mitsubishi_Mc::PLCWriteInt32(const int plc_code, const int32_t data, const int start_addr) 
{
    // 声明发送缓冲区
    char write_buff[21 + 4 * 2] = { 0 }; // 修改这里的数组大小为4 * 2，每个int32_t需要4个字节，每个字节占用2个字符

    // 从CPU监视定时器L到指令结束的长度
    write_buff[7] = (4 * 2 + 12) & 0xff;             // 请求数据长度低字节
    write_buff[8] = ((4 * 2 + 12) & 0xff00) >> 8;    // 请求数据长度高字节

    // 字单位的批量写入
    write_buff[15] = start_addr & 0xff;              // 起始软元件
    write_buff[16] = (start_addr >> 8) & 0xff;       // 起始软元件
    write_buff[17] = (start_addr >> 16) & 0xff;      // 起始软元件
    write_buff[18] = plc_code;                       // 软元件代码
    write_buff[19] = 4 & 0xff;                       // 软元件点数（4个字节）
    write_buff[20] = (4 & 0xff00) >> 8;              // 软元件点数

    // 将int32位整数拆分成四个8位字节
    for (int i = 0; i < 4; i++) 
    {
        write_buff[21 + i * 2] = static_cast<char>((data >> (i * 8)) & 0xFF);
    }

    // 发送与接收
    char recv_buff[1024]{ 0 };
    int recv_buff_size = 0;
    if (!Transfer(write_buff, 21 + 4 * 2, recv_buff, recv_buff_size)) {
        return false;
    }

    // 结果确认 @《三菱Q_L系列通信协议参考》3.1.2 P67
    // 条件1：总长度 11
    // 条件2：副头部 D0 00
    // 条件3：结束代码 00 00
    if (recv_buff_size < 11) {
        return false;
    }
    if (recv_buff[0] != (char)0xd0 || recv_buff[1] != (char)0x00 || recv_buff[9] != (char)0x00 || recv_buff[10] != (char)0x00) {
        return false;
    }
    return true;
}

bool YtPlc_Mitsubishi_Mc::PLCWrite(const int plc_code, const int* write_data, const int len, const int start_addr)
{
    // 长度从CPU监视定时器L到指令结束
    write_buff_[7] = (len * 2 + 12) & 0xff;          //请求数据长度L
    write_buff_[8] = ((len * 2 + 12) & 0xff00) >> 8; //请求数据长度H

    // 字单位的批量写入
    write_buff_[15] = start_addr & 0xff;         // 起始软元件
    write_buff_[16] = (start_addr >> 8) & 0xff;  // 起始软元件
    write_buff_[17] = (start_addr >> 16) & 0xff; // 起始软元件
    write_buff_[18] = plc_code;  // 软元件代码
    write_buff_[19] = len & 0xff;         // 软元件点数
    write_buff_[20] = (len & 0xff00) >> 8;// 软元件点数

    // 软元件点数的范围
    // e.g 1995H 1202H 1130H -> {95H 19H 02H 12H 30H 12H}
    for (int i = 0; i < len; i++)
    {
        write_buff_[21 + i * 2] = write_data[i] & 0xff;
        write_buff_[21 + i * 2 + 1] = (write_data[i] & 0xff00) >> 8;
    }

    // 发送与接收
    char recv_buff[1024]{ 0 };
    int recv_buff_size = 0;
    if (!Transfer(write_buff_, 21 + len * 2, recv_buff, recv_buff_size))
        // 打印接收缓存，即使在Transfer失败的情况下
        Log("Transfer failed, but received data:");
  /*      PrintBuffer(recv_buff, recv_buff_size);*/
        return false;

    // 结果确认 @《三菱Q_L系列通信协议参考》3.1.2 P67
    // 条件1：总长度 11
    // 条件2：副头部 D0 00
    // 条件3：结束代码 00 00
    if (recv_buff_size < 11)
    {
        Log("Response too short.");
        return false;
    }
    if (recv_buff[0] != (char)0xd0 || recv_buff[1] != (char)0x00 || recv_buff[9] != (char)0x00 || recv_buff[10] != (char)0x00)
    {
        return false;
    }

    return true;
}

//void PrintBuffer(const char* buffer, size_t size)
//{
//    std::stringstream ss;
//    for (size_t i = 0; i < size; ++i)
//    {
//        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(buffer[i]) << " ";
//    }
//    ss << std::endl;
//
//    // 将构建的日志消息传递给 Log 函数
//    Log(ss.str());
//}

bool YtPlc_Mitsubishi_Mc::PLCWrite32(const int plc_code, const int* write_data, const int len, const int start_addr)
{
    // 计算请求数据长度
    int request_length = len * 4 + 12;  // 每个int32占用4字节
    write_buff_[7] = request_length & 0xff;          // 请求数据长度L
    write_buff_[8] = (request_length & 0xff00) >> 8; // 请求数据长度H

    // 其他部分不变

    // 软元件点数的范围
    // e.g 1995H 1202H 1130H -> {95H 19H 02H 12H 30H 12H}
    for (int i = 0; i < len; i++)
    {
        write_buff_[21 + i * 4] = write_data[i] & 0xff;
        write_buff_[21 + i * 4 + 1] = (write_data[i] & 0xff00) >> 8;
        write_buff_[21 + i * 4 + 2] = (write_data[i] & 0xff0000) >> 16;
        write_buff_[21 + i * 4 + 3] = (write_data[i] & 0xff000000) >> 24;
    }

    // 发送与接收
    char recv_buff[1024]{ 0 };
    int recv_buff_size = 0;
    if (!Transfer(write_buff_, 21 + len * 4, recv_buff, recv_buff_size))
        return false;

    // 结果确认部分不变

    return true;
}


bool YtPlc_Mitsubishi_Mc::PLCRead(const int plc_code, const int start_addr, const int len, int* recv_data, int& recv_size)
{
    recv_size = 0;

    // 字单位的批量写入
    read_buff_[15] = start_addr & 0xff;         // 起始软元件
    read_buff_[16] = (start_addr >> 8) & 0xff;  // 起始软元件
    read_buff_[17] = (start_addr >> 16) & 0xff; // 起始软元件
    read_buff_[18] = plc_code;  // 软元件代码
    read_buff_[19] = len & 0xff;         // 软元件点数
    read_buff_[20] = (len & 0xff00) >> 8;// 软元件点数

    // 发送与接收
    char recv_buf[1024]{ 0 };
    int recv_buf_size = 0;
    if (!Transfer(read_buff_, 21 + len * 2, recv_buf, recv_buf_size))
        return false;

    // 结果确认 @《三菱Q_L系列通信协议参考》3.1.2 P66
    // 0~1副头部 D0 00
    // 9~10结束代码 00 00
    if (recv_buf[0] != (char)0xd0 || recv_buf[1] != (char)0x00 || recv_buf[9] != (char)0x00 || recv_buf[10] != (char)0x00)
    {
        return false;
    }

    // 7~8相应数据长度
    recv_size = ((recv_buf[7] | (recv_buf[8] << 8)) - 2) / 2;

    // 接收数据长度大于解析后数组长度并且解析后数据长度与之前申请相同
    if ((recv_buf_size < (11 + recv_size * 2)) || (recv_size != len))
    {
        return false;
    }

    // 数据解析
    for (int i = 0; i != recv_size; ++i)
    {
        recv_data[i] = recv_buf[11 + i * 2] | (recv_buf[12 + i * 2] << 8);
    }
    return true;
}

void YtPlc_Mitsubishi_Mc::InitParam()
{
    //发送命令
    write_buff_[0] = 0x50; // 副头部
    write_buff_[1] = 0x00; // 副头部
    write_buff_[2] = 0x00; // 网络编号
    write_buff_[3] = (char)0xff; // 可编程控制器编号
    write_buff_[4] = (char)0xff; // 请求目标模块I/O编号L
    write_buff_[5] = 0x03; // 请求目标模块I/O编号H
    write_buff_[6] = 0x00; // 请求目标模块站号

    // 长度从CPU监视定时器L到指令结束
    write_buff_[7] = 0x00; //请求数据长度L
    write_buff_[8] = 0x00; //请求数据长度H

    //CPU 监视定时器 0:无限等待 0001～FFFF H (1～65535): 等待时间(单位 250ms) @《三菱Q_L系列通信协议参考》3.1.3 P73
    write_buff_[9] = 0x01;  // CPU监视定时器L
    write_buff_[10] = 0x00; // CPU监视定时器H

    // 指令1401 @《三菱Q_L系列通信协议参考》3.3.7 P154
    write_buff_[11] = 0x01; // 指令L
    write_buff_[12] = 0x14; // 指令H
    write_buff_[13] = 0x00; // 子指令L
    write_buff_[14] = 0x00; // 子指令H

    // 字单位的批量写入
    write_buff_[15] = 0x00; // 起始软元件
    write_buff_[16] = 0x00; // 起始软元件
    write_buff_[17] = 0x00; // 起始软元件
    write_buff_[18] = 0x00; // 软元件代码
    write_buff_[19] = 0x00; // 软元件点数
    write_buff_[20] = 0x00; // 软元件点数

    // 软元件点数的范围
    // e.g 1995H 1202H 1130H -> {95H 19H 02H 12H 30H 12H}
    for (int i = 0; i < 20; i++)
    {
        write_buff_[21 + i * 2] = 0x00;
        write_buff_[21 + i * 2 + 1] = 0x00;
    }

    //读取命令
    read_buff_[0] = 0x50; // 副头部
    read_buff_[1] = 0x00; // 副头部
    read_buff_[2] = 0x00; // 网络编号
    read_buff_[3] = (char)0xff; // 可编程控制器编号
    read_buff_[4] = (char)0xff; // 请求目标模块I/O编号L
    read_buff_[5] = 0x03; // 请求目标模块I/O编号H
    read_buff_[6] = 0x00; // 请求目标模块站号

    read_buff_[7] = 0x0c; //请求数据长度L
    read_buff_[8] = 0x00; //请求数据长度H

    //CPU 监视定时器 0:无限等待 0001～FFFF H (1～65535): 等待时间(单位 250ms) @《三菱Q_L系列通信协议参考》3.1.3 P73
    read_buff_[9] = 0x01;  // CPU监视定时器L
    read_buff_[10] = 0x00; // CPU监视定时器H

    // 0401  T寄存器读取 @《三菱Q_L系列通信协议参考》3.3.6 P150
    // WARNING: M寄存器读取 send_buff_[18] = 0x90 待确认
    read_buff_[11] = 0x01; // 指令L
    read_buff_[12] = 0x04; // 指令H
    read_buff_[13] = 0x00; // 子指令L
    read_buff_[14] = 0x00; // 子指令H

    // 字单位的批量写入
    read_buff_[15] = 0x00; // 起始软元件
    read_buff_[16] = 0x00; // 起始软元件
    read_buff_[17] = 0x00; // 起始软元件
    read_buff_[18] = 0x00; // 软元件代码
    read_buff_[19] = 0x00; // 软元件点数
    read_buff_[20] = 0x00; // 软元件点数
}

bool YtPlc_Mitsubishi_Mc::GetRecipe(const int read_addr, int& recipe)
{
    int recv_size = 0;

    if (!PLCRead((int)e_plc_code_mitsubishi_w, read_addr, 1, &recipe, recv_size))
    {
        return false;
    }

    return true;
}

bool YtPlc_Mitsubishi_Mc::GetID(const int read_addr, const int len, QString& id_string)
{
    int recv_size = 0;
    int* recv_data = new int[len];
    char id_array[100]{ 0 };

    if (!PLCRead((int)e_plc_code_mitsubishi_w, read_addr, len, recv_data, recv_size))
    {
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        id_array[i * 2] = recv_data[i] & 0xff;
        id_array[i * 2 + 1] = (recv_data[i] & 0xff) >> 8;
    }

    id_string = QString(id_array);

    delete[] recv_data;
    recv_data = NULL;

    return true;
}

bool YtPlc_Mitsubishi_Mc::WriteResult(const int write_addr, const QString result)
{
    char result_array[3] = { 0 };
    int write_data[100] = { 0 };
    char* temp;

    temp = result.toLatin1().data();
    memcpy(result_array, temp, sizeof(result_array));

    write_data[0] = result_array[1] << 8 | result_array[0];

    if (!PLCWrite((int)e_plc_code_mitsubishi_w, write_data, 1, write_addr))
    {
        return false;
    }
    return true;
}
