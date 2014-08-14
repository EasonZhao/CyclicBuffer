#include "CyclicBuffer.h"
#include <algorithm>
#include <string.h>

CyclicBuffer::CyclicBuffer(const size_t size) :
    buffer_(NULL),
    capacity_(0),
    is_create_(false),
    drop_size_(0)
{
    if (size>0)
        create(size);
}

CyclicBuffer::~CyclicBuffer()
{
    if (is_create_)
        destory();
}

size_t CyclicBuffer::write(char *data, size_t const &len)
{
    if (len<=0 && write_avail()==0) return 0;
    size_t data_to_write = std::min(len, write_avail());
    //尽量少的丢弃历史数据
    int save_to_drop = data_to_write - write_buffer_size();
    if (save_to_drop > 0)
        drop_save(save_to_drop);
    char *buffer_end = buffer_ + capacity_;
    if ((write_pos_.p + data_to_write) > buffer_end) {
        int len_to_end = buffer_end - write_pos_.p;
        memcpy(write_pos_.p, data, len_to_end);
        memcpy(buffer_, data + len_to_end, data_to_write - len_to_end);
        write_pos_.p = buffer_ + (data_to_write - len_to_end);
    } else {
        memcpy(write_pos_.p, data, data_to_write);
        write_pos_.p += data_to_write;
    }
    write_pos_.offset += data_to_write;
    return data_to_write;
}

size_t CyclicBuffer::read(char *data, size_t const &len)
{
    if (len<=0 && read_avail() == 0) return 0;
    int data_to_read = std::min(len, read_avail());
    int len_to_end = (buffer_ + capacity_) - read_pos_.p;
    if (len_to_end >= data_to_read) {
        memcpy(data, read_pos_.p, data_to_read);
    } else {
        memcpy(data, read_pos_.p, len_to_end);
        memcpy(data + len_to_end, buffer_, data_to_read - len_to_end);
    }
    drop_size_ = data_to_read;
    return data_to_read;
}

int CyclicBuffer::drop(int const &size)
{
    if (size <= 0 && size != -1) return 0;
    int drop_size = -1;
    if (size == -1) {
        drop_size = drop_size_;
    } else {
        drop_size = std::min(drop_size_, read_avail());
    }
    move_pos(read_pos_.p, drop_size);
    read_pos_.offset += drop_size;
    drop_size_ = 0;
    return drop_size;
}

void CyclicBuffer::set_read_offset(long const &offset)
{
    if (offset == read_pos_.offset) {
        return;
    }
    //数据复用
    if (offset >= save_pos_.offset && offset < get_write_offset()) {
        int step = offset - read_pos_.offset;
        move_pos(read_pos_.p, step);
        read_pos_.offset = offset;
    } else {
        reset();
        save_pos_.offset = offset;
        read_pos_.offset = offset;
        write_pos_.offset = offset;
    }
}

long CyclicBuffer::get_read_offset(void)
{
    return read_pos_.offset;
}

long CyclicBuffer::get_write_offset(void)
{
    return read_pos_.offset + read_avail();
}

size_t CyclicBuffer::buffer_size(void)
{
    return capacity_;
}

size_t CyclicBuffer::write_avail(void)
{
    //获取历史数据中可以丢弃数据大小
    int data_to_discard = save_buffer_size() - save_buffer_capacity();
    return write_buffer_size() + (data_to_discard > 0 ? data_to_discard : 0);
}

size_t CyclicBuffer::read_avail(void)
{
    return read_buffer_size();
}

size_t CyclicBuffer::create(size_t const &size)
{
    if (is_create_) 
        return -1;
    buffer_ = new char[size];
    if (buffer_==NULL)
        return -1;
    read_pos_.p = buffer_;
    write_pos_.p = buffer_;
    save_pos_.p = buffer_;
    capacity_ = size;
    reset();
    return buffer_size();
}

void CyclicBuffer::destory(void)
{
    if (is_create_) {
        delete[] buffer_;
        capacity_ = 0;
        drop_size_ = 0;
        buffer_ = NULL;
        read_pos_.p = NULL;
        write_pos_.p = NULL;
        save_pos_.p = NULL;
        is_create_ = false;
        reset();
    }
}

void CyclicBuffer::reset(void)
{
    write_pos_.p = buffer_;
    write_pos_.offset = 0;
    read_pos_.p = buffer_;
    read_pos_.offset = 0;
    save_pos_.p = buffer_;
    save_pos_.offset = 0;
}

bool CyclicBuffer::is_full(void)
{
    return write_avail()==0;
}

size_t CyclicBuffer::save_buffer_size(void)
{
    return buffer_size(save_pos_, read_pos_);
}

size_t CyclicBuffer::save_buffer_capacity(void)
{
    return capacity_ / 3;
}

size_t CyclicBuffer::drop_save(size_t const &size)
{
    int drop_size = std::min(size, (save_buffer_size() - save_buffer_capacity()));
    if (drop_size < 0) return 0;
    int len_to_end = (buffer_ + capacity_) - save_pos_.p;
    if (len_to_end > drop_size) {
        save_pos_.p += drop_size;
    } else {
        save_pos_.p = buffer_ + (drop_size - len_to_end);
    }
    save_pos_.offset += drop_size;
    return drop_size;
}

size_t CyclicBuffer::buffer_size(char* const beg, char* const end)
{
    size_t buffer_size = 0;
    char *buffer_end = buffer_ + capacity_;
    if (beg <= buffer_end && beg >= buffer_ 
        && end <= buffer_end && end >= buffer_) {
        //指针相同时返回0
        if (end >= beg) {
            buffer_size = end - beg;
        } else {
            buffer_size += buffer_end - beg;
            buffer_size += end - buffer_;
        }
    }
    return buffer_size;
}
    
size_t CyclicBuffer::write_buffer_size(void)
{
    return capacity_ - read_avail() - save_buffer_size();
}

size_t CyclicBuffer::read_buffer_size(void)
{
    return buffer_size(read_pos_, write_pos_);
}

size_t CyclicBuffer::buffer_size(Buffer_ptr& beg, Buffer_ptr& end)
{
    return end.offset - beg.offset;
}

int CyclicBuffer::move_pos(char* &pos, int step)
{
    if (step == 0) return 0;
    step = step % capacity_;
    if (step > 0) {
        int len_to_end = (buffer_ + capacity_) - pos;
        if (step <= len_to_end) {
            pos += step;
        } else {
            pos = buffer_ + (step - len_to_end);
        }
    } else {
        int len_to_beg = pos - buffer_;
        if ((-step) <= len_to_beg) {
            pos += step;
        } else {
            pos = (buffer_ + capacity_) + (step + len_to_beg);
        }
    }
    return step;
}

