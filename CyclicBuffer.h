#ifndef _CYCLICBUFFER_H_
#define _CYCLICBUFFER_H_

#include <stdlib.h>
#include "CyclicBufferSection.h"

class CyclicBufferSection;

class CyclicBuffer
{
public:
    CyclicBuffer(const size_t size = 0);

    ~CyclicBuffer(void);

    size_t write(char *data, size_t const &len);

    size_t read(char *data, size_t const &len);

    int drop(int const &size = -1);

    void set_read_offset(long const &offset);

    long get_read_offset(void);

    long get_write_offset(void);

    ///获取buffer容量
    size_t capacity(void);

    size_t write_avail(void);

    size_t read_avail(void);

    size_t create(size_t const & size);

    void destory(void);

    void reset(long offset = 0);

    bool is_full(void);

private:
    size_t history_capacity(void);

    size_t drop_save(size_t const &size);

private:
    char* buffer_;  
    CyclicBufferSection* history_section_;
    CyclicBufferSection* write_section_;
    CyclicBufferSection* read_section_;
    size_t capacity_;
    bool is_create_;
    size_t drop_size_;
};

#endif
