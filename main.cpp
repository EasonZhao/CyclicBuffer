#include <iostream>
#include <gtest/gtest.h>
#include "CyclicBuffer.h"
#include "CyclicBufferSection.h"
#include <stdio.h>
#include <algorithm>
#include <fstream>

using namespace std;

int myrand(int const &max)
{
    return rand() % max + 1;
}

TEST(CyclicBuffer, create)
{
    size_t size = 8*1024*1024;
    CyclicBuffer buff(size);
    EXPECT_EQ(size, buff.capacity());
}

/*
TEST(CyclicBuffer, create2)
{
    size_t size = 8*1024*1024;
    size = myrand(size);
    CyclicBuffer cb;
    size_t result = cb.create(size);
    bool ret = ((result==size) && (cb.capacity()==size));
    EXPECT_EQ(ret, true);
}
*/

TEST(CyclicBuffer, create3)
{
    CyclicBuffer cp(0);
    EXPECT_EQ(1, cp.capacity());
}

TEST(CyclicBuffer, write)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    size_t write_size = myrand(size);
    char *p = new char[write_size];
    size_t result = cb.write((const char*)p, write_size);
    delete[] p;
    EXPECT_EQ(result, write_size);
}

TEST(CyclicBuffer, write2)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char *p = new char[size];
    cb.write((const char*)p, size);
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

TEST(CyclicBuffer, read_pos)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    //char out[128] = {0};
    std::vector<boost::asio::const_buffer> vb;
    cb.read(vb, cb.read_avail());
    EXPECT_EQ(0, strcmp(msg, boost::asio::buffer_cast<const char*>(vb[0])));
}

TEST(CyclicBuffer, read_pos1)
{
    size_t size = 1000;
    CyclicBuffer cb(size);
    char data[995] = {0};
    cb.write(data, 995);
    cb.read(data, 995);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    //char out[128] = {0};
    std::vector<boost::asio::const_buffer> vb;
    cb.read(vb, cb.read_avail());
    assert(vb.size() == 2);
    EXPECT_EQ(0, strcmp("read", boost::asio::buffer_cast<const char*>(vb[1])));
}

TEST(CyclicBuffer, read_pos2)
{
    size_t size = 1000;
    CyclicBuffer cb(size);
    char data[1000] = {0};
    cb.write(data, 1000);
    cb.read(data, 500);
    cb.read(data, 500);
    char msg[] = "test read";
    size_t len = sizeof(msg)/sizeof(msg[0]);
    cb.write(msg, len); 
    std::vector<boost::asio::const_buffer> vb;
    cb.read(vb, cb.read_avail());
    EXPECT_EQ(1, vb.size());
}

TEST(CyclicBuffer, read_pos3)
{
    size_t size = 1000;
    CyclicBuffer cb(size);
    char data[] = "test read";
    cb.write(data, sizeof(data) / sizeof(data[0]));
    std::vector<boost::asio::const_buffer> vb;
    cb.read(vb, 4);
    string ret(boost::asio::buffer_cast<const char*>(vb[0]), 
                boost::asio::buffer_size(vb[0]));
    EXPECT_EQ(true, ret=="test");
}

TEST(CyclicBuffer, read_pos4)
{
    size_t size = 1000;
    CyclicBuffer cb(size);
    char data[] = "testread";
    cb.write(data, sizeof(data) / sizeof(data[0]));
    std::vector<boost::asio::const_buffer> vb;
    cb.read(vb, 4);
    cb.drop();
    cb.read(vb, 4);
    string ret(boost::asio::buffer_cast<const char*>(vb[0]), 
                boost::asio::buffer_size(vb[0]));
    cout << ret << endl;
    EXPECT_EQ(true, ret=="read");
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
    //cb.drop(5);
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
    //cb.drop();
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
    //cb.drop();
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
    //cb.drop(drop_size);
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

TEST(CyclicBuffer, capacity)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    EXPECT_EQ(size, cb.capacity());
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
/*
TEST(CyclicBuffer, destory)
{
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    cb.destory();
    EXPECT_EQ(1, 1);
}
*/

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
    //cb.drop(1);
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
    //cb.drop();
    size = cb.write_avail();
    int write_ret = cb.write(p, size);
    int offset = 0;
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(cb.read_avail(), 0);
}

