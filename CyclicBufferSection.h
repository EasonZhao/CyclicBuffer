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
    ///@param[in] pos 有效数据起始位置（外部控制移动）
    ///@param[in] offset 逻辑io偏移位置
    CyclicBufferSection(char* &buffer, size_t& capacity, long offset = 0);

    long offset(void);

    char* & pos(void);

    ///写入函数
    ///@note 从beg开始写入，最多写入到end
    int write(const char *data, const size_t data_len);

    ///@note 从beg开始读取，最多读取到end
    int read(char *data, const size_t data_len);
    
    size_t capacity(void);

    CyclicBufferSection& operator += (long value);

    CyclicBufferSection& operator -= (long value);

    ///重置
    void reset_offset(long offset = 0);
private:
    int move_pos(int step);

private:
    char* &buffer_;         ///<物理内存起始位置
    size_t const & capacity_;     ///<物理内存容量
    char* pos_;               ///<有效数据结束位置
    long offset_;
};

#endif
