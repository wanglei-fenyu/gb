#pragma once 
#include <google/protobuf/io/zero_copy_stream.h>
#include <protobuf/meta.pb.h>
#include "common/common.h"
#include "../common_component/define.h"
namespace gb
{
class AbstractCompressedOutputStream : public google::protobuf::io::ZeroCopyOutputStream
{
public:
    virtual bool Flush() = 0;
    virtual bool Close() = 0;

};

class AbstractCompressedInputStream : public google::protobuf::io::ZeroCopyInputStream
{
public:
    //没有多余的数据需要压缩处理返回true
    virtual bool ExpectAtEnd() = 0;
};

//level 用于zlib
AbstractCompressedOutputStream * get_compressed_output_stream(google::protobuf::io::ZeroCopyOutputStream * ostream, CompressType type, int level=1);
AbstractCompressedInputStream * get_compressed_input_stream(google::protobuf::io::ZeroCopyInputStream * istream, CompressType type);
}