/*
TEST(CyclicBuffer, set_offset6)
{
    //向前set到最后一个位置
    size_t size = 8*1024*1024;
    CyclicBuffer cb(size);
    char *p = new char[size];
    cb.write(p, size);
    cb.read(p, size);
    //cb.drop();
    size = cb.write_avail();
    int write_ret = cb.write(p, size);
    long offset = cb.get_write_offset();
    long tmp = cb.write_avail();
    cb.set_read_offset(offset);
    delete[] p;
    EXPECT_EQ(cb.write_avail(), tmp);
}
*/

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
    bool write_fin = false;
    while (!write_fin) {
        size_t read_size = myrand(1024);    
        read_size = std::min(read_size, cb.write_avail());
        read_size = std::min(read_size, (size_t)(file_size - read_total));
        if (read_size > 0) {
            in.read(read_buff, read_size);
            //printf("read size:%d\n", read_size);
            assert(in);
            int ret = cb.write(read_buff, read_size);
            assert(ret == read_size);
            read_total += read_size;
        }
        //read_fin = (file_size == read_total);

        size_t write_size = myrand(1024);
        write_size = std::min(write_size, cb.read_avail());
        write_size = std::min(write_size, (size_t)(file_size - write_total));
        if (write_size > 0) {
            int ret = cb.read(write_buff, write_size); 
            assert(ret == write_size);
            //cb.drop(write_size);
            out.write(write_buff, write_size);
            //printf("write size:%d\n", write_size);
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
    remove(out_name.c_str());
    EXPECT_EQ(value, 0);
}

TEST(CyclicBufferSection, construct)
{
    size_t size = myrand(8*1024*1024);
    char *p = new char[size];
    CyclicBufferSection cp(p, size);
    delete[] p;
    EXPECT_EQ(cp.capacity(), size);
}

TEST(CyclicBufferSection, write)
{
    char msg[11] = {0};
    char *p = msg;
    size_t len = 11;
    CyclicBufferSection cp(p, len);
    cp.write((const char*)"test write", 11);
    EXPECT_EQ(0, strcmp(p, "test write"));
}

TEST(CyclicBufferSection, read)
{
    char msg[] = "test read";
    size_t size = sizeof(msg) / sizeof(msg[0]);
    char *p = msg;
    CyclicBufferSection cp(p, size);
    char out[11] = {0};
    cp.read(out, 4);
    EXPECT_EQ(0, strcmp("test", out));
}

/*
   TEST(CyclicBufferSection, avail_size)
   {
   char msg[] = "test read";
   size_t size = sizeof(msg) / sizeof(msg[0]);
   char *p = msg;
   char *p1 = msg;
   char *p2 = p + 5;
   CyclicBufferSection cp(p, size, p1, p2);
   EXPECT_EQ(5, cp.avail_size());
   }
 */

/*
   TEST(CyclicBufferSection, avail_size2)
   {
   char msg[] = "test read";
   size_t size = sizeof(msg) / sizeof(msg[0]);
   char *p = msg;
   char *p1 = msg;
   char *p2 = p;
   CyclicBufferSection cp(p, size);
   assert(cp.avail_size() == 0);
   int tmp = myrand(size);
   p2 += tmp;
   EXPECT_EQ(tmp, cp.avail_size());
   }
 */

TEST(CyclicBufferSection, operator1)
{
    char msg[] = "test read";
    size_t size = sizeof(msg) / sizeof(msg[0]);
    char *p = msg;
    CyclicBufferSection cp(p, size);
    cp += 5;
    assert(cp.offset() == 5);
    char out[10] = {0};
    cp.read(out, 10);
    EXPECT_EQ(strcmp(out, "read"), 0);
}

TEST(CyclicBufferSection, operator2)
{
    char msg[] = "test read";
    size_t size = sizeof(msg) / sizeof(msg[0]);
    char *p = msg;
    CyclicBufferSection cp(p, size);
    cp += 0;
    EXPECT_EQ(0, cp.offset());
}

TEST(CyclicBufferSection, operator3)
{
    char msg[] = "test read";
    size_t size = sizeof(msg) / sizeof(msg[0]);
    char *p = msg;
    CyclicBufferSection cp(p, size, 1);
    cp -= 1;
    EXPECT_EQ(0, cp.offset());
}

TEST(CyclicBuffer, resize)
{
    CyclicBuffer cp(0);
    size_t size = 1024*1024*8;
    size = myrand(size); 
    cp.resize(size);
    EXPECT_EQ(size, cp.capacity());
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
