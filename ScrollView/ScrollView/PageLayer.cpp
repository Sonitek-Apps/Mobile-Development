//
//  PageLayer.cpp
//  ScrollView
//
//  Created by Wing on 10/8/13.
//
//

#include "PageLayer.h"

PageLayer::PageLayer(){
    _items = CCArray::create(); CC_SAFE_RETAIN(_items);
    _menu = CCMenu::createWithArray(_items); CC_SAFE_RETAIN(_menu);
}

PageLayer::~PageLayer(){
    CC_SAFE_RELEASE_NULL(_items);
    CC_SAFE_RELEASE_NULL(_menu);
}

bool PageLayer::init(){
    return true;
}

static PageLayer* create()
{
    PageLayer *pRet = new PageLayer(); 
    if (pRet && pRet->init())
    { 
        pRet->autorelease();
        return pRet;
    } 
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    } 
}


void PageLayer::downloadImageToNode(string& url, CCNode* node, string& saveMask){
    CCHttpRequest *request = new CCHttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, callfuncND_selector(PageLayer::onImageDownloadFinished));
    request->setUserData(node);
    /*
     saveMask:
     000--don't save;
     001--save cropped image to texture cache only;
     010--save original image texture cache only;
     100--save original image to local storage only;
     101--save cropped image to texture cache, and save original image to local storage
     etc...
     */
    request->setTag(saveMask.c_str());
    CCHttpClient::getInstance()->send(request);
    request->release();
}

void PageLayer::onImageDownloadFinished(CCNode* node, void* obj){
    
    CCHttpResponse* response = (CCHttpResponse*)obj;
    string requestUrl = response->getHttpRequest()->getUrl();
    string saveMask = response->getHttpRequest()->getTag();
    void* pUserData = response->getHttpRequest()->getUserData();
    CCNode* targetNode = (CCNode*) pUserData;
    
	if (!response->isSucceed())
	{
		CCLog("Receive Error! %s\n",response->getErrorBuffer());
	}
    // extract fileName
    
	else{
        std::vector<char> *buffer = response->getResponseData();
        CCImage * image=new CCImage();
        image->initWithImageData(&(buffer->front()), buffer->size());
        CCTexture2D* tex = new CCTexture2D();
        tex->initWithImage(image);
        CCSprite* spr = CCSprite::createWithTexture(tex);
        
        targetNode->removeAllChildren();
        targetNode->addChild(spr);
        
        if(saveMask[0]=='1'){
            // save original image to local storage
            image->saveToFile(filenameFromUrl(requestUrl).c_str());
        }
        if(saveMask[1]=='1'){
            // save original image to texture cache
            CCTextureCache::sharedTextureCache()->addUIImage(image, filenameFromUrl(requestUrl).c_str());
        }
        if(saveMask[2]=='1'){
            // save cropped image to texture cache
            CCImage* croppedImage = new CCImage();
            cropRegionOfTextureToImageBySizeRatio(tex, croppedImage, targetNode->getContentSize());
            CCTextureCache::sharedTextureCache()->addUIImage(croppedImage, filenameFromUrl(requestUrl).c_str());
            CC_SAFE_RELEASE_NULL(croppedImage);
        }
        
        CC_SAFE_RELEASE_NULL(image);
	}
}

