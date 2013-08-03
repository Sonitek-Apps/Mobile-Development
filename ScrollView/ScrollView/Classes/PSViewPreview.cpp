//
//  PSViewPreview.cpp
//  Puzzle
//
//  Created by Tact Sky on 29/7/13.
//
//

#include "PSViewPreview.h"
#include "Constants.h"
#include "cJSON.h"
#include <math.h>

CCScene* PSViewPreview::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    PSViewPreview *layer = PSViewPreview::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool PSViewPreview::init()
{
    setTouchEnabled(true);
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float scaleFactor = 0.8;
    
    // initialize number limits
    m_numPhotosPerDay = 25;
    m_numDaysToLoad=3;
    m_cols=3;
    m_rows=(int)ceil((float)m_numPhotosPerDay/m_cols)*m_numDaysToLoad;
    m_numViewsPerPage=5;
    
    // initialize list file path
    m_listFilePath = CCFileUtils::sharedFileUtils()->getWritablePath()+"urlList";
    
    // initialize the download marks
    m_JSONDownloadFinished = false;
    m_loadListBufferFinished = false;
    
    m_scrollExceedingTop=false, m_scrollExceedingBottom=false;
    m_scrollBouncingFromTop=false, m_scrollBouncingFromBottom=false;
    
    
    float scrollViewCutBottomRatio = 0.08;
    
    m_sizeScrollView.setSize( winSize.width, winSize.height*(1-scrollViewCutBottomRatio) );
    m_sizeScrollPage.setSize( m_sizeScrollView.width, m_sizeScrollView.height*m_numViewsPerPage);
    m_sizeScrollCapacity.setSize(m_sizeScrollPage.width, m_numDaysToLoad*m_sizeScrollPage.height);
    
    m_pPreviewScroll = CCScrollView::create(m_sizeScrollView, NULL);
    m_pPreviewScroll->retain();
    m_pPreviewScroll->setContentSize(m_sizeScrollCapacity);
    m_pPreviewScroll->setDirection(kCCScrollViewDirectionVertical);
    m_pPreviewScroll->setContentOffset(CCPointZero, false);
    m_pPreviewScroll->setPosition(CCPointZero);
    m_pPreviewScroll->setDelegate(this);
    m_pPreviewScroll->setBounceable(true);
    
    this->addChild(m_pPreviewScroll);
    
    m_pPreviewScroll->setContentOffset(m_pPreviewScroll->minContainerOffset());
    
    m_topY = m_pPreviewScroll->minContainerOffset().y;
    m_bottomY = m_pPreviewScroll->maxContainerOffset().y;
    
    m_previousOffsetY = m_topY;
    
    CCLog("Scroll Capacity is %.2f points height, Scroll View is %.2f points height.", m_sizeScrollCapacity.height, m_sizeScrollView.height);
    

    // set scroll bar initial position
    //m_pPreviewScroll->getScrollBar()->setVisible(false);
    
    downloadJSON(m_feed);
    
    // clear caches
    m_imageLoadedToSprite.clear();
    m_imageStartedDownload.clear();
    schedule(schedule_selector(PSViewPreview::loadListBuffer), /*interval*/0.1, /*repeat*/kCCRepeatForever,/*delay*/0);
    m_loadListBufferFinished = false;
    return true;
}

