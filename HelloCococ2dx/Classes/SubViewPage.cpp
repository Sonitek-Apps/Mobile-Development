//
//  SubViewPage.cpp
//  ScrollTest
//
//  Created by Wing on 5/9/13.
//
//

#include "SubViewPage.h"
#include "Constants.h"
#include "CCHttpClientEx.h"
#include <set>
using namespace std;

static set<string> s_downloadBeganSet;

SubViewPage::SubViewPage(){
    _nodes = NULL;
    _urls = NULL;
    _pTarget = NULL;
}

SubViewPage::~SubViewPage(){
    CC_SAFE_RELEASE_NULL(_nodes);
    CC_SAFE_RELEASE_NULL(_urls);
    CC_SAFE_RELEASE_NULL(_pTarget);
    s_downloadBeganSet.clear();
}

void SubViewPage::onEnter(){
    CCLayer::onEnter();
}

void SubViewPage::onExit(){
    cleanupNodes();
    CCLayer::onExit();
}

bool SubViewPage::init(cocos2d::CCObject *pTarget, cocos2d::CCArray *urls, CCSize pageSize){
    
    this->setContentSize(pageSize);
    
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

SubViewPage* SubViewPage::create(cocos2d::CCObject *pTarget, cocos2d::CCArray *urls, CCSize pageSize){
    SubViewPage* instance = new SubViewPage();
    if (instance!=NULL && instance->init(pTarget, urls, pageSize)) {
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
    CCHttpClientEx* client = CCHttpClientEx::getInstance();
    client->send(request);
    client->setTimeoutForConnect(kConnnectionTimeout);
    client->setTimeoutForRead(kReadTimeout);
    request->release();
}

void SubViewPage::downloadAll(){
    
    setActive(true);
    
    CCObject* pObject=NULL;
    int i=0;
    CCARRAY_FOREACH(_urls, pObject){
        CCString* url = (CCString*)pObject;
        CCNode* node = (CCNode*)_nodes->objectAtIndex(i);
        if(s_downloadBeganSet.find(url->getCString())==s_downloadBeganSet.end()){
            s_downloadBeganSet.insert(url->getCString());
            downloadToNode(url->getCString(), node);
        }
        ++i;
    }
}

void SubViewPage::createNodes(){
    
    _nodes = CCArray::create();
    CC_SAFE_RETAIN(_nodes);

    CCSize dimensions(3,1);
    CCSize winSize = this->getContentSize();
    _gapSize = CCSize(0.1*winSize.width/(dimensions.width+1), 0.1*winSize.height/(dimensions.height+1));
    _iconSize = CCSize(
                       (winSize.width-_gapSize.width*(dimensions.width+1))/dimensions.width,
                       (winSize.height-_gapSize.height*(dimensions.height+1))/dimensions.height
                       );
    
    CCSize(0.2*this->getContentSize().width, this->getContentSize().height);
    
    CCPoint position(
                     _gapSize.width+0.5*_iconSize.width,
                     _gapSize.height+0.5*_iconSize.height
                     );
    
    for(int i=0;i<_urls->count();++i){
        CCNode* node = CCNode::create();
        
        int r = i/(int)dimensions.width, c = i%(int)dimensions.width;
        CCPoint positionTemp(
                             position.x+c*(_iconSize.width+_gapSize.width),
                             position.y-r*(_iconSize.height+_gapSize.height)
                             );
        
        node->setContentSize(_iconSize);
        node->setPosition(positionTemp);
        node->removeFromParent();
        
        this->addChild(node);
        _nodes->addObject(node);
    }
    
}

void SubViewPage::cleanupNodes(){
    
    setActive(false);
    
    for(int i=0;i<_nodes->count();++i){
        CCNode* node = (CCNode*) _nodes->objectAtIndex(i);
        if(node!=NULL && node->retainCount()>0){
            // remove sprites from node
            node->removeAllChildren();
        }
    }
}

void SubViewPage::onDownloadToNodeFinished(CCNode*, void* obj){
    
    CCHttpResponse* response = (CCHttpResponse*) obj;
    string url = response->getHttpRequest()->getUrl();
    void* pUserData = response->getHttpRequest()->getUserData();
    CCNode* node = (CCNode*) pUserData;
    
    if (node) {
        do{
            CC_BREAK_IF(!this || !this->isRunning());

            if(s_downloadBeganSet.find(url)!=s_downloadBeganSet.end()){
                s_downloadBeganSet.erase(url);
            }

            if(getActive()==false){
                return;
            }
            
            if(pUserData==NULL || !response->isSucceed()){
                CCLog("Receive Error! %s\n",response->getErrorBuffer());
                if(node!=NULL){
                    // re-download
                    if(s_downloadBeganSet.find(url)==s_downloadBeganSet.end()){
                        s_downloadBeganSet.insert(url);
                        downloadToNode(url, node);
                    }
                }
                break;
            }
            
            std::vector<char> *buffer = response->getResponseData();
            CCImage * pCCImage=new CCImage();
            pCCImage->initWithImageData(&(buffer->front()), buffer->size());
            CCTexture2D* tex = new CCTexture2D();
            tex->initWithImage(pCCImage);
            
            if(tex != NULL){
                CCSprite* spr = NULL;
                spr=CCSprite::createWithTexture(tex);
                cropRegionOfSpriteBySizeRatio(spr, node->getContentSize());
                node->removeAllChildren();
                node->addChild(spr,node->getZOrder()+1);
                // release unused pointers
                CC_SAFE_RELEASE_NULL(tex);
            }
            CC_SAFE_DELETE(pCCImage);
            
        }while(false);
    }
}