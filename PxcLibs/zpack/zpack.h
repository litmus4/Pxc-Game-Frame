#ifndef __ZPACK_H__
#define __ZPACK_H__

#include <string>

#if defined (_MSC_VER) && defined (UNICODE)
	#define ZP_USE_WCHAR
#endif

//#if defined (_MSC_VER)
#define ZP_CASE_SENSITIVE	0
//#else
//	#define ZP_CASE_SENSITIVE	1
//#endif

namespace zp
{
#if defined (ZP_USE_WCHAR)
	typedef wchar_t Char;
	#ifndef _T
		#define _T(str) L##str
	#endif
	typedef std::wstring String;
	#define Fopen _wfopen
	#define Strcpy wcscpy_s
#else
	typedef char Char;
	#ifndef _T
		#define _T(str) str
	#endif
	typedef std::string String;
	#define Fopen fopen
	#define Strcpy strcpy_s
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

const u32 MAX_FILENAME_LEN = 1024;

const u32 OPEN_READONLY = 1;
const u32 OPEN_NO_FILENAME = 2;

const u32 PACK_UNICODE = 1;

const u32 FILE_DELETE = (1<<0);
const u32 FILE_COMPRESS = (1<<1);
//const u32 FILE_WRITING = (1<<2);

const u32 FILE_FLAG_USER0 = (1<<10);
const u32 FILE_FLAG_USER1 = (1<<11);

typedef bool (*Callback)(const Char* path, zp::u32 fileSize, void* param);

class IReadFile;
class IWriteFile;

///////////////////////////////////////////////////////////////////////////////////////////////////
class IPackage
{
public:
	virtual bool readonly() const = 0;

	virtual const Char* packageFilename() const = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//readonly functions, not available when package is dirty
	//IFile will become unavailable after package is modified

	virtual bool hasFile(const Char* filename) const = 0;
	virtual IReadFile* openFile(const Char* filename) = 0;
	virtual void closeFile(IReadFile* file) = 0;

	virtual u32 getFileCount() const = 0;
	virtual bool getFileInfo(u32 index, Char* filenameBuffer, u32 filenameBufferSize, u32* fileSize = 0,
							u32* packSize = 0, u32* flag = 0, u32* availableSize = 0, u64* contentHash = 0) const = 0;
	virtual bool getFileInfo(const Char* filename, u32* fileSize = 0, u32* packSize = 0,
							u32* flag = 0, u32* availableSize = 0, u64* contentHash = 0) const = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//package manipulation fuctions, not available in read only mode

	//do not add same file more than once between flush() call
	//outFileSize	origin file size
	//outPackSize	size in package
	virtual bool addFile(const Char* filename, const Char* externalFilename, u32 fileSize, u32 flag,
						u32* outPackSize = 0, u32* outFlag = 0, u32 chunkSize = 0) = 0;

	virtual IWriteFile* createFile(const Char* filename, u32 fileSize, u32 packSize,
									u32 chunkSize = 0, u32 flag = 0, u64 contentHash = 0) = 0;
	virtual IWriteFile* openFileToWrite(const Char* filename) = 0;
	virtual void closeFile(IWriteFile* file) = 0;

	//can not remove files added after last flush() call
	virtual bool removeFile(const Char* filename) = 0;

	//return true if there's any unsaved change of package
	virtual bool dirty() const = 0;

	//package file won't change before calling this function
	virtual void flush() = 0;

	virtual bool defrag(Callback callback, void* callbackParam) = 0;	//can be very slow, don't call this all the time

	virtual u32 getFileUserDataSize() const = 0;

	virtual bool writeFileUserData(const Char* filename, const u8* data, u32 dataLen) = 0;
	virtual bool readFileUserData(const Char* filename, u8* data, u32 dataLen) = 0;

protected:
	virtual ~IPackage(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class IReadFile
{
public:
	virtual u32 size() const = 0;

	virtual u32 availableSize() const = 0;

	virtual u32 flag() const = 0;

	virtual void seek(u32 pos) = 0;

	virtual u32 tell() const = 0;

	virtual u32 read(u8* buffer, u32 size) = 0;

protected:
	virtual ~IReadFile(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class IWriteFile
{
public:
	virtual u32 size() const = 0;

	virtual u32 flag() const = 0;

	virtual void seek(u32 pos) = 0;

	virtual u32 tell() const = 0;

	virtual u32 write(const u8* buffer, u32 size) = 0;

protected:
	virtual ~IWriteFile(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
IPackage* create(const Char* filename, u32 chunkSize = 0x40000, u32 fileUserDataSize = 0);
IPackage* open(const Char* filename, u32 flag = OPEN_READONLY | OPEN_NO_FILENAME);
void close(IPackage* package);

}

#endif
