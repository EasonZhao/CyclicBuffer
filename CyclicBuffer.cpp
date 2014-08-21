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
    history_capacity_(0),
    drop_size_(0)
{
    create();
}

CyclicBuffer::~CyclicBuffer()
{
    destory();
}

size_t CyclicBuffer::write(const char *data, size_t const &len)
{
    assert(len <= write_avail());
    int ret = write_section_->write(data, len);
    //更新history数据
    size_t cap = write_section_->offset() - history_section_->offset();
    if (cap > capacity_)
        (*history_section_) += cap - capacity_;
    return ret;
}

size_t CyclicBuffer::write(boost::asio::const_buffer buffer)
{
    const char *p = boost::asio::buffer_cast<const char *>(buffer);
    size_t size = boost::asio::buffer_size(buffer);
    return write(p, size);
}

size_t CyclicBuffer::read(char *data, size_t const &len)
{
    assert(len <= read_avail());
    size_t read_ret = read_section_->read(data, len);
    drop_size_ = read_ret;
    drop();
    return read_ret;
}

void CyclicBuffer::read(std::vector<boost::asio::const_buffer> &buffers)
{
    buffers.clear();
    long read_size = read_avail();
    long len_to_end = (buffer_ + capacity_) - read_section_->pos();
    if (read_size <= len_to_end) {
        buffers.resize(1);
        buffers[0] = boost::asio::const_buffer(read_section_->pos(), read_size);
    } else {
        buffers.resize(2);
        buffers[0] = boost::asio::const_buffer(read_section_->pos(), len_to_end);
        buffers[1] = boost::asio::const_buffer(buffer_, read_size - len_to_end);
    }
    drop_size_ = read_size;
}

int CyclicBuffer::drop(const unsigned long size)
{
    int drop_size = std::min(size, drop_size_);
    (*read_section_) += drop_size;
    drop_size_ = 0;
    return 0;
}

void CyclicBuffer::set_read_offset(long const &offset)
{
    if (read_section_->offset() == offset)
        return ;
    if (offset < history_section_->offset() || 
        offset > write_section_->offset()) {
        reset(offset);
    } else {
        //数据复用
        long step = offset - read_section_->offset();
        (*read_section_) += step;
        drop_size_ = 0;
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
    //可以写入数据大小
    long write_size = capacity_ - (write_section_->offset() - read_section_->offset());
    //历史数据大小
    size_t history_size = read_section_->offset() - history_section_->offset();
    //减去历史数据
    write_size -= std::min(history_size, history_capacity());
    return write_size;
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
    history_capacity_ = capacity_ / 3;
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
}

void CyclicBuffer::reset(long offset)
{
    history_section_->reset_offset(offset);
    read_section_->reset_offset(offset);
    write_section_->reset_offset(offset);
    drop_size_ = 0;
}

bool CyclicBuffer::is_full(void)
{
    return write_avail()==0;
}

size_t CyclicBuffer::history_capacity(void)
{
    return history_capacity_;
}

size_t CyclicBuffer::resize(const size_t size)
{
    destory();
    capacity_ = size;
    create();
    return capacity();
}
