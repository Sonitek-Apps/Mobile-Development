//
//  CCSpriteTouch.cpp
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#include "CCSpriteTouch.h"
#define defaultTouchPriority (-128)

CCSpriteTouch::CCSpriteTouch()
{
    _mainSpr=NULL;
    _pTarget=NULL;
    _pSelector=NULL;
    _loadingIcon=NULL;
    _loadingAction=NULL;
    _nLoadingActionFrames=12;
    _loadingActionCycle=0.25;
    _bLoading=false;
}

CCSpriteTouch::~CCSpriteTouch()
{
    CC_SAFE_RELEASE_NULL(_pTarget);
}

bool CCSpriteTouch::init(){
    bool bRet=false;
    do {
        bRet=true;
    } while (false);
    return bRet;
}

bool CCSpriteTouch::initWithSpriteAndTarget(CCSprite *spr,CCObject * pTarget, SEL_CallFuncND pSelector){
    bool bRet=false;
    do {
        setMainSprite(spr);
        setTarget(pTarget, pSelector);
        bRet=true;
    } while (false);
    return bRet;
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

CCSpriteTouch* CCSpriteTouch::createWithSpriteAndTarget(CCSprite *spr, CCObject *pTarget, SEL_CallFuncND pSelector)
{
    CCSpriteTouch* pRes=new CCSpriteTouch();
    if(pRes&&pRes->initWithSpriteAndTarget(spr, pTarget, pSelector)){
        pRes->autorelease();
        return pRes;
    }
    CC_SAFE_DELETE(pRes);
    return NULL;
}

void CCSpriteTouch::setMainSprite(CCSprite *spr){
    if(_mainSpr!=NULL){
        _mainSpr->removeFromParent();
        CC_SAFE_RELEASE_NULL(_mainSpr);
    }
    _mainSpr=spr;
    _mainSpr->setAnchorPoint(ccp(0, 0));
    this->addChild(_mainSpr);
    this->setContentSize(_mainSpr->getContentSize());
}

void CCSpriteTouch::setTarget(CCObject *pTarget, SEL_CallFuncND pSelector){
    if(_pTarget!=NULL){
        CC_SAFE_RELEASE_NULL(_pTarget);
    }
    _pTarget=pTarget;
    if(_pTarget!=NULL){
        CC_SAFE_RETAIN(_pTarget);
    }
    _pSelector=pSelector;
}

void CCSpriteTouch::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, defaultTouchPriority, true);
    CCNodeRGBA::onEnter();
}

void CCSpriteTouch::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCNodeRGBA::onExit();
}

bool CCSpriteTouch::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);

    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        CCLog("Touch Began");
        if(!_bLoading){
            this->startLoading();
            if(_pTarget && _pSelector){
                (_pTarget->*_pSelector)(this,NULL);
            }
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