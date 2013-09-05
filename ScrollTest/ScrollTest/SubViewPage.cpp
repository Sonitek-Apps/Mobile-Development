//
//  SubViewPage.cpp
//  ScrollTest
//
//  Created by Wing on 5/9/13.
//
//

#include "SubViewPage.h"
#include "Constants.h"

SubViewPage::SubViewPage(){
    _nodes = NULL;
    _urls = NULL;
    _pTarget = NULL;
}

SubViewPage::~SubViewPage(){
    CC_SAFE_RELEASE_NULL(_nodes);
    CC_SAFE_RELEASE_NULL(_urls);
    CC_SAFE_RELEASE_NULL(_pTarget);
}

void SubViewPage::onEnter(){
    CCLayer::onEnter();
}

void SubViewPage::onExit(){
    CCLayer::onExit();
}

bool SubViewPage::init(cocos2d::CCObject *pTarget, cocos2d::CCArray *urls){
    _pTarget=pTarget;
    if (_pTarget) {
        CC_SAFE_RETAIN(_pTarget);
    }
    _urls=urls;
    if(_urls){
        CC_SAFE_RETAIN(_urls);
    }
    createNodes();
    return true;
}

SubViewPage* SubViewPage::create(cocos2d::CCObject *pTarget, cocos2d::CCArray *urls){
    SubViewPage* instance = new SubViewPage();
    if (instance!=NULL && instance->init(pTarget, urls)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    instance = NULL;
    return NULL;
}

void SubViewPage::downloadToNode(const string &url, cocos2d::CCNode *node){
    CCHttpRequest *request = new extension::CCHttpRequest();
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setUrl(url.c_str());
    request->setResponseCallback((CCObject*)this, callfuncND_selector(SubViewPage::onDownloadToNodeFinished));
    request->setUserData(node);
    
    // send request
    CCHttpClient* client = CCHttpClient::getInstance();
    client->setTimeoutForConnect(connnectionTimeout);
    client->setTimeoutForRead(readTimeout);
    client->send(request);
    
    request->release();
}

void SubViewPage::downloadAll(){
    CCObject* pObject=NULL;
    int i=0;
    CCARRAY_FOREACH(_urls, pObject){
        CCString* url = (CCString*)pObject;
        CCNode* node = (CCNode*)_nodes->objectAtIndex(i);
        downloadToNode(url->getCString(), node);
        ++i;
    }
}

void SubViewPage::createNodes(){
    
    _nodes = CCArray::create();
    CC_SAFE_RETAIN(_nodes);

    CCSize dimensions(3,1);
    CCSize iconSize(0.2*this->getContentSize().width, this->getContentSize().height);
    CCPoint position(0.5*iconSize.width, 0.5*iconSize.height);
    
    for(int i=0;i<_urls->count();++i){
        CCNode* node = CCNode::create();
        
        int r = (i-1)/(int)dimensions.width, c = (i-1)%(int)dimensions.width;
        CCPoint positionTemp(position.x+c*iconSize.width, position.y-r*iconSize.height);
        
        node->setContentSize(iconSize);
        node->setPosition(positionTemp);
        node->removeFromParent();
        
        this->addChild(node);
        _nodes->addObject(node);
    }
    
}

void SubViewPage::onDownloadToNodeFinished(CCNode*, void* obj){
    
    CCHttpResponse* response = (CCHttpResponse*) obj;
    void* pUserData = response->getHttpRequest()->getUserData();
    CCNode* node = (CCNode*) pUserData;
    
    if (node) {
        do{
            
            if(pUserData==NULL || !response->isSucceed()){
                CCLog("Receive Error! %s\n",response->getErrorBuffer());
                break;
            }
            CC_BREAK_IF(!this || !this->isRunning());
            
            std::vector<char> *buffer = response->getResponseData();
            CCImage * pCCImage=new CCImage();
            pCCImage->initWithImageData(&(buffer->front()), buffer->size());
            CCTexture2D* tex = new CCTexture2D();
            tex->initWithImage(pCCImage);
            
            if(tex != NULL){
                CCSprite* spr = NULL;
                spr=CCSprite::createWithTexture(tex);
                node->removeAllChildren();
                node->addChild(spr,node->getZOrder()+1);
                // release unused pointers
                CC_SAFE_DELETE(tex);
            }
            CC_SAFE_DELETE(pCCImage);
            
        }while(false);
    }
}