//
//  CCSpriteTouchable.cpp
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#include "CCSpriteTouchable.h"
using namespace cocos2d;
//===============================================================
CCSpriteTouchable::CCSpriteTouchable()
{

}
//===============================================================
CCSpriteTouchable::~CCSpriteTouchable()
{

}
//===============================================================
CCSpriteTouchable* CCSpriteTouchable::create(const char* filename)
{
    CCSpriteTouchable* pRes=new CCSpriteTouchable();
    if(pRes&&pRes->initWithFile(filename)){
        pRes->autorelease();
        return pRes;
    }
    CC_SAFE_DELETE(pRes);
    return NULL;
}

//===============================================================
void CCSpriteTouchable::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}
//===============================================================
void CCSpriteTouchable::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}
 
//===============================================================
bool CCSpriteTouchable::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    CCLog("Touch Began");
    return true;
}
//===============================================================
void CCSpriteTouchable::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCLog("Touch Ended");
}
//===============================================================
void CCSpriteTouchable::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    CCLog("Touch Moved");
}
//===============================================================
void CCSpriteTouchable::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent)
{
    CCLog("Touch Cancelled");
}
//===============================================================