#ifndef __CCMENUTOUCH_H__
#define __CCMENUTOUCH_H__

#include "cocos2d.h"
USING_NS_CC;

class CCMenuTouch : public cocos2d::CCLayerRGBA
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    virtual bool initWithArray(CCArray* pItems, int zOrder=0);
    static CCMenuTouch* create();
    static CCMenuTouch* createWithArray(CCArray* pItems, int zOrder=0);
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    static CCScene* sceneWithArray(CCArray* pItems, int zOrder=0);
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
private:
    CCArray* _items;
};

#endif // __HELLOWORLD_SCENE_H__
