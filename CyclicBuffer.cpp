#include "CyclicBuffer.h"
#include <algorithm>
#include <string.h>
#include <assert.h>

CyclicBuffer::CyclicBuffer(const size_t size) :
    buffer_(NULL),
    history_section_(NULL),
    write_section_(NULL),
    read_section_(NULL),
    capacity_(size),
    drop_size_(0)
{
    create();
}

CyclicBuffer::~CyclicBuffer()
{
    destory();
}

size_t CyclicBuffer::write(char *data, size_t const &len)
{
    size_t data_to_write = std::min(len, write_avail());
    size_t write_ret = write_section_->write(data, data_to_write);
    //更新history数据
    size_t cap = write_section_->offset() - history_section_->offset();
    if (cap > capacity_)
        (*history_section_) += cap - capacity_;
    return write_ret;
}

size_t CyclicBuffer::read(char *data, size_t const &len)
{
    size_t data_to_read = std::min(len, read_avail());
    size_t read_ret = read_section_->read(data, data_to_read);
    drop_size_ = std::max(drop_size_, read_ret);
    return read_ret;
}

int CyclicBuffer::drop(int const &size)
{
    int drop_size = std::min((size_t)size, drop_size_);
    (*read_section_) += drop_size;
    drop_size_ = 0;
    return 0;
}

void CyclicBuffer::set_read_offset(long const &offset)
{
    if (read_section_->offset() == offset)
        return ;
    //数据复用
    if (offset >= history_section_->offset() && 
        offset < write_section_->offset()) {
        long step = offset - read_section_->offset();
        (*read_section_) += step;
    } else {
        reset(offset);
    }
}

long CyclicBuffer::get_read_offset(void)
{
    return read_section_->offset();
}

long CyclicBuffer::get_write_offset(void)
{
    return write_section_->offset();
}

size_t CyclicBuffer::capacity(void)
{
    return capacity_;
}

size_t CyclicBuffer::write_avail(void)
{
/*
    //获取历史数据中可以丢弃数据大小
    int data_to_discard = save_buffer_size() - save_buffer_capacity();
    return write_section_size() + (data_to_discard > 0 ? data_to_discard : 0);
*/
    //获取历史数据大小
    size_t history_size = read_section_->offset() - history_section_->offset();
    return read_section_->offset() + capacity_ - write_section_->offset() - 
        std::min(history_size, history_capacity());
}

size_t CyclicBuffer::read_avail(void)
{
    return write_section_->offset() - read_section_->offset();
}

size_t CyclicBuffer::create(void)
{
    if (capacity_ == 0) capacity_ = 1;
    buffer_ = new char[capacity_];
    if (buffer_==NULL)
        return 0;
    history_section_ = new CyclicBufferSection(buffer_, capacity_);
    read_section_ = new CyclicBufferSection(buffer_, capacity_);
    write_section_ = new CyclicBufferSection(buffer_, capacity_);
    return capacity();
}

void CyclicBuffer::destory(void)
{
    delete[] buffer_;
    capacity_ = 0;
    drop_size_ = 0;
    buffer_ = NULL;
    delete history_section_;
    history_section_ = NULL;
    delete read_section_;
    read_section_ = NULL;
    delete write_section_;
    write_section_ = NULL;
    reset();
}

void CyclicBuffer::reset(long offset)
{
    if (history_section_) 
        history_section_->reset_offset(offset);
    if (read_section_)
        read_section_->reset_offset(offset);
    if (write_section_)
        write_section_->reset_offset(offset);
}

bool CyclicBuffer::is_full(void)
{
    return write_avail()==0;
}

size_t CyclicBuffer::history_capacity(void)
{
    return capacity_ / 3;
}

size_t CyclicBuffer::resize(const size_t size)
{
    reset();
    capacity_ = size;
    create();
    return capacity();
}
