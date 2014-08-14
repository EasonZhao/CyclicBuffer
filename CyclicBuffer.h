#ifndef _CYCLICBUFFER_H_
#define _CYCLICBUFFER_H_

#include <stdlib.h>

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

    size_t buffer_size(void);

    size_t write_avail(void);

    size_t read_avail(void);

    size_t create(size_t const & size);

    void destory(void);

    void reset(void);

    bool is_full(void);

private:
    struct Buffer_ptr
    {
        char *p;
        long offset;
        Buffer_ptr():
            p(NULL),
            offset(0)
        {}
    };
    size_t save_buffer_size(void);

    size_t write_buffer_size(void);

    size_t read_buffer_size(void);

    size_t save_buffer_capacity(void);

    size_t drop_save(size_t const &size);

    size_t buffer_size(char* const beg, char* const end);

    size_t buffer_size(Buffer_ptr& beg, Buffer_ptr& end);

    int move_pos(char* &pos, int step);

private:
    char* buffer_;  
    Buffer_ptr read_pos_;
    Buffer_ptr write_pos_;
    Buffer_ptr save_pos_;
    size_t capacity_;
    bool is_create_;
    size_t drop_size_;
};

#endif
