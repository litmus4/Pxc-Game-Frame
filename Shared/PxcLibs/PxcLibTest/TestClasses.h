#include "PxcUtil/Thread.h"

class CSubRun : public PxcUtil::Runnable
{
public:
	CSubRun() {}
	virtual ~CSubRun() {}
	virtual void Run();
};