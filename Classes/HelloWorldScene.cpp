#include "HelloWorldScene.h"
#include "Gif/GIFMovie.h"
#include "Gif/CacheGif.h"
#include "Gif/InstantGif.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define FILE_FORMAT ("/mnt/sdcard/g%d.gif") //On the Android platform, the resources are compressed in the asset directory. Therefore, the resources must be files on the sd card
#else
#define FILE_FORMAT ("g%d.gif")
#endif

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    Scene *scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

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
    if ( !CCLayer::init() )
    {
        return false;
    }
      
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
    MenuItemImage *pCloseItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback,this)
                      );

	pCloseItem->setPosition(Point(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
		origin.y + pCloseItem->getContentSize().height/2));

	// create menu, it's an autorelease object
	Menu* pMenu = Menu::create(pCloseItem, NULL);
	pMenu->setPosition(Point(0,0));
	this->addChild(pMenu, 1);

    return true;
}

int count = 1;
void HelloWorld::update(float delta)
{
	count++ ;
	if(count > 240)
	{
		this->removeAllChildren();
	}
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    count++;
	while(this->getChildByTag(1000))
	{
		this->removeChildByTag(1000);
	}
    log("%s","------after remove gif-----------");
    std::string info = Director::getInstance()->getTextureCache()->getCachedTextureInfo();
    log("%s",info.c_str());
	if(count % 2 != 0)
    {
        return ;
    }
	std::string name = CCString::createWithFormat(FILE_FORMAT,count/2)->getCString();
	name = FileUtils::getInstance()->fullPathForFilename(name.c_str());
    
	GifBase *gif = InstantGif::create(name.c_str());
	if(gif == NULL)
	{
        CCLOG("%s","create gif failed");
		return ;
	}
	gif->setAnchorPoint(Point(0,0));
	this->addChild(gif);
	gif->setPosition(Point(0,0));
	gif->setTag(1000);


	gif = CacheGif::create(name.c_str());
	gif->setAnchorPoint(Point(0,0));
	this->addChild(gif);
	gif->setPosition(Point(500,0));
	gif->setScale(2);
	gif->setTag(1000);
	log("%s","------after add gif-----------");
    info = Director::getInstance()->getTextureCache()->getCachedTextureInfo();
    log("%s",info.c_str());
	return ;
}
