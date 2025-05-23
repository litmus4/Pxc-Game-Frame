#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

class CSpriteProduct;
class CAudioProduct;
class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	virtual void update(float dt);

	void menuPlayDebuffCallback(cocos2d::Ref* pSender, bool bForceStop);
	void partMoveCompleteCallback(cocostudio::Armature* pArmature, const std::string& strBoneName,
					cocostudio::MovementEventType eEventType, const std::string& strAnimName);

private:
	CSpriteProduct* m_pBigProd;
	bool m_bBigProdAdded;
	CSpriteProduct* m_pSmallProd;
	bool m_bSmallProdAdded;
	CAudioProduct* m_pDiscoProd;
	CAudioProduct* m_pDebuffProd;
	cocostudio::Armature* m_pPartArm;
	float m_fPartArmTime;
};

#endif // __HELLOWORLD_SCENE_H__
