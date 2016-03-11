#pragma once
#include "cocos2d.h"

class CSubThreadDataReader
{
public:
	CSubThreadDataReader();
	~CSubThreadDataReader();

	//Texture
	void LoadTexImage(const std::string& strFileName);//Sub
	bool InitAndAddTexture();//Main

	//Sprite
	cocos2d::Sprite* CreateSprite();//Main °üº¬InitAndAddTexture

	//SpriteFrames
	void LoadPlist(const std::string& strFileName);//Sub
	void AddSpriteFrames();//Main °üº¬InitAndAddTexture

private:
	//Texture
	cocos2d::Image* m_pImage;
	std::string m_strFullPath;
	cocos2d::Texture2D* m_pTexture;

	//SpriteFrame
	std::string m_strsfFileName;
	cocos2d::ValueMap m_sfDict;
};