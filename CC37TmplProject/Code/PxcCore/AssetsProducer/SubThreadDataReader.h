#pragma once
#include "cocos2d.h"

class CSubThreadDataReader
{
public:
	CSubThreadDataReader();
	~CSubThreadDataReader();

	void LoadImage(const std::string& strFileName);//Sub
	bool InitAndAddTexture();//Main

	static cocos2d::Sprite* CreateSprite();

	void LoadPlist(const std::string& strFileName);//Sub
	void AddSpriteFrames();//Main

	//FLAG
};