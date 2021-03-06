#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "../GenxiumDownloader/GenxiumDownloader.h"
#include "../CCSpriteTouch/CCSpriteTouch.h"

class HelloWorld : public cocos2d::CCLayer
{
    
public:
    HelloWorld(){}
    ~HelloWorld(){}
    
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual void onEnter();
    virtual void onExit();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
private:
    void onSpriteTouchClicked(CCNode*, void*);
};

#endif // __HELLOWORLD_SCENE_H__
