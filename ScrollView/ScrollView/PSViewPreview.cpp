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
    
    // initialize number limits
    m_numPhotosPerDay = 25;
    m_numDaysToLoad=10;
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
    
    // clear caches
    m_imageLoadedToSprite.clear();
    m_imageStartedDownload.clear();
    m_loadListBufferFinished = false;
    
    jsonReader = JSONReader::create(this, callfuncO_selector(PSViewPreview::onJSONDownloadFinished));
    return true;
}

void PSViewPreview::onEnter(){
    CCLayer::onEnter();
    m_pages = CCArray::create(); m_pages->retain();
}

void PSViewPreview::onExit(){
    CCLayer::onExit();
    m_pages->release();
}

inline void PSViewPreview::scrollViewDidScroll(CCScrollView* view)
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

void PSViewPreview::loadListBuffer()
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
        
        // load photos of the first date
        char buf[256]; sprintf(buf, "%04d-%02d-%02d", m_startDate.tm_year+1900, m_startDate.tm_mon+1, m_startDate.tm_mday);
        
        string targetDate(buf);
    }
}

CCSprite* PSViewPreview::cropRegionOfSpriteBySizeRatio(CCSprite* src, CCSize targetSize)
{
    if(!src) return NULL;
    
    CCSprite* ret= CCSprite::create();
    
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

map<imageIndex,imageAttr>::iterator PSViewPreview::firstPhotoOfDate(map<imageIndex, imageAttr>& photos, string& date){
    
    string indicatorString = date; indicatorString.append("-0000");
    imageIndex indicator(indicatorString);
    
    auto it = photos.find(indicator);
    return it;
}

string PSViewPreview::integrateToString(imageIndex indicator, imageAttr attributes){
    string ret = indicator.imageId;
    ret.append("/");
    char buf[256]; sprintf(buf,"%04d",attributes.posInQueue);
    ret.append(string(buf));
    return ret;
}

string PSViewPreview::imageIdFromString(const char* str){
    string tmp(str);
    return tmp.substr(0,15);
}

void PSViewPreview::onJSONDownloadFinished(CCObject* obj){
    JSONReader* jsonReader = (JSONReader*) obj;
    CC_SAFE_RELEASE_NULL(jsonReader);
    loadListBuffer();
}
