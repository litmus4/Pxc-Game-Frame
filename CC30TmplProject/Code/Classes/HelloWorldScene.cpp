#include "HelloWorldScene.h"
//*²âÊÔÁÙÊ±
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "tinyxml/tinyxml.h"
#include "PxcUtil/StringTools.h"
#include "PxcUtil/zPackEx.h"
//*/

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.tga",
                                           "CloseSelected.tga",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
	//*²âÊÔÁÙÊ±
	PXCU_LOGINST->Init(SpecialFileDef::ELogFile_AssetsTables, "log_assetstables.txt");
	CTextTableCenter::GetInstance()->Init();
	std::string strText5 = "Hello World ";
	strText5 += CTextTableCenter::GetInstance()->GetText(5);

	PxcUtil::CCSVTableOperator tabop;
	if (tabop.Load("testzpk.zpk#dir\\test2.csv"))
	{
		tabop.ReadRow();
		std::string strName;
		if (tabop.GetValue("Name", strName))
			strText5 += strName;
	}
	TiXmlDocument doc;
	if (doc.LoadFile("testzpk.zpk#dir\\testxml.xml"))
	{
		TiXmlElement* pRoot = doc.RootElement();
		if (pRoot)
		{
			int iValue = 0;
			TiXmlElement* pItem = pRoot->FirstChildElement("ItemInt");
			if (pItem)
			{
				pItem->QueryIntAttribute("Value", &iValue);
				strText5 += PxcUtil::StringTools::BasicToStr(iValue);
			}
		}
	}
	PxcUtil::zPackRelease();
	//*/
    auto label = LabelTTF::create(strText5, "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.tga");

    // position the sprite on the center of the screen
    sprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

	//*²âÊÔÁÙÊ±
	CTextTableCenter::GetInstance()->Release();
	PXCU_LOGINST->Release();
	//*/
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
