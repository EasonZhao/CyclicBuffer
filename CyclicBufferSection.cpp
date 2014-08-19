#include "CyclicBufferSection.h"
#include "string.h"
#include <algorithm>
#include <assert.h>

CyclicBufferSection::CyclicBufferSection(char* &buffer, size_t& capacity, long offset) :
    buffer_(buffer),
    capacity_(capacity),
    pos_(buffer_),
    offset_(offset)
{
}

long CyclicBufferSection::offset(void)
{
    return offset_;
}

char* &CyclicBufferSection::pos(void)
{
    return pos_;
}

int CyclicBufferSection::move_pos(int value)
{
    pos_ += value;
    if (pos_ >= (buffer_ + capacity_))
        pos_ -= capacity_;
    else if (pos_ < buffer_)
        pos_ += capacity_;
    offset_ += value;
    return value;
}

int CyclicBufferSection::write(char *data, const size_t data_len)
{
    assert(data_len <= capacity_);
    size_t len_to_end = (buffer_ + capacity_) - pos_;
    if (len_to_end >= data_len) {
        memcpy(pos_, data, data_len);
    } else {
        memcpy(pos_, data, len_to_end);
        memcpy(buffer_, data + len_to_end, data_len - len_to_end);
    }
    move_pos(data_len);
    return data_len;
}

int CyclicBufferSection::read(char* data, const size_t data_len)
{
    assert(data_len <= capacity_);
    size_t len_to_end = (buffer_ + capacity_) - pos_;
    if (len_to_end >= data_len) {
        memcpy(data, pos_, data_len);
    } else {
        memcpy(data, pos_, len_to_end);
        memcpy(data + len_to_end, buffer_, data_len - len_to_end);
    }
    return data_len;
}

size_t CyclicBufferSection::capacity(void)
{
    return capacity_;
}

CyclicBufferSection& CyclicBufferSection::operator+=(long value)
{
    move_pos(value);
    return *this;
}

CyclicBufferSection& CyclicBufferSection::operator-=(long value)
{
    move_pos(-value);
    return *this;
}

void CyclicBufferSection::reset_offset(long offset)
{
    offset_ = offset;
    pos_ = buffer_;
}
    
