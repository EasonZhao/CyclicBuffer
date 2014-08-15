#ifndef CYCLICBUFFERSECTION_H_
#define CYCLICBUFFERSECTION_H_

#include <stdio.h>

///buffer分段信息
class CyclicBufferSection
{
public:
    ///构造函数
    ///@param[in] buffer 物理内存起始位置
    ///@param[in] capactiy 物理内存容量
    ///@param[in] end 有效数据结束位置（外部控制移动）
    ///@param[in] offset 逻辑io偏移位置
    CyclicBufferSection(char* &buffer, size_t& capacity, char* &end, long offset = 0);

    long offset(void);

    char* pos(void);

    ///写入函数
    ///@note 从beg开始写入，最多写入到end
    int write(char *data, const size_t data_len);

    ///@note 从beg开始读取，最多读取到end
    int read(char *data, const size_t data_len);

    ///获取有效数据长度
    size_t avail_size(void);

    size_t capacity(void);

    CyclicBufferSection& operator += (long value);

    CyclicBufferSection& operator -= (long value);

    ///重置
    void reset_offset(void);
private:
    int move(int step);

private:
    char* &buffer_;         ///<物理内存起始位置
    size_t & capacity_;     ///<物理内存容量
    char* beg_;               ///<有效数据结束位置
    char* &end_;            ///<有效数据结束位置
    long offset_;
};

#endif
