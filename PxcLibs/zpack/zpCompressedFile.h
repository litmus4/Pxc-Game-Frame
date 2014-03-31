#ifndef __ZP_COMPRESSED_FILE_H__
#define __ZP_COMPRESSED_FILE_H__

#include "zpack.h"

namespace zp
{

class Package;

class CompressedFile : public IReadFile
{
public:
	CompressedFile(const Package* package, u64 offset, u32 compressedSize, u32 originSize,
					u32 chunkSize, u32 flag, u64 nameHash);
	virtual ~CompressedFile();

	//from IFiled
	virtual u32 size() const;

	virtual u32 availableSize() const;

	virtual u32 flag() const;

	virtual void seek(u32 pos);

	virtual u32 tell() const;

	virtual u32 read(u8* buffer, u32 size);

private:
	bool checkChunkPos() const;

	void seekInPackage(u32 offset);

	u32 oneChunkRead(u8* buffer, u32 size);

	bool readChunk(u32 chunkIndex, u32 offset, u32 readSize, u8* buffer);

private:
	u64				m_offset;
	u64				m_nameHash;
	const Package*	m_package;
	u32				m_chunkSize;
	u32				m_flag;
	u32				m_compressedSize;
	u32				m_originSize;

	u32				m_readPos;
	u32				m_chunkCount;
	u32*			m_chunkPos;
	u8*				m_fileData;		//available when there's only 1 chunk
	u8**			m_chunkData;	//available when there's more than 1 chunk
};

}

#endif
