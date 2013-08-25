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
    _lastTouchBegan = CCPointZero;
    _lastTouchEnded = CCPointZero;
    _scrollFriendlyThreshold = 1e02;
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


void CCSpriteTouch::onEnter()
{
    setTouchEnabled(true);
    CCNodeRGBA::onEnter();
}

void CCSpriteTouch::onExit()
{
    setTouchEnabled(false);
    CCNodeRGBA::onExit();
}

bool CCSpriteTouch::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    _lastTouchBegan=locationInNode;
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
    _lastTouchEnded=locationInNode;
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        if(_pTarget && _pSelector && !isDragging()){
            (_pTarget->*_pSelector)(this,NULL);
        }
    }
}

void CCSpriteTouch::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint location = pTouch->getLocation();
    CCPoint locationInNode=this->getParent()->convertToNodeSpace(location);
    CCRect box=this->boundingBox();
    if(box.containsPoint(locationInNode)){
        //CCLog("Touch Moved");
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

void CCSpriteTouch::setPenetrable(bool bPenetrable){
    CCDirector* pDirector=CCDirector::sharedDirector();
    CCTouchHandler* touchHandler=pDirector->getTouchDispatcher()->findHandler(this);
    if(touchHandler!=NULL){
        pDirector->getTouchDispatcher()->removeDelegate(this);
    }
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, defaultTouchPriority, !bPenetrable);
}

bool CCSpriteTouch::isDragging(){
    float dx = _lastTouchBegan.x - _lastTouchEnded.x;
    float dy = _lastTouchBegan.y - _lastTouchEnded.y;
    float distanceSquare = dx*dx+dy*dy;
    return (distanceSquare>_scrollFriendlyThreshold);
}

void CCSpriteTouch::setScrollFriendlyThreshold(float scrollFriendlyThreshold){
    setPenetrable(true); // Scroll friendly CCSpriteTouch must be penetrable
    _scrollFriendlyThreshold=scrollFriendlyThreshold;
}

void CCSpriteTouch::setTouchEnabled(bool bTouchEnabled){
    CCDirector* pDirector=CCDirector::sharedDirector();
    CCTouchHandler* touchHandler=pDirector->getTouchDispatcher()->findHandler(this);
    if(bTouchEnabled==true){
        if(touchHandler==NULL){
            pDirector->getTouchDispatcher()->addTargetedDelegate(this, defaultTouchPriority, true);
        }
    }
    else{
        if(touchHandler!=NULL){
            pDirector->getTouchDispatcher()->removeDelegate(this);
        }
    }
}