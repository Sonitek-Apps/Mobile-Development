//
//  JSONReader.cpp
//  Puzzle
//
//  Created by Tact Sky on 5/8/13.
//
//

#include "JSONReader.h"
#include "Constants.h"

JSONReader::JSONReader(){
}

JSONReader::~JSONReader(){
    CC_SAFE_RELEASE_NULL(m_pTarget);
}

JSONReader* JSONReader::create(CCObject* pTarget, SEL_CallFuncO pSelector)
{
    JSONReader *pRet = new JSONReader();
    if (pRet && pRet->init(pTarget, pSelector))
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

bool JSONReader::init(CCObject* pTarget, SEL_CallFuncO pSelector){
    bool bRet = false;
    do {
        // initialize feed string
        m_feed = kFeed;
        // initialize list file path
        m_listFilePath = CCFileUtils::sharedFileUtils()->getWritablePath()+"urlList";
        // initialize setting file path
        m_settingFilePath = CCFileUtils::sharedFileUtils()->getWritablePath()+"setting";
        
        m_pTarget = pTarget;
        CC_SAFE_RETAIN(m_pTarget);
        m_pSelector = pSelector;
        
        bool bCheckListFile = false; //isListFileUpToDate();
        if(!bCheckListFile){
            CCLog("Updating JSON list");
            downloadJSON();
        }
        else{
            CCLog("JSON list is up to date");
            if (m_pTarget && m_pSelector)
            {
                (m_pTarget->*m_pSelector)(this);
            }
            else{
                CCLog("Cannot call back from JSONReader!");
            }
            if ( m_pTarget ) {
                m_pTarget->release();
                m_pTarget = NULL;
            }
        }
        bRet = true;
    } while (0);
    return bRet;
}

void JSONReader::downloadJSON(){
    CCLog( "start to download" );
    
    cocos2d::extension::CCHttpRequest *request = new extension::CCHttpRequest();
    request->setUrl(m_feed.c_str());
    request->setRequestType(extension::CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, callfuncND_selector(JSONReader::onJSONDownloadFinished));
    request->setTag("tactskySlidePuzzle");
    
    CCHttpClient* client = CCHttpClient::getInstance();
    client->setTimeoutForConnect(connnectionTimeout);
    client->setTimeoutForRead(readTimeout);
    client->send(request);
    request->release();
    
    time(&m_start);
}

bool JSONReader::isListFileUpToDate(){
    bool bRet = false;
    FILE* fp = fopen(m_settingFilePath.c_str(),"r");
    if(fp) {
        char buf[256];
        if(fgets(buf, 256, fp)){
            if(sizeof(buf)!=0){
                string savedDateStr(buf);
                if(savedDateStr.find("\n")!=string::npos) {
                    savedDateStr.erase(savedDateStr.find("\n"));
                }
                time_t currentTime = time(0);
                struct tm currentDate = *localtime(&currentTime);
                memset(buf,0,sizeof(buf));
                sprintf(buf,"%04d-%02d-%02d",currentDate.tm_year+1900, currentDate.tm_mon+1, currentDate.tm_mday);
                string currentDateStr(buf);
                if(currentDateStr==savedDateStr){
                    bRet=true;
                }
            }
        }
    }
    fflush(fp);
    fclose(fp);
    return bRet;
}

bool JSONReader::updateListFile(){
    bool bRet = false;
    FILE* fp = fopen(m_settingFilePath.c_str(),"w");
    if(fp) {
        char buf[256];
        time_t currentTime = time(0);
        struct tm currentDate = *localtime(&currentTime);
        memset(buf,0,sizeof(buf));
        sprintf(buf,"%04d-%02d-%02d",currentDate.tm_year+1900, currentDate.tm_mon+1, currentDate.tm_mday);
        
        if(fputs(buf, fp)){
            bRet=true;
        }
    }
    fflush(fp);
    fclose(fp);
    return bRet;
}

void JSONReader::onJSONDownloadFinished(CCNode* , void* obj){
    CCHttpResponse* response = (CCHttpResponse*)obj;
    if (!response->isSucceed())
    {
        CCLog("Receive Error! %s\n",response->getErrorBuffer());
        return ;
    }
    
    CCLog( "time to download %ld", time(NULL) - m_start);
    time_t start;
    
    time(&start);
    
    std::vector<char> *data = response->getResponseData();
    cJSON *cJsonRootArray = cJSON_Parse(data->data());
    FILE* fp = fopen(m_listFilePath.c_str(),"w");
    
    if (cJsonRootArray) {
        int arraySize = cJSON_GetArraySize(cJsonRootArray);
        for (int i = 0; i < arraySize; ++i) {
            cJSON *arrayItem = cJSON_GetArrayItem(cJsonRootArray, i);
            cJSON *dateItem = cJSON_GetObjectItem(arrayItem, "date");
            cJSON *urlItem = cJSON_GetObjectItem(arrayItem, "imageUrl");
            
            if(dateItem!=NULL && urlItem!=NULL){
                fputs(dateItem->valuestring,fp); fputs("\n",fp);
                fputs(urlItem->valuestring,fp); fputs("\n",fp);
            }
        }
    }
    fflush(fp);
    fclose(fp);
    cJSON_Delete(cJsonRootArray);
    updateListFile();
    CCLog( "time to update file %ld", time(NULL) - start);
    
    if (m_pTarget && m_pSelector)
    {
        (m_pTarget->*m_pSelector)(response);
    }
    else{
        CCLog("Cannot call back from JSONReader!");
    }
    if ( m_pTarget ) {
        m_pTarget->release();
        m_pTarget = NULL;
    }
}
