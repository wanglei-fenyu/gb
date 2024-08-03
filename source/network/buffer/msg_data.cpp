#include "msg_data.h"
#include "compressed_stream.h"
namespace gb
{
	std::string GetMsgData(Meta& meta, ReadBufferPtr buffer,int meta_size,int64_t data_size)
	{
		std::string s = buffer->ToString();
		if (meta.compress_type() == CompressType::CompressTypeNone)
			s = s.substr(meta_size, data_size);
		else
		{
			google::protobuf::io::ArrayInputStream         i(s.data()+meta_size,data_size);
			std::shared_ptr<AbstractCompressedInputStream> is(get_compressed_input_stream(&i, meta.compress_type()));
			google::protobuf::io::CodedInputStream   c(is.get());
			uint32_t size;
			c.ReadVarint32(&size);
			std::string out_s;
			c.ReadString(&out_s, size);
			s = out_s;
		}
		return s;
	}

}

