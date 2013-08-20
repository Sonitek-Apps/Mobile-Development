//
//  CCSpriteTouch.cpp
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#include "CCSpriteTouch.h"
using namespace cocos2d;
//===============================================================
CCSpriteTouch::CCSpriteTouch()
{

}
//===============================================================
CCSpriteTouch::~CCSpriteTouch()
{

}
//===============================================================
CCSpriteTouch* CCSpriteTouch::create()
{
    CCSpriteTouch* pRes=new CCSpriteTouch();
    if(pRes&&pRes->init()){
        pRes->autorelease();
        return pRes;
    }
    CC_SAFE_DELETE(pRes);
    return NULL;
}
//===============================================================
CCSpriteTouch* CCSpriteTouch::create(const char* filename)
{
    CCSpriteTouch* pRes=new CCSpriteTouch();
    if(pRes&&pRes->initWithFile(filename)){
        pRes->autorelease();
        return pRes;
    }
    CC_SAFE_DELETE(pRes);
    return NULL;
}

//===============================================================
void CCSpriteTouch::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}
//===============================================================
void CCSpriteTouch::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}

//===============================================================
bool CCSpriteTouch::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Began");
        
    }
    return true;
}
//===============================================================
void CCSpriteTouch::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Ended");
        
    }
}
//===============================================================
void CCSpriteTouch::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Moved");        
    }
}
//===============================================================
void CCSpriteTouch::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Cancelled");
    }
}
//===============================================================