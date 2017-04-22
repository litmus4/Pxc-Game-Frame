#include "AssetsProducer.h"
#include "PxcUtil/zPackEx.h"

CLoadAssetsThread::CLoadAssetsThread(CAssetsProducer* pProducer)
{
	m_pThread = NULL;
	m_pProducer = pProducer;
	m_bRunning = false;
	m_bSuspend = false;
}

CLoadAssetsThread::~CLoadAssetsThread()
{
	SAFE_DELETE(m_pThread)
}

void CLoadAssetsThread::Run()
{
	if (m_pProducer)
	{
		while (m_bRunning)
		{
			bool bOk = m_pProducer->RunLoadProducts();
			if (!bOk && m_pThread && !m_bSuspend)
			{
				{	CRI_SEC(m_lock)
					m_bSuspend = true;
				}
				m_pThread->Suspend();
			}
		}
		m_pProducer->OnLoadingThreadEnd();
	}
}

bool CLoadAssetsThread::Start()
{
	if (!m_pThread)
		m_pThread = new PxcUtil::CThread(this);
	m_bRunning = m_pThread->Start();
	return m_bRunning;
}

void CLoadAssetsThread::Stop()
{
	m_bRunning = false;
}

void CLoadAssetsThread::TryResume()
{
	CRI_SEC(m_lock)
	if (m_bRunning && m_pThread && m_bSuspend)
	{
		m_bSuspend = false;
		m_pThread->Resume();
	}
}

CAssetsProducer::CAssetsProducer()
{
	m_pLoadingThread = new CLoadAssetsThread(this);
}

CAssetsProducer::~CAssetsProducer()
{
	delete m_pLoadingThread;
}

bool CAssetsProducer::Init(std::string strPath)
{
	PxcUtil::zPackCombinePath(strPath);

	if (!m_SpriteLine.Init((strPath + "\\Sprites\\SpriteList.csv").c_str()))
		return false;

	if (!m_AnimateLine.Init((strPath + "\\Animates\\AnimateList.csv").c_str()))
		return false;

	if (!m_AudioLine.Init((strPath + "\\Audios\\AudioList.csv").c_str()))
		return false;

	return m_pLoadingThread->Start();
}

void CAssetsProducer::Release()
{
	m_SpriteLine.DiscardLoadingDraw();
	m_pLoadingThread->TryResume();
	m_pLoadingThread->Stop();
}

CSpriteLine& CAssetsProducer::SpriteLine()
{
	return m_SpriteLine;
}

CAnimateLine& CAssetsProducer::AnimateLine()
{
	return m_AnimateLine;
}

CAudioLine& CAssetsProducer::AudioLine()
{
	return m_AudioLine;
}

void CAssetsProducer::Update(float dt)
{
	if (!m_SpriteLine.IsLoadQueueEmpty() ||
		!m_AnimateLine.IsLoadQueueEmpty() ||
		!m_AudioLine.IsLoadQueueEmpty())
		m_pLoadingThread->TryResume();

	m_SpriteLine.Update(dt);
	m_AnimateLine.Update(dt);
	m_AudioLine.Update(dt);
}

bool CAssetsProducer::RunLoadProducts()
{
	if (m_SpriteLine.IsLoadQueueEmpty() &&
		m_AnimateLine.IsLoadQueueEmpty() &&
		m_AudioLine.IsLoadQueueEmpty())
		return false;

	m_SpriteLine.RunLoadProducts();
	m_AnimateLine.RunLoadProducts();
	m_AudioLine.RunLoadProducts();
	return true;
}

void CAssetsProducer::OnLoadingThreadEnd()
{
	m_SpriteLine.Release();
	m_AnimateLine.Release();
	m_AudioLine.Release();
	DeleteInstance();
}