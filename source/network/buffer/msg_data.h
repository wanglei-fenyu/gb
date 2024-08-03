#pragma once

#include "protobuf/meta.pb.h"
#include "buffer.h"

namespace gb
{

	std::string GetMsgData(Meta& meta, ReadBufferPtr buffer, int meta_size, int64_t data_size);
}