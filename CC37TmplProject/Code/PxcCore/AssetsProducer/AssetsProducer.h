#pragma once
#include "SpriteLine.h"
#include "AnimateLine.h"
#include "PxcUtil/Singleton.h"
#include "PxcUtil/Thread.h"

class CAssetsProducer;
class CLoadAssetsThread : public PxcUtil::Runnable
{
public:
	CLoadAssetsThread(CAssetsProducer* pProducer);
	~CLoadAssetsThread();

	virtual void Run();
	bool Start();
	void Stop();

private:
	PxcUtil::CThread* m_pThread;
	CAssetsProducer* m_pProducer;
	bool m_bRunning;
};

class CAssetsProducer : public PxcUtil::Singleton<CAssetsProducer>
{
public:
	CAssetsProducer();
	~CAssetsProducer();

	bool Init(std::string strPath);
	void Release();

	CSpriteLine& SpriteLine();
	CAnimateLine& AnimateLine();

	void Update(float dt);
	void RunLoadProducts();
	void OnLoadingThreadEnd();

private:
	CSpriteLine m_SpriteLine;
	CAnimateLine m_AnimateLine;

	CLoadAssetsThread* m_pLoadingThread;
};