#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CocosGif/Movie.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    virtual void update(float delta);

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene *createScene();

    // a selector callback
    void menuCloseCallback(Ref *pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

    Movie *movie;

};

#endif // __HELLOWORLD_SCENE_H__
