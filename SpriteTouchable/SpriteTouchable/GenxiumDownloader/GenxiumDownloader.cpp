//
//  GenxiumDownloader.cpp
//  SpriteTouchable
//
//  Created by Wing on 24/8/13.
//
//

#include "GenxiumDownloader.h"
#define defaultDownloadQuota 5
static pthread_mutex_t s_downloadQuotaMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_downloadQueueMutex = PTHREAD_MUTEX_INITIALIZER;
static int s_downloadQuota = defaultDownloadQuota;
static queue<downloadTask> s_downloadQueue;

static void* checkDownloadQueue(void* arg){ // set invisible for other files
    
    GenxiumDownloader* sharedDownloader = GenxiumDownloader::sharedDownloader();
    
    do{
        CC_BREAK_IF(s_downloadQueue.empty());
        
        // consume a quota
        if(0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
            if(s_downloadQuota>0){
                if(0==pthread_mutex_trylock(&s_downloadQueueMutex)){
                    do{
                        CC_BREAK_IF(s_downloadQueue.empty());
                        --s_downloadQuota;
                        // download the first task in queue
                        downloadTask task= s_downloadQueue.front();
                        sharedDownloader->downloadToNode(task._node, task._url);
                        s_downloadQueue.pop();
                        
                    }while(false);
                    pthread_mutex_unlock(&s_downloadQueueMutex);
                }
            }
            pthread_mutex_unlock(&s_downloadQuotaMutex);
        }
    }while(true);
    pthread_exit(NULL);
}

// singleton
static GenxiumDownloader* g_downloader;

GenxiumDownloader* GenxiumDownloader::sharedDownloader(){
    if(g_downloader==NULL){
        // reference count = 1, but not added to the pool manager
        g_downloader=new GenxiumDownloader();
    }
    return g_downloader;
}

void GenxiumDownloader::purgeSharedDownloader(){
    CC_SAFE_RELEASE_NULL(g_downloader);
}

GenxiumDownloader::GenxiumDownloader(){
    
    CCAssert(g_downloader==NULL, "DO NOT re-create a singleton!");
    _checkDownloadQueueThread=0;
}

GenxiumDownloader::~GenxiumDownloader(){
    purgeSharedDownloader();
}

void GenxiumDownloader::downloadToNode(CCNode* node, CCString* url){
    CCHttpRequest* request = new CCHttpRequest(); // request reference count = 1
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setUrl(url->getCString());
    request->setResponseCallback(this,
                                 callfuncND_selector(GenxiumDownloader::onDownloadToNodeFinished));
    request->setUserData(node);
    
    CCHttpClient::getInstance()->send(request); // request reference count = 2
    request->release(); // request reference count = 1
}

void GenxiumDownloader::onDownloadToNodeFinished(CCNode*, void* obj){

    CCHttpResponse* response = (CCHttpResponse*)obj;
    void* pUserData = response->getHttpRequest()->getUserData();
    
    if(pUserData==NULL || !response->isSucceed()){
        // release a quota
        if(0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
            ++s_downloadQuota;
            pthread_mutex_unlock(&s_downloadQuotaMutex);
        }
        CCLog("Receive Error! %s\n",response->getErrorBuffer());
        return;
    }
    CCNode* targetNode = (CCNode*)pUserData;
    
    std::vector<char> *buffer = response->getResponseData();
    
    CCImage * pCCImage=new CCImage();
    
    pCCImage->initWithImageData(&(buffer->front()), buffer->size());
    CCTexture2D* tex = new CCTexture2D();
    tex->initWithImage(pCCImage);
    
    if(tex != NULL){
        CCImage* pCroppedImage = cropRegionOfTextureToImageBySizeRatio(tex, targetNode->getContentSize());
        CCSprite* spr = getSpriteFromImage(pCroppedImage);
        spr->setAnchorPoint(CCPointZero);
        targetNode->removeAllChildren();
        targetNode->addChild(spr,targetNode->getZOrder()+1);
        // release unused pointers
        CC_SAFE_RELEASE_NULL(pCroppedImage);
        CC_SAFE_RELEASE_NULL(tex);
    }
    CC_SAFE_RELEASE_NULL(pCCImage);
    
    // release a quota
    if(0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
        ++s_downloadQuota;
        pthread_mutex_unlock(&s_downloadQuotaMutex);
    }
    
    if(0==pthread_mutex_trylock(&s_downloadQueueMutex) && 0==pthread_mutex_trylock(&s_downloadQuotaMutex)){
        if(s_downloadQueue.empty() && s_downloadQuota==defaultDownloadQuota){
            purgeSharedDownloader();
            pthread_mutex_unlock(&s_downloadQuotaMutex);
            pthread_mutex_unlock(&s_downloadQueueMutex);
        }
    }
}

bool GenxiumDownloader::addTaskToDownloadQueue(downloadTask task){
    bool bRet=false;
    do{
        if(0==pthread_mutex_trylock(&s_downloadQueueMutex)){
            s_downloadQueue.push(task);
            pthread_mutex_unlock(&s_downloadQueueMutex);
            bRet=true;
        }
    }while(false);
    return bRet;
}

void GenxiumDownloader::invokeCheckingDownloadQueue(){
    if(0==_checkDownloadQueueThread){
        int errorCodeCreateThread=pthread_create(&_checkDownloadQueueThread, NULL, &checkDownloadQueue, NULL);
        if(0==errorCodeCreateThread){
            CCLOG("Successfully created thread");
            int errorCodeJoinThread=pthread_join(_checkDownloadQueueThread, NULL);
            if(0==errorCodeJoinThread){
                CCLOG("Successful added thread");
            }
            else{
                CCLOG("Cannot add thread %d", errorCodeJoinThread);
            }
        }
        else{
            CCLOG("Cannot create thread %d",errorCodeCreateThread);
        }
    }
}