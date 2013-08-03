//
//  PSViewPreview.h
//  Puzzle
//
//  Created by Tact Sky on 29/7/13.
//
//

#ifndef __Puzzle__PSViewPreview__
#define __Puzzle__PSViewPreview__

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <pthread.h>

#include "cocos2d.h"
#include "CCScrollView.h"
#include "CCScheduler.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "imageIndex.h"
#include "imageAttr.h"
#include "cJSON.h"

using namespace cocos2d;
using namespace extension;
using namespace std;

class PSViewPreview: public CCLayer, public CCScrollViewDelegate
{
private:
    
    char* m_feed = "http://www.tactsky.com/ws/list/?view=list&from=tomlinsonphoto&type=latest";
    
    int m_numPhotosPerDay;
    int m_numDaysToLoad;
    int m_rows, m_cols; // number of rows and columns in the whole scroll
    int m_numViewsPerPage;
    
    string m_listFilePath;
    float m_topY, m_bottomY;
    float m_previousOffsetY;
    float m_accOffsetY;
    
    bool m_scrollExceedingTop, m_scrollExceedingBottom; // scroll bar status indicators
    bool m_scrollBouncingFromTop, m_scrollBouncingFromBottom; // scroll bar status indicators
    
    cocos2d::extension::CCScrollView* m_pPreviewScroll;
    CCSize m_sizeScrollView; // scrollView is slightly shorter than scrollPage to fit the showcase
    CCSize m_sizeScrollPage; // scrollPage is used to calculate the date
    CCSize m_sizeScrollCapacity;
    
    time_t m_startTime;
    struct tm m_startDate;
    struct tm m_previousDate;
    
    // caches
    CCArray* m_pDigits;
    map<imageIndex,imageAttr> m_photos; // map<(groupId,withinGroupIndex),(posInQueue, url)>
    string m_previousWithinGroupIndex;
    
    // download marks
    bool m_JSONDownloadFinished;
    bool m_LoadListBufferFinished;
    set<string> m_imageStartedDownload;
    set<string> m_imageLoadedToSprite;
    
public:
    virtual bool init();

    static cocos2d::CCScene* scene();
    CREATE_FUNC(PSViewPreview);

    inline virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);
    inline virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view){}
    
public:
    bool equalOnDate(struct tm d1, struct tm d2);
    
    void fileNameFromUrl(string url, string& fileName, string& suffix);
    bool checkListFile(struct tm date);
    
    void downloadJSON(char* feed);
    void onJSONDownloadFinished(CCNode* node, void* obj );
    void loadListBuffer(float dt);
    void loadPhotoSprite(float dt);
    void downloadImage(imageIndex indicator, imageAttr attributes);
    void onImageDownloadFinished(CCNode* node,void* obj );
    void addSpriteToScroll(CCSprite *pIcon, CCScrollView* pScroll, int posInQueue, CCSize dimensions, CCSize marginRatio, CCSize gapsRatio);
    CCSprite* cropRegionOfSpriteBySizeRatio(CCSprite* src, CCSize targetSize);
    string getWithinGroupIndex(string imageId);
    
    CCSprite* getSpriteFrom(CCImage* pCCImage);
    map<imageIndex,imageAttr>::iterator firstPhotoOfDate(map<imageIndex, imageAttr>& photosUrls, string& date);
    
    string integrateToTag(imageIndex indicator, imageAttr attributes);
    string imageIdFromTag(const char* tag);
    int posInQueueFromTag(const char* tag);
};

#endif /* defined(__Puzzle__PSViewPreview__) */
