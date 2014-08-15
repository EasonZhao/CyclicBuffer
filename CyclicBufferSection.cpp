#include "CyclicBufferSection.h"
#include "string.h"
#include <algorithm>

CyclicBufferSection::CyclicBufferSection(char* &buffer, size_t& capacity, char* &end, long offset) :
    buffer_(buffer),
    capacity_(capacity),
    beg_(buffer_),
    end_(end),
    offset_(offset)
{
}

long CyclicBufferSection::offset(void)
{
    return offset_;
}

char* CyclicBufferSection::pos(void)
{
    return beg_;
}

int CyclicBufferSection::move(int value)
{
    int step = value % capacity_;
    beg_ += value;
    /*
    if (step > 0) {
        int len_to_end = (buffer_ + capacity_) - beg_;
        if (step <= len_to_end) {
            beg_ += step;
        } else {
            beg_ = buffer_ + (step - len_to_end);
        }
    } else {
        int len_to_beg = beg_ - buffer_;
        if ((-step) <= len_to_beg) {
            beg_ += step;
        } else {
            beg_ = (buffer_ + capacity_) + (step + len_to_beg);
        }
    }
    */
    offset_ += value;
    return step;
}

int CyclicBufferSection::write(char *data, const size_t data_len)
{
    int data_to_write = std::min(data_len, avail_size());
    int len_to_end = (buffer_ + capacity_) - beg_;
    if (len_to_end >= data_to_write) {
        memcpy(beg_, data, data_to_write);
    } else {
        memcpy(beg_, data, len_to_end);
        memcpy(buffer_, data + len_to_end, data_to_write - len_to_end);
    }
    move(data_to_write);
    return data_to_write;
}

int CyclicBufferSection::read(char* data, const size_t data_len)
{
    int data_to_read = std::min(avail_size(), data_len);
    int len_to_end = (buffer_ + capacity_) - beg_;
    if (len_to_end >= data_to_read) {
        memcpy(data, beg_, data_to_read);
    } else {
        memcpy(data, beg_, len_to_end);
        memcpy(data + len_to_end, buffer_, data_to_read - len_to_end);
    }
    return data_to_read;
}

size_t CyclicBufferSection::capacity(void)
{
    return capacity_;
}
    
size_t CyclicBufferSection::avail_size(void)
{
    size_t avail_size = 0;
    if (end_ >= beg_) {
        avail_size = end_ - beg_;
    } else {
        avail_size = (end_ + capacity_) - beg_;
    }
    return avail_size;
}

CyclicBufferSection& CyclicBufferSection::operator+=(long value)
{
    move(value);
    return *this;
}

CyclicBufferSection& CyclicBufferSection::operator-=(long value)
{
    move(-value);
    return *this;
}

void CyclicBufferSection::reset_offset(void)
{
    offset_ = 0;
    beg_ = buffer_;
}
    
