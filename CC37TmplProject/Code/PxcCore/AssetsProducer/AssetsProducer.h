#pragma once
#include "SpriteLine.h"
#include "AnimateLine.h"
#include "AudioLine.h"
#include "SpriteProduct.h"
#include "AnimateProduct.h"
#include "AudioProduct.h"
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
	void TryResume();

private:
	PxcUtil::CThread* m_pThread;
	CAssetsProducer* m_pProducer;
	bool m_bRunning, m_bSuspend;
	PxcUtil::Lock m_lock;
};

class CAssetsProducer : public PxcUtil::Singleton<CAssetsProducer>
{
DECLARE_SINGLETON_CONSTRUCTOR(CAssetsProducer)
public:
	~CAssetsProducer();

	bool Init(std::string strPath);
	void Release();

	CSpriteLine& SpriteLine();
	CAnimateLine& AnimateLine();
	CAudioLine& AudioLine();

	void Update(float dt);
	bool RunLoadProducts();
	void OnLoadingThreadEnd();

private:
	CSpriteLine m_SpriteLine;
	CAnimateLine m_AnimateLine;
	CAudioLine m_AudioLine;

	CLoadAssetsThread* m_pLoadingThread;
};