#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
//*²âÊÔÁÙÊ±
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "AssetsProducer/AssetsProducer.h"
#include "PxcUtil/zPackEx.h"
#include "PxcUtil/StringTools.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define USE_ZPACK 2
#endif
//*/

USING_NS_CC;
using namespace cocostudio;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
	//*²âÊÔÁÙÊ±
	PXCU_LOGINST->Init(SpecialFileDef::ELogFile_AssetsTables, "log_assetstables.txt");
#ifdef USE_ZPACK
#if USE_ZPACK == 2
	PxcUtil::zPackPathSwitch(true, false);
#else
	PxcUtil::zPackPathSwitch(true);
#endif
	PxcUtil::zPackAddPathAim("Packs\\DataTables.zpk", "DataTables");
	PxcUtil::zPackAddPathAim("Packs\\Assets.zpk", "Assets");
#else
	PxcUtil::zPackAddPathAim("ms-appx:///Assets\\Resources", "DataTables");
	PxcUtil::zPackAddPathAim("ms-appx:///Assets\\Resources", "Assets");
#endif
	CTextTableCenter::GetInstance()->Init(GlobalDef::ELanguage_ZH_CN);
	COtherTableCenter::GetInstance()->Init();
	CAssetsProducer::GetInstance()->Init("Assets", GlobalDef::ELanguage_ZH_CN);
	PxcUtil::zPackRelease();

	std::string strText5 = "Hello World ";
	strText5 += CTextTableCenter::GetInstance()->GetText(5);
	CGlobalParamRow* pParamRow = COtherTableCenter::GetInstance()->GetGlobalParamRow(4);
	if (pParamRow)
		strText5 += PxcUtil::StringTools::BasicToStr(pParamRow->GetLeave<LEAVECOL_I>("Int1"));
	pParamRow = COtherTableCenter::GetInstance()->GetGlobalParamRow(5);
	if (pParamRow)
	{
		std::vector<std::string>& vecValues = pParamRow->GetLeave<LEAVECOL_SA>("Descs");
		if (!vecValues.empty())
			strText5 += vecValues[0];
	}
	//*/
	auto label = Label::createWithTTF(strText5, "fonts/msyh.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

	//*²âÊÔÁÙÊ±
	scheduleUpdate();
	m_pBigProd = NULL;
	m_bBigProdAdded = false;
	m_pSmallProd = NULL;
	m_bSmallProdAdded = false;
	m_pDiscoProd = NULL;
	m_pDebuffProd = NULL;
	m_pPartArm = NULL;
	m_fPartArmTime = -1.0f;

	m_pBigProd = CAssetsProducer::GetInstance()->SpriteLine().Fetch(1, CBaseProduct::EClonedData, true);
	if (m_pBigProd)
	{
		Vec4 v4Trans(origin.x + visibleSize.width / 2, origin.y + 200.0f, 0.0f, 1.0f);
		m_pBigProd->SetTransform(v4Trans);
	}
	m_pSmallProd = CAssetsProducer::GetInstance()->SpriteLine().Fetch(2, CBaseProduct::EClonedData, true);
	if (m_pSmallProd)
	{
		Vec4 v4Trans(origin.x + 50.0f, origin.y + visibleSize.height / 2, 0.0f, 1.0f);
		m_pSmallProd->SetTransform(v4Trans);
	}
	CAnimateProduct* pCountProd = CAssetsProducer::GetInstance()->AnimateLine().Fetch(0, CBaseProduct::EClonedData, true);
	if (pCountProd)
	{
		pCountProd->SetFps(1.0f);
		pCountProd->SetRestoredFinally(true);
		if (m_pSmallProd)
			m_pSmallProd->AddAnimate(pCountProd);
	}
	m_pDiscoProd = CAssetsProducer::GetInstance()->AudioLine().Fetch(0, CBaseProduct::ESharedData, true);
	if (m_pDiscoProd)
	{
		m_pDiscoProd->SetVolume(0.7f);
		m_pDiscoProd->Play(true);
	}
	m_pDebuffProd = CAssetsProducer::GetInstance()->AudioLine().Fetch(1, CBaseProduct::ESharedData, true);
	if (m_pDebuffProd)
	{
		float fRightWidth = closeItem->getContentSize().width;

		auto pPlayItemForce = MenuItemImage::create("CloseSelected.png", "CloseNormal.png",
			CC_CALLBACK_1(HelloWorld::menuPlayDebuffCallback, this, true));
		pPlayItemForce->setPosition(Vec2(
			origin.x + visibleSize.width - fRightWidth - pPlayItemForce->getContentSize().width / 2,
			origin.y + pPlayItemForce->getContentSize().height / 2));
		menu->addChild(pPlayItemForce);
		fRightWidth += pPlayItemForce->getContentSize().width;

		auto pPlayItemQueue = MenuItemImage::create("CloseSelected.png", "CloseNormal.png",
			CC_CALLBACK_1(HelloWorld::menuPlayDebuffCallback, this, false));
		pPlayItemQueue->setPosition(Vec2(
			origin.x + visibleSize.width - fRightWidth - pPlayItemQueue->getContentSize().width / 2,
			origin.y + pPlayItemQueue->getContentSize().height / 2));
		menu->addChild(pPlayItemQueue);
	}
	std::string strPartImage = "Assets\\Armatures\\arm_part\\arm_part.png";
	PxcUtil::zPackCombinePath(strPartImage);
	std::string strPartPlist = "Assets\\Armatures\\arm_part\\arm_part.plist";
	PxcUtil::zPackCombinePath(strPartPlist);
	std::string strPartJson = "Assets\\Armatures\\arm_part\\arm_part.ExportJson";
	PxcUtil::zPackCombinePath(strPartJson);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(strPartImage, strPartPlist, strPartJson);
	m_pPartArm = Armature::create("NewAnimation1");
	if (m_pPartArm)
	{
		m_pPartArm->setPosition(Vec2(origin.x + visibleSize.width - 120.0f, origin.y + visibleSize.height / 2));
		this->addChild(m_pPartArm);

		m_pPartArm->getAnimation()->registerPartEventType(COMPLETE);
		m_pPartArm->getAnimation()->setPartMovementEventCallFunc(this,
			(SEL_PartMovementEventCallFunc)&HelloWorld::partMoveCompleteCallback);
		m_pPartArm->getAnimation()->playPart("up_small", "Layer2");
		m_fPartArmTime = 0.0f;
	}
	//*/
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
	//*²âÊÔÁÙÊ±
	CAssetsProducer::GetInstance()->SpriteLine().Discard(m_pBigProd);
	CAnimateProduct* pCountProd = m_pSmallProd->GetAnimate("count");
	CAssetsProducer::GetInstance()->SpriteLine().Discard(m_pSmallProd);
	CAssetsProducer::GetInstance()->AnimateLine().Discard(pCountProd);
	CAssetsProducer::GetInstance()->AudioLine().Discard(m_pDiscoProd);
	CAssetsProducer::GetInstance()->AudioLine().Discard(m_pDebuffProd);

	CTextTableCenter::GetInstance()->Release();
	COtherTableCenter::GetInstance()->Release();
	CAssetsProducer::GetInstance()->Release();
	PxcUtil::zPackClearPathAims();
	PxcUtil::zPackRelease();
	PXCU_LOGINST->Release();
	//*/
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::update(float dt)
{
	CAssetsProducer::GetInstance()->Update(dt);

	if (m_pBigProd && m_pBigProd->IsComplete() && !m_bBigProdAdded)
	{
		this->addChild(m_pBigProd->GetSprite());
		m_bBigProdAdded = true;
	}
	if (m_pSmallProd && m_pSmallProd->IsComplete() && !m_bSmallProdAdded)
	{
		this->addChild(m_pSmallProd->GetSprite());
		m_pSmallProd->GetAnimate("count")->Play();
		m_bSmallProdAdded = true;
	}
	if (m_pPartArm && m_fPartArmTime >= -0.5f)
	{
		m_fPartArmTime += dt;
		if (m_fPartArmTime >= 1.0f)
		{
			m_pPartArm->getAnimation()->playPart("up_big", "Layer1", 30);
			m_pPartArm->getAnimation()->playPart("down_big", "Layer4");
			m_fPartArmTime = -1.0f;
		}
	}
}

void HelloWorld::menuPlayDebuffCallback(cocos2d::Ref* pSender, bool bForceStop)
{
	if (m_pDebuffProd)
	{
		m_pDebuffProd->Play(false, bForceStop);
		if (bForceStop)
			CAssetsProducer::GetInstance()->AudioLine().SetBgm(true, 1.0f);
		else
			CAssetsProducer::GetInstance()->AudioLine().SetBgm(true, 0.2f);
	}
}

void HelloWorld::partMoveCompleteCallback(Armature* pArmature, const std::string& strBoneName,
							MovementEventType eEventType, const std::string& strAnimName)
{
	if (m_pPartArm && strAnimName == "down_big")
	{
		m_pPartArm->getAnimation()->playPart("up_big", "Layer4", 30, -1, 30.0f);
	}
}