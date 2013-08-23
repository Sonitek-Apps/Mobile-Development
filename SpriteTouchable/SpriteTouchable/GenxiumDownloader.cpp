//
//  GenxiumDownloader.cpp
//  SpriteTouchable
//
//  Created by Wing on 24/8/13.
//
//

#include "GenxiumDownloader.h"

void GenxiumDownloader::initDownloaderWithTarget(CCObject* pTarget){

    do {
        _pTarget=pTarget;
        if(_pTarget){
            CC_SAFE_RETAIN(_pTarget);
        }
        _downloadQueue=CCArray::create();
        CC_SAFE_RETAIN(_downloadQueue);
        
        _scheduler= new CCScheduler(); // released in destructor
        _scheduler->scheduleSelector(schedule_selector(GenxiumDownloader::checkDownloadQueue), pTarget, 0.05, kCCRepeatForever, 0, false);
        
        _bInitFinished=true;
    } while (false);
}

void GenxiumDownloader::downloadToNode(CCNode* node, CCString* url){
    CCHttpRequest* request = new CCHttpRequest();
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setUrl(url->getCString());
    request->setResponseCallback(_pTarget,
                                 callfuncND_selector(GenxiumDownloader::onDownloadToNodeFinished));
    request->setUserData(node);
    
    CCHttpClient::getInstance()->send(request);
    request->release(); // check retain count here
}

void GenxiumDownloader::onDownloadToNodeFinished(CCNode*, void* obj){
    
    // release a quota
    if(0==pthread_mutex_trylock(&_downloadQuotaMutex)){
        ++_downloadQuota;
        pthread_mutex_unlock(&_downloadQuotaMutex);
    }
    
    CCHttpResponse* response = (CCHttpResponse*)obj;
    void* pUserData = response->getHttpRequest()->getUserData();
    
    if(pUserData==NULL || !response->isSucceed()){
        CCLog("Receive Error! %s\n",response->getErrorBuffer());
        return;
    }
    
    CCNode* targetNode = (CCNode*)pUserData;
    
    std::vector<char> *buffer = response->getResponseData();
    
    CCImage * pCCImage=new CCImage();
    
    pCCImage->initWithImageData(&(buffer->front()), buffer->size());
    CCTexture2D* tex = new CCTexture2D();
    tex->initWithImage(pCCImage);
    if(tex){
        CCSprite* spr = CCSprite::createWithTexture(tex);
        targetNode->removeAllChildren();
        targetNode->addChild(spr);
        CC_SAFE_RELEASE_NULL(tex);
    }
    CC_SAFE_RELEASE_NULL(tex);
    CC_SAFE_RELEASE_NULL(pCCImage);
}

bool GenxiumDownloader::addTaskToDownloadQueue(downloadTask* task){
    bool bRet=false;
    do{
        CC_BREAK_IF(_bInitFinished);
        if(0==pthread_mutex_trylock(&_downloadQueueMutex)){
            _downloadQueue->addObject(task);
            pthread_mutex_unlock(&_downloadQueueMutex);
            bRet=true;
        }
    }while(false);
    
    return bRet;
}

void GenxiumDownloader::checkDownloadQueue(float dt){
    // consume a quota
    if(0==pthread_mutex_trylock(&_downloadQuotaMutex)){
        if(_downloadQuota>0){
            if(0==pthread_mutex_trylock(&_downloadQueueMutex)){
                do{
                    CC_BREAK_IF(_downloadQueue->count()<=0);
                    --_downloadQuota;
                    // download the first task in queue
                    downloadTask* task=(downloadTask*)_downloadQueue->objectAtIndex(0);
                    downloadToNode(task->_node, task->_url);
                }while(false);
                pthread_mutex_unlock(&_downloadQueueMutex);
            }
        }
        pthread_mutex_unlock(&_downloadQuotaMutex);
    }
}