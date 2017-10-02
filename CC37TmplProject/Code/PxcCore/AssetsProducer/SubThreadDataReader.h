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
	cocos2d::Sprite* CreateSprite();//Main ����InitAndAddTexture

	//SpriteFrames
	void LoadPlist(const std::string& strFileName);//Sub
	void AddSpriteFrames();//Main ����InitAndAddTexture

	//Animations
	void LoadAnimPlist(const std::string& strFileName);//Sub
	void AddAnimations();//Main

	//Armature
	void LoadArmConfig(const std::string& strFileName);//Sub
	void AddArmature();//Main

private:
	//Texture
	cocos2d::Image* m_pImage;
	std::string m_strFullPath;
	cocos2d::Texture2D* m_pTexture;

	//SpriteFrame
	std::string m_strsfFileName;
	cocos2d::ValueMap m_sfDict;

	//Animations
	std::string m_straniFileName;
	cocos2d::ValueMap m_aniDict;

	//Armature
	std::string m_strarmFileName;
};