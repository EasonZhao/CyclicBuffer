#ifndef _CYCLICBUFFER_H_
#define _CYCLICBUFFER_H_

#include <stdlib.h>
#include "CyclicBufferSection.h"
#include <boost/asio/buffer.hpp>
#include <vector>

class CyclicBufferSection;

class CyclicBuffer
{
public:
    CyclicBuffer(const size_t size = 0);

    ~CyclicBuffer(void);

    size_t write(const char *data, size_t const &len);

    size_t write(boost::asio::const_buffer buffer);

    size_t read(char *data, size_t const &len);

    void read(std::vector<boost::asio::const_buffer> &buffers);

    int drop(const unsigned long size = -1);

    void set_read_offset(long const &offset);

    long get_read_offset(void);

    long get_write_offset(void);

    ///获取buffer容量
    size_t capacity(void);

    size_t write_avail(void);

    size_t read_avail(void);

    void reset(long offset = 0);

    bool is_full(void);

    ///重新设置大小
    ///@note resize会清空有效数据
    size_t resize(const size_t size);

private:
    size_t history_capacity(void);

    size_t drop_save(size_t const &size);

    size_t create(void);

    void destory(void);
private:
    char* buffer_;  
    CyclicBufferSection* history_section_;
    CyclicBufferSection* write_section_;
    CyclicBufferSection* read_section_;
    size_t capacity_;
    size_t drop_size_;
};

#endif
