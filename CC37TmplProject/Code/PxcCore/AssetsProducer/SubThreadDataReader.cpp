#include "SubThreadDataReader.h"

USING_NS_CC;

CSubThreadDataReader::CSubThreadDataReader()
{
	m_pImage = nullptr;
	m_pTexture = nullptr;
}

CSubThreadDataReader::~CSubThreadDataReader()
{
	//
}

void CSubThreadDataReader::LoadTexImage(const std::string& strFileName)
{
	m_strFullPath = FileUtils::getInstance()->fullPathForFilename(strFileName);

	m_pTexture = Director::getInstance()->getTextureCache()->getTextureForKey(m_strFullPath);
	if (m_pTexture)
		return;

	if (m_strFullPath.empty() || !FileUtils::getInstance()->isFileExist(m_strFullPath))
		return;

	m_pImage = new (std::nothrow) Image();
	if (m_pImage && !m_pImage->initWithImageFile(m_strFullPath))
		CC_SAFE_RELEASE(m_pImage);
}

bool CSubThreadDataReader::InitAndAddTexture()
{
	if (m_pTexture)
		return true;

	m_pTexture = Director::getInstance()->getTextureCache()->addImage(m_pImage, m_strFullPath, true);
	return (m_pTexture ? true : false);
}

Sprite* CSubThreadDataReader::CreateSprite()
{
	Sprite* pSprite = new (std::nothrow) Sprite;
	if (pSprite && InitAndAddTexture())
	{
		Rect rect = Rect::ZERO;
		rect.size = m_pTexture->getContentSize();
		if (pSprite->initWithTexture(m_pTexture, rect))
		{
			pSprite->autorelease();
			return pSprite;
		}
	}
	CC_SAFE_DELETE(pSprite);
	return nullptr;
}

void CSubThreadDataReader::LoadPlist(const std::string& strFileName)
{
	if (SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(strFileName))
		return;

	m_strsfFileName = strFileName;
	std::string strFullPath = FileUtils::getInstance()->fullPathForFilename(strFileName);
	m_sfDict = FileUtils::getInstance()->getValueMapFromFile(strFullPath);
}

void CSubThreadDataReader::AddSpriteFrames()
{
	if (m_strsfFileName.empty())
		return;

	if (InitAndAddTexture())
		SpriteFrameCache::getInstance()->addSpriteFramesDictFile(m_sfDict, m_pTexture, m_pImage, m_strsfFileName);
}