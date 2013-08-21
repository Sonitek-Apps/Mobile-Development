//
//  CCSpriteTouch.cpp
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#include "CCSpriteTouch.h"
using namespace cocos2d;

CCSpriteTouch::CCSpriteTouch()
{
    _loadingIcon=NULL;
    _loadingAction=NULL;
    _nLoadingActionFrames=12;
    _loadingActionCycle=0.25;
    _bLoading=false;
}

CCSpriteTouch::~CCSpriteTouch()
{

}

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


void CCSpriteTouch::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}

void CCSpriteTouch::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}


bool CCSpriteTouch::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Began");
        if(!_bLoading){
            this->startLoading();
        }
        else{
            this->stopLoading();
        }
    }
    return true;
}

void CCSpriteTouch::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Ended");
        
    }
}

void CCSpriteTouch::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Moved");        
    }
}

void CCSpriteTouch::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Cancelled");
    }
}

CCAction* CCSpriteTouch::buildLoadingAction(){
    CCRotateBy* rotate=CCRotateBy::create(_loadingActionCycle, 360/_nLoadingActionFrames);
    CCRepeatForever* repeatForever=CCRepeatForever::create(rotate);
    return dynamic_cast<CCAction*>(repeatForever);
}

void CCSpriteTouch::startLoading(){
    if(!_bLoading){
        _bLoading=true;
        _loadingAction=buildLoadingAction();
        if(_loadingIcon==NULL){
            _loadingIcon=CCSprite::create("loading.png");
            _loadingIcon->setPosition(ccp(this->getContentSize().width*0.5,
                                          this->getContentSize().height*0.5));
            this->addChild(_loadingIcon, this->getZOrder()+1);
        }
        _loadingIcon->runAction(_loadingAction);
    }
}

void CCSpriteTouch::stopLoading(){
    if(_bLoading){
        _bLoading=false;
        _loadingIcon->stopAllActions();
    }
}