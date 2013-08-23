//
//  GenxiumDownloader.cpp
//  SpriteTouchable
//
//  Created by Wing on 24/8/13.
//
//

#include "GenxiumDownloader.h"

static pthread_mutex_t s_downloadQuotaMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_downloadQueueMutex = PTHREAD_MUTEX_INITIALIZER;
static int s_downloadQuota = 5;
static CCArray* s_downloadQueue = new CCArray();

static void* checkDownloadQueue(void* arg){ // set invisible for other files
    
    GenxiumDownloader* pObject=(GenxiumDownloader*)arg;
    
    do{
        CC_BREAK_IF(s_downloadQueue->count()<=0);
        
        // consume a quota
        if(0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
            if(s_downloadQuota>0){
                if(0==pthread_mutex_trylock(&s_downloadQueueMutex)){
                    do{
                        CC_BREAK_IF(s_downloadQueue->count()<=0);
                        --s_downloadQuota;
                        // download the first task in queue
                        downloadTask* task= (downloadTask*)s_downloadQueue->objectAtIndex(0);
                        pObject->downloadToNode(task->_node, task->_url);
                        s_downloadQueue->removeObjectAtIndex(0);
                        
                    }while(false);
                    pthread_mutex_unlock(&s_downloadQueueMutex);
                }
            }
            pthread_mutex_unlock(&s_downloadQuotaMutex);
        }
    }while(true);
    return NULL;
}

void GenxiumDownloader::initDownloaderWithTarget(CCObject* pTarget){

    do {
        _pTarget=pTarget;
        if(_pTarget){
            CC_SAFE_RETAIN(_pTarget);
        }
        if(0 != pthread_create(&_checkDownloadQueueThread, NULL, &checkDownloadQueue, this)){
            CCLOG("Cannot create thread!");
        }
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
    if(0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
        ++s_downloadQuota;
        pthread_mutex_unlock(&s_downloadQuotaMutex);
    }
    
    CCHttpResponse* response = (CCHttpResponse*)obj;
    void* pUserData = response->getHttpRequest()->getUserData();
    
    if(pUserData==NULL || !response->isSucceed()){
        CCLog("Receive Error! %s\n",response->getErrorBuffer());
        //return;
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
        CC_BREAK_IF(!_bInitFinished);
        if(0==pthread_mutex_trylock(&s_downloadQueueMutex)){
            s_downloadQueue->addObject(task);
            pthread_mutex_unlock(&s_downloadQueueMutex);
            bRet=true;
        }
    }while(false);
    return bRet;
}

void GenxiumDownloader::invokeCheckingDownloadQueue(){
    pthread_join(_checkDownloadQueueThread, NULL);
}