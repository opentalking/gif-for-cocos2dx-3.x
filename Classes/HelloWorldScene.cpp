#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Gif/GIFMovie.h"
#include "Gif/CacheGif.h"
#include "Gif/CacheGifData.h"
#include "Gif/InstantGif.h"
#include "Gif/InstantGifData.h"
#include "GridItemPos.h"

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
	if (!CCLayer::init())
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
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this)
	);

	pCloseItem->setPosition(Point(origin.x + visibleSize.width - pCloseItem->getContentSize().width / 2,
		origin.y + pCloseItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	Menu* pMenu = Menu::create(pCloseItem, NULL);
	pMenu->setPosition(Point(0, 0));
	this->addChild(pMenu, 1);

	CreateGif();

	return true;
}

void HelloWorld::update(float delta)
{
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();
}

void HelloWorld::CreateGif(void)
{
	std::string name = FileUtils::getInstance()->fullPathForFilename( "g1.gif" );
	GifBase* gif = InstantGif::create( name.c_str() );
	gif->setPosition(320, 480);
	addChild(gif);

	gif = CacheGif::create(name.c_str());
	gif->setPosition(320, 160);
	addChild(gif);

	gif = InstantGifData::create( "g1.gif" );
	gif->setPosition( 960, 480);
	addChild(gif);

	gif = CacheGifData::create("g1.gif");
	gif->setPosition(960, 160);
	addChild(gif);
	
	GIFMovieData* movie = new GIFMovieData;
	movie->init("g1.gif");
	Sprite* pImg = Sprite::create();
	pImg->initWithTexture(movie->GetTexture( 0 ));
	pImg->setPosition(428, 320);
	addChild(pImg);
	delete movie;

	pImg = Sprite::create();
	pImg->initWithTexture(GIFMovieData::StaticGetTexture("g1.gif", 0));
	pImg->setPosition( 856, 320);
	addChild(pImg);
}