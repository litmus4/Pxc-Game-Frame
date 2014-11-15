#include "PxcUtil/Thread.h"
#include "PxcUtil/Delegate.h"

typedef PxcUtil::CDelegate<bool, int> tDaliGate;

class CSubRun : public PxcUtil::Runnable
{
public:
	CSubRun() {}
	virtual ~CSubRun() {}
	virtual void Run();
};

class CDali
{
public:
	CDali();
	~CDali();
	bool DynFunc(int i);
	static bool StaFunc(int i);
};