inline void PSViewPreview::scrollViewDidScroll(cocos2d::extension::CCScrollView* view)
{
    
    CCPoint offset = m_pPreviewScroll->getContentOffset();
    //CCLog("previous offsetY is %.2f, current offsetY is %.2f",m_previousOffsetY, offset.y);
    if(offset.y<m_topY){
        // exceeding or bouncing back states
        if(m_previousOffsetY>=offset.y){
            // exceeding top
            m_scrollExceedingTop=true; m_scrollExceedingBottom=false;
            m_scrollBouncingFromTop=false; m_scrollBouncingFromBottom=false;
            
            CCLog("Exceeding Top!");
        }
        else{
            // bouncing from top
            m_scrollExceedingTop=false; m_scrollExceedingBottom=false;
            m_scrollBouncingFromTop=true; m_scrollBouncingFromBottom=false;
            CCLog("Bouncing from Top!");
        }
    }
    
    else if(offset.y>m_bottomY){
        // exceeding or bouncing back states
        if(m_previousOffsetY<=offset.y){
            // exceeding bottom
            m_scrollExceedingTop=false; m_scrollExceedingBottom=true;
            m_scrollBouncingFromTop=false; m_scrollBouncingFromBottom=false;
            CCLog("Exceeding Bottom!");
        }
        else{
            // bouncing from bottom
            m_scrollExceedingTop=false; m_scrollExceedingBottom=false;
            m_scrollBouncingFromTop=false; m_scrollBouncingFromBottom=true;
            CCLog("Bouncing from Bottom");
        }
    }
    else{
        // returning to normal
        if(m_scrollBouncingFromTop){
            m_scrollBouncingFromTop=false;
            CCLog("Returned by bouncing from top");
        }
        else if(m_scrollBouncingFromBottom){
            m_scrollBouncingFromBottom=false;
            CCLog("Returned by bouncing from bottom");
        }
        // normal
        else{
            if(m_loadListBufferFinished){
                // calculate the corresponnding date of scroll bar position only after list buffer loaded
                m_accOffsetY=m_accOffsetY+(offset.y-m_previousOffsetY); // Note the moving direction
                m_previousOffsetY=offset.y;
                time_t currentTime = m_startTime - secPerDay*(m_accOffsetY/m_sizeScrollPage.height);
                struct tm currentDate = *localtime(&currentTime);
                if(!equalOnDate(currentDate, m_previousDate)){
                    CCLog("Currently at Year: %04d, month: %02d, day: %02d", currentDate.tm_year+1900, currentDate.tm_mon+1, currentDate.tm_mday);
                    m_previousDate = currentDate;
                    
                    // download image of the date
                    char buf[256]; sprintf(buf, "%04d-%02d-%02d", currentDate.tm_year+1900, currentDate.tm_mon+1, currentDate.tm_mday);
                    
                    string targetDate(buf);
                    loadSpritesOfDate(targetDate);
                }
            }
        }
    }
    m_previousOffsetY=offset.y; //  store the previous position of the scroll offset
}

bool PSViewPreview::equalOnDate(struct tm d1, struct tm d2){
    return (d1.tm_year==d2.tm_year && d1.tm_mon==d2.tm_mon && d1.tm_mday==d2.tm_mday);
}

void PSViewPreview::fileNameFromUrl(string url, string& fileName, string& suffix){
    fileName = suffix = "\0";
    for(int i = 0;i<nImageTypes;++i){
        size_t dotPos = url.find(imageSuffix[i]);
        if(dotPos!=std::string::npos){
            // suffix found
            int lastSlashPos=url.size();
            for(int j=dotPos;j>=0;--j) {
                if(url[j]=='/'){
                    lastSlashPos=j;
                    break;
                }
            }
            fileName = url.substr(lastSlashPos+1, (dotPos+ sizeof(imageSuffix[i])/sizeof(char)) - (lastSlashPos+1));
            suffix = url.substr(dotPos,sizeof(imageSuffix[i])/sizeof(char));
        }
    }
}

bool PSViewPreview::checkListFile(struct tm date){
    FILE* fp = fopen(m_listFilePath.c_str(),"r");
    if(!fp) return false;
    else return true;
}

void PSViewPreview::downloadJSON(char* feed){
    CCHttpRequest *request = new CCHttpRequest();
    request->setUrl(feed);
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, callfuncND_selector(PSViewPreview::onJSONDownloadFinished));
    request->setTag("tactskySlidePuzzle");
    CCHttpClient::getInstance()->send(request);
    request->release();
}

void PSViewPreview::onJSONDownloadFinished( CCNode* node, void* obj )
{
	CCHttpResponse* response = (CCHttpResponse*)obj;
	if (!response->isSucceed())
	{
		CCLog("Receive Error! %s\n",response->getErrorBuffer());
		return ;
	}
    std::string res;
	const char* tag = response->getHttpRequest()->getTag();
	if (strcmp("tactskySlidePuzzle",tag)==0)
	{
		std::vector<char> *data = response->getResponseData();
		int data_length =  data->size();
		for (int i=0;i<data_length;++i){
            res+=(*data)[i];
        }
	}
    
    cJSON *cJsonRootArray = cJSON_Parse(res.c_str());
    FILE* fp = fopen(m_listFilePath.c_str(),"w");
    
    if (cJsonRootArray) {
        int arraySize = cJSON_GetArraySize(cJsonRootArray);
        for (int i = 0; i < arraySize; ++i) {
            cJSON *arrayItem = cJSON_GetArrayItem(cJsonRootArray, i);
            cJSON *dateItem = cJSON_GetObjectItem(arrayItem, "date");
            fputs(dateItem->valuestring,fp); fputs("\n",fp);
            cJSON *urlItem = cJSON_GetObjectItem(arrayItem, "imageUrl");
            fputs(urlItem->valuestring,fp); fputs("\n",fp);
        }
    }
    fflush(fp); fclose(fp);
    cJSON_Delete(cJsonRootArray);
    m_JSONDownloadFinished=true; // trigger "loadListBuffer" event
}

void PSViewPreview::loadListBuffer(float dt)
{
    if(m_JSONDownloadFinished==true){
        
        m_photos.clear(); // clear cache
        
        // savedPhotosUrls is guaranteed in-order
        
        FILE* fp=fopen(m_listFilePath.c_str(),"r");
        if(fp){
            CCLog("The list file has size %d.", ftell(fp));
            
            char buf[1024];
            char* fRet;
            memset(buf,0,sizeof(buf));
            
            int posInQueue=0;
            int count = m_numPhotosPerDay*m_numDaysToLoad;
            int withinGroupIndex=0;
            string previousDate = "";
            
            while( (fRet=fgets(buf,1024,fp)) && count>0 ){ // the first line is date
                
                std::string currentDate(buf); // groupId is date in %04d-%02d-%02d format
                
                if(posInQueue==0){
                    
                    // initialize time variabls
                    
                    m_startDate.tm_year = atoi(currentDate.substr(0,4).c_str())-1900;
                    m_startDate.tm_mon = atoi(currentDate.substr(5,2).c_str())-1;
                    m_startDate.tm_mday = atoi(currentDate.substr(8,2).c_str());
                    
                    // set m_startTime to 23:00:00 of the day
                    m_startDate.tm_hour=23;
                    m_startDate.tm_min=0;
                    m_startDate.tm_sec=0;
                    
                    m_startTime = mktime(&m_startDate);
                    m_previousDate = m_startDate;
                    
                    CCLog("Starting at Year: %02d, month: %02d, day: %02d", m_startDate.tm_year+1900, m_startDate.tm_mon+1, m_startDate.tm_mday);
                }
                
                if(currentDate!=previousDate){
                    withinGroupIndex=0; // reset withGroupIndex to when it comes to a new date
                    if(posInQueue%m_cols!=0){
                        posInQueue+=(m_cols-posInQueue%m_cols);
                    }
                    previousDate=currentDate;
                }
                currentDate.erase(currentDate.size()-1); // erase the new line token
               
                fRet=fgets(buf,1024,fp);  // the second line is url
                std::string url(buf);
                url.erase(url.size()-1); // erase the new line token
                
                imageIndex wholeIndex(currentDate, withinGroupIndex);
                imageAttr attributes(posInQueue, url);
                
                // check if the url contains valid image formats
                for(int i=0;i<nImageTypes;++i){
                    if(url.find(imageSuffix[i])!=std::string::npos){
                        
                        if(withinGroupIndex<m_numPhotosPerDay){
                            // Do NOT add more than m_NumPhotosPerDay to the url buffer
                            m_photos[wholeIndex]=attributes; // wholeIdPart is guranteed unique
                            ++posInQueue;
                        }
                        
                        ++withinGroupIndex;
                        break;
                    }
                }
                
                memset(buf,0,sizeof(buf)); // clear buf
                --count;
            }
            CCLog("Save to list buffer finished:");
            /*
            for(int i=0;i<savedPhotosUrls.size();++i){
                CCLog("%s",savedPhotosUrls[i].c_str());
            }
             */
        }
        else{
            CCLog("Read file %s error.", m_listFilePath.c_str());
        }
        fclose(fp);
        m_loadListBufferFinished = true;
        unschedule(schedule_selector(PSViewPreview::loadListBuffer));
        
        // load photos of the first date
        char buf[256]; sprintf(buf, "%04d-%02d-%02d", m_startDate.tm_year+1900, m_startDate.tm_mon+1, m_startDate.tm_mday);
        
        string targetDate(buf);
        loadSpritesOfDate(targetDate);
    }
}

void PSViewPreview::downloadImage(imageIndex indicator, imageAttr attributes)
{
    extension::CCHttpRequest *request = new extension::CCHttpRequest();
    request->setRequestType(extension::CCHttpRequest::kHttpGet);
    request->setUrl(attributes.url.c_str());
    request->setResponseCallback((CCObject*)this, callfuncND_selector(PSViewPreview::onImageDownloadFinished));
    request->setTag(integrateToTag(indicator, attributes).c_str());
    // get CCHttpClient instance and set timeouts
    CCHttpClient* client = CCHttpClient::getInstance();
    client->setTimeoutForConnect(5);
    client->setTimeoutForRead(60);
    
    // send request
    client->send(request);
}

void PSViewPreview::onImageDownloadFinished(CCNode *node, void* obj)
{
    CCHttpResponse* response = (CCHttpResponse*)obj;
    string requestUrl = response->getHttpRequest()->getUrl();
    string fileName, suffix;
    fileNameFromUrl(requestUrl, fileName, suffix);
    
    const char* tag = response->getHttpRequest()->getTag();
    
    string imageId = imageIdFromTag(tag);
    int posInQueue = posInQueueFromTag(tag);
    
	if (!response->isSucceed())
	{
		CCLog("Receive Error! %s\n",response->getErrorBuffer());
        m_imageStartedDownload.erase(imageId);// mark for re-download
        //downloadImage(indicator, requestUrl);
	}
    // extract fileName
    
	else if (m_imageStartedDownload.find(imageId)!=m_imageStartedDownload.end()) // a valid imageId should be found in the imageDownloadStarted map by far
	{
        string photoSavePath = CCFileUtils::sharedFileUtils()->getWritablePath()+imageId+suffix; // the image is saved using its imageId and suffix as a name
        std::vector<char> *buffer = response->getResponseData();
		
        CCImage * image=new CCImage();
        image->initWithImageData(&(buffer->front()), buffer->size());
        
        if(image->saveToFile(photoSavePath.c_str())){
            CCLog("Successfully saved data of %s", (imageId+suffix).c_str());
            
            // display the image in the grid w.r.t posInQueue
            CCSprite* spr = getSpriteFrom(image);
            
            // Note: I'm not sure if adding sprites to CCNode is a blocking thread/process.
            
            m_imageLoadedToSprite.insert(imageId);
            
            addSpriteToScroll(spr, m_pPreviewScroll, posInQueue, CCSize(m_cols,m_rows), CCSize(0.05, 0.06), CCSize(0.05, 0.03));
        }
        image->release();
	}
    
    else {
        m_imageStartedDownload.erase(imageId);// mark for re-download
        //downloadImage(indicator, requestUrl);
    }
}

void PSViewPreview::addSpriteToScroll(CCSprite* pIcon, CCScrollView* pScroll, int posInQueue, CCSize dimensions, CCSize marginRatio, CCSize gapsRatio)
{
    // Note that pScroll->getContentSize() returns the scroll container's size
    
    // calculate margin
    CCSize margin;
    margin.setSize(marginRatio.width*pScroll->getContentSize().width, marginRatio.height*pScroll->getViewSize().height);
    // calculate gaps
    CCSize gaps;
    gaps.setSize(gapsRatio.width*(pScroll->getContentSize().width-2*margin.width)/(dimensions.width+1), gapsRatio.height*(pScroll->getContentSize().height-2*margin.height)/(dimensions.height+1));
    
    // calculate iconSize
    CCSize iconSize;
    iconSize.setSize(
                            (pScroll->getContentSize().width-2*margin.width-(dimensions.width+1)*gaps.width)/dimensions.width,
                            (pScroll->getContentSize().height-2*margin.height-(dimensions.height+1)*gaps.height)/dimensions.height
                    );
    // calculate iconAnchor, here <0,0> is the lower-left point of pScroll
    CCPoint iconAnchor;
    iconAnchor.setPoint(
                            (gaps.width + margin.width+iconSize.width/2),
                            pScroll->getContentSize().height-(/*gaps.height*/ + margin.height + iconSize.height/2)
                        );
    // calculate current row & col
    int r = (posInQueue)/(int)dimensions.width, c = (posInQueue)%(int)dimensions.width;
    CCPoint anchorTemp(iconAnchor.x+c*(gaps.width+iconSize.width),
                       iconAnchor.y-r*(gaps.height+iconSize.height));
        
    // crop and scale the icon
    CCSprite *processedIcon = cropRegionOfSpriteBySizeRatio(pIcon, CCSize(iconSize.width,iconSize.height));
    processedIcon->setScale(iconSize.width/processedIcon->getContentSize().width);
    processedIcon->setPosition(anchorTemp);
    // Add to scroll view
    pScroll->getContainer()->addChild(processedIcon, 1);
}

CCSprite* PSViewPreview::cropRegionOfSpriteBySizeRatio(CCSprite* src, CCSize targetSize)
{
    if(!src) return NULL;
    
    CCSprite* ret= new CCSprite();
    ret->autorelease();
    
    CCTexture2D* mediumTex = ((CCSprite*)src)->getTexture();
    float srcWHRatio = mediumTex->getContentSize().width/mediumTex->getContentSize().height;
    float refWHRatio = targetSize.width/targetSize.height;
    
    if(srcWHRatio>refWHRatio){
        // keep src height, turn src width to src height*(ref width/ref height)
        float widthRect =  mediumTex->getContentSize().height*refWHRatio;
        float heightRect = mediumTex->getContentSize().height;
        float xRect = (mediumTex->getContentSize().width - widthRect)/2;
        float yRect = 0;
        
        CCRect ROI(xRect,yRect,widthRect,heightRect);
        //CCSpriteFrame* mediumFrame = CCSpriteFrame::createWithTexture(mediumTex, rect);
        //src->setDisplayFrame(mediumFrame);
        ret->initWithTexture(mediumTex, ROI);
    }
    else{
        // keep src width, turn src height to src width*(ref height/ref width)
        float widthRect =  mediumTex->getContentSize().width;
        float heightRect = mediumTex->getContentSize().width/refWHRatio;
        float xRect = 0;
        float yRect = (mediumTex->getContentSize().height-heightRect)/2;
        
        CCRect ROI(xRect,yRect,widthRect,heightRect);
        //CCSpriteFrame* mediumFrame = CCSpriteFrame::createWithTexture(mediumTex, rect);
        //src->setDisplayFrame(mediumFrame);
        ret->initWithTexture(mediumTex,ROI);
    }
    return ret;
}

string PSViewPreview::getWithinGroupIndex(string imageId){
    // Note that the fileName is constrained in format %04d-%02d-%02d-%04d
    return imageId.substr(11,4);
}

CCSprite* PSViewPreview::getSpriteFrom(CCImage* pCCImage){
    CCTexture2D* pTexture = new CCTexture2D();
    if (pTexture && pTexture->initWithImage(pCCImage)) {
        pTexture->autorelease();
        CCSprite* pSprite = CCSprite::createWithTexture(pTexture);
        return pSprite;
    }
    CC_SAFE_DELETE(pTexture);
    return NULL;
}

map<imageIndex,imageAttr>::iterator PSViewPreview::firstPhotoOfDate(map<imageIndex, imageAttr>& photos, string& date){
    
    string indicatorString = date; indicatorString.append("-0000");
    imageIndex indicator(indicatorString);
    
    auto it = photos.find(indicator);
    return it;
}

string PSViewPreview::integrateToTag(imageIndex indicator, imageAttr attributes){
    string ret = indicator.imageId;
    ret.append("/");
    char buf[256]; sprintf(buf,"%04d",attributes.posInQueue);
    ret.append(string(buf));
    return ret;
}

string PSViewPreview::imageIdFromTag(const char* tag){
    string tmp(tag);
    return tmp.substr(0,15);
}

int PSViewPreview::posInQueueFromTag(const char* tag){
    string tmp(tag);
    return atoi(tmp.substr(16,4).c_str());
}

void PSViewPreview::loadSpritesOfDate(string date){
    
    string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
    for(auto it = firstPhotoOfDate(m_photos, date);
        it!=m_photos.end();++it){
        
        if(it->first.date!= date){break;}
        
        string imageId = it->first.imageId;
        string fileName, suffix;
        fileNameFromUrl(it->second.url, fileName, suffix);
        string fullFilePath = dir+imageId+suffix; // the image is saved using its imageId and suffix as a name
        
        if(m_imageLoadedToSprite.find(imageId)!=m_imageLoadedToSprite.end()) continue; // image loaded
        
        else if(CCFileUtils::sharedFileUtils()->isFileExist(fullFilePath)){
                CCSprite* spr = CCSprite::create(fullFilePath.c_str());
                if(spr){
                    //CCLog("Successfully loaded sprite for %s, named %s.", imageId.c_str(), fileName.c_str());
                    // display the image in the grid w.r.t posInQueue
                    int posInQueue = it->second.posInQueue;
                    
                    // Note: I'm not sure if adding sprites to CCNode is a blocking thread/process.
                    
                    m_imageLoadedToSprite.insert(imageId);
                    
                    addSpriteToScroll(spr, m_pPreviewScroll, posInQueue, CCSize(m_cols,m_rows), CCSize(0.05, 0.06), CCSize(0.05, 0.03));
                    
                }
                else{
                    CCLog("Failed to load sprite for %s, named %s!", imageId.c_str(), fileName.c_str());
                }
        }
        else{
            if(m_imageStartedDownload.find(imageId)==m_imageStartedDownload.end()){
                // initial download or re-download
                downloadImage(it->first, it->second); // the id part is used as request tag
                m_imageStartedDownload.insert(imageId);
            }
        }
    }
}