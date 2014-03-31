#ifndef __ZP_WRITE_COMPRESS_FILE_H__
#define __ZP_WRITE_COMPRESS_FILE_H__

#include <vector>

namespace zp
{

u32 writeCompressFile(FILE* dstFile, u64 offset, FILE* srcFile, u32 srcFileSize, u32 chunkSize, u32& flag,
						std::vector<u8>& chunkData,	std::vector<u8>& compressBuffer, std::vector<u32>& chunkPosBuffer);

}

#endif
