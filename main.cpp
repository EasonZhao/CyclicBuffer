#include <iostream>
#include <gtest/gtest.h>
#include "CyclicBuffer.h"
#include <stdio.h>
#include <algorithm>
#include <fstream>

using namespace std;

int myrand(int const &max)
{
    srand(time(NULL));
    return rand() % max + 1;
}

TEST(CyclicBuffer, create)
{
    size_t size = 8*1024*1024;
    CyclicBuffer buff(size);
    EXPECT_EQ(size, buff.buffer_size());
}

TEST(CyclicBuffer, create2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb;
    size_t result = cb.create(size);
    bool ret = ((result==size) && (cb.buffer_size()==size));
    EXPECT_EQ(ret, true);
}

TEST(CyclicBuffer, write)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    size_t write_size = myrand(size);
    char *p = new char[write_size];
    size_t result = cb.write(p, write_size);
    delete[] p;
    EXPECT_EQ(result, write_size);
}

TEST(CyclicBuffer, write2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char *p = new char[size];
    cb.write(p, size);
    delete[] p;
    EXPECT_EQ(0, cb.write_avail());
}
TEST(CyclicBuffer, read)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    char out[128] = {0};
    cb.read(out, len);
    EXPECT_EQ(0, strcmp(msg, out));
}

TEST(CyclicBuffer, drop)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    char tmp[5] = {0};
    cb.read(tmp, 5);
    cb.drop(5);
    char out[128] = {0};
    cb.read(out, 4);
    EXPECT_EQ(0, strcmp("read", out));
}

TEST(CyclicBuffer, drop2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    size_t read_size = myrand(len);
    char out[128] = {0};
    cb.read(out, read_size);
    cb.drop();
    EXPECT_EQ(cb.read_avail(), len - read_size);
}

TEST(CyclicBuffer, drop3)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char* p = new char[size];
    cb.write(p, size); 
    char* out = new char[size];
    cb.read(out, size);
    cb.drop();
    delete[] p;
    delete[] out;
    EXPECT_EQ(cb.write_avail(), size - size / 3);
}

TEST(CyclicBuffer, get_read_offset)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    EXPECT_EQ(0, cb.get_read_offset());
}

TEST(CyclicBuffer, get_read_offset2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    size_t data_to_write = myrand(size);
    char *data = new char[data_to_write];
    cb.write(data, data_to_write);
    size_t drop_size = myrand(data_to_write);
    char* p = new char[drop_size];
    delete[] p;
    cb.read(p, drop_size);
    cb.drop(drop_size);
    delete[] data;
    EXPECT_EQ(drop_size, cb.get_read_offset());
}

TEST(CyclicBuffer, get_write_offset)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    size_t data_to_write = myrand(size);
    char *data = new char[data_to_write];
    int write_ret = cb.write(data, data_to_write);
    delete[] data;
    EXPECT_EQ(write_ret, cb.get_write_offset());
}

TEST(CyclicBuffer, buffer_size)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    EXPECT_EQ(size, cb.buffer_size());
}

TEST(CyclicBuffer, write_avail)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    EXPECT_EQ(cb.write_avail(), size);
}

TEST(CyclicBuffer, write_avail2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char *p = new char[write_size];
    int write_ret = cb.write(p, write_size);
    delete[] p;
    EXPECT_EQ(cb.write_avail(), size - write_ret);
}

TEST(CyclicBuffer, read_avail)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char* p = new char[write_size];
    int write_ret = cb.write(p, write_size);
    delete[] p;
    EXPECT_EQ(cb.read_avail(), write_ret);
}

TEST(CyclicBuffer, destory)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    cb.destory();
    EXPECT_EQ(1, 1);
}

TEST(CyclicBuffer, reset)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    cb.write("test", 4);
    cb.reset();
    EXPECT_EQ(cb.write_avail(), size);
}

TEST(CyclicBuffer, is_full)
{
    size_t size = myrand(100000);
    CyclicBuffer cb(size);
    EXPECT_EQ(cb.is_full(), false);
}

TEST(CyclicBuffer, is_full2)
{
    size_t size = myrand(100000);
    CyclicBuffer cb(size);
    char *p = new char[size];
    cb.write(p, size);
    delete[] p;
    EXPECT_EQ(cb.is_full(), true);
}

TEST(CyclicBuffer, set_offset)
{
    //向后set到有效位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char *p = new char[write_size];
    cb.write(p, write_size);
    int offset = myrand(write_size);
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(offset, cb.get_read_offset());
}

TEST(CyclicBuffer, set_offset2)
{
    //向后set到有效位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char *p = new char[write_size];
    cb.write(p, write_size);
    size_t read_size = cb.read_avail();
    int offset = myrand(write_size);
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(cb.read_avail(), read_size - offset);
}

TEST(CyclicBuffer, set_offset3)
{
    //向后set到无效位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char *p = new char[write_size];
    cb.write(p, write_size);
    size_t read_size = cb.read_avail();
    int offset = read_size + 1;
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(cb.read_avail(), 0);
}

TEST(CyclicBuffer, set_offset4)
{
    //向前set到有效位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    int write_size = myrand(size);
    char *p = new char[write_size];
    cb.write(p, write_size);
    size_t read_size = cb.read_avail();
    delete[] p;
    char c;
    cb.read(&c, 1);
    cb.drop(1);
    int read_offset = cb.get_read_offset();
    assert(read_offset==1);
    int offset = 0;
    cb.set_read_offset(offset);
    EXPECT_EQ(cb.read_avail(), read_size);
}

TEST(CyclicBuffer, set_offset5)
{
    //向前set到无效位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char *p = new char[size];
    cb.write(p, size);
    cb.read(p, size);
    cb.drop();
    int write_ret = cb.write(p, size);
    int offset = 0;
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(cb.read_avail(), 0);
}

TEST(CyclicBuffer, test_file)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    std::string out_name = "out.mp4";
    ifstream in("record.mp4", ios::in | ios::binary | ios::ate);
    ofstream out(out_name.c_str(), ios::out | ios::binary | ios::ate);
    long file_size = in.tellg();
    in.seekg(0, ios::beg);
    char *read_buff = new char[1024]; 
    char *write_buff = new char[1024]; 
    long read_total = 0, write_total = 0;
    //bool read_fin = false;
    bool write_fin = false;
    while (!write_fin) {
        size_t read_size = myrand(1024);    
        read_size = std::min(read_size, cb.write_avail());
        read_size = std::min(read_size, (size_t)(file_size - read_total));
        if (read_size > 0) {
            in.read(read_buff, read_size);
            assert(in);
            cb.write(read_buff, read_size);
            read_total += read_size;
        }
        //read_fin = (file_size == read_total);

        size_t write_size = myrand(1024);
        write_size = std::min(write_size, cb.read_avail());
        write_size = std::min(write_size, (size_t)(file_size - write_total));
        if (write_size > 0) {
            cb.read(write_buff, write_size); 
            cb.drop(write_size);
            out.write(write_buff, write_size);
            assert(out);
            write_total += write_size;
        }
        write_fin = (file_size == write_total);
    }
    //printf("file size:%d, write:%d, read:%d\n", file_size, write_total, read_total);
    //assert(file_size==write_total && file_size==read_total);
    in.close();
    out.close();
    char msg[128] = {0};
    sprintf(msg, "diff record.mp4 %s", out_name.c_str());
    int value = system(msg);
    //remove(out_name.c_str());
    EXPECT_EQ(value, 0);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
