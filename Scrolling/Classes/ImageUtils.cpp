#include "ImageUtils.h"

static const int nImageTypes=6;
static const char* imageSuffix[] = {".jpg",".png",".jpeg",".JPG",".PNG",".JPEG"};

ImageUtils::ImageUtils(){
    _quotaFilePath="";
    _nLoadingActionFrames=12;
    _loadingActionCycle=0.25;
}

ImageUtils::~ImageUtils(){
}

void ImageUtils::cropRegionOfSpriteBySizeRatio(CCSprite* src, CCSize targetSize){
    
    if(!src) return;
    
    CCTexture2D* mediumTex = ((CCSprite*)src)->getTexture();
    float srcWHRatio = mediumTex->getContentSize().width/mediumTex->getContentSize().height;
    float refWHRatio = targetSize.width/targetSize.height;
    CCRect ROI;
    if(srcWHRatio>refWHRatio){
        // keep src height, turn src width to src height*(ref width/ref height)
        float widthRect =  mediumTex->getContentSize().height*refWHRatio;
        float heightRect = mediumTex->getContentSize().height;
        float xRect = (mediumTex->getContentSize().width - widthRect)/2;
        float yRect = 0;
        
        ROI = CCRectMake(xRect,yRect,widthRect,heightRect);
    }
    else{
        // keep src width, turn src height to src width*(ref height/ref width)
        float widthRect =  mediumTex->getContentSize().width;
        float heightRect = mediumTex->getContentSize().width/refWHRatio;
        float xRect = 0;
        float yRect = (mediumTex->getContentSize().height-heightRect)/2;
        
        ROI = CCRectMake(xRect,yRect,widthRect,heightRect);
    }
    
    CCSpriteFrame* sprF =  CCSpriteFrame::createWithTexture(mediumTex, ROI);
    if(sprF){
        src->setDisplayFrame(sprF);
        src->setScale(targetSize.width/src->getContentSize().width);
        CC_SAFE_DELETE(sprF);
    }
}

CCImage* ImageUtils::cropRegionOfTextureToImageBySizeRatio(CCTexture2D* srcTexture, CCSize targetSize){
    
    CCSprite* croppedTextureToSprite=NULL;
    
    CCSize textureSize = srcTexture->getContentSize();
    
    if((textureSize.width/textureSize.height)>(targetSize.width/targetSize.height)) // image is wider than expected
    {
        CCSize newSize;
        newSize.setSize(textureSize.height*((targetSize.width/targetSize.height)), textureSize.height);
        CCRect ROI((textureSize.width-newSize.width)*0.5,0,newSize.width, newSize.height);
        croppedTextureToSprite = CCSprite::createWithTexture(srcTexture, ROI);
        //CCLog("case 1 :newSize is (height: %.2f, width: %.2f)", newSize.height, newSize.width);
    }
    else // image is higher than expected
    {
        CCSize newSize;
        newSize.setSize(textureSize.width, textureSize.width*(targetSize.height/targetSize.width));
        CCRect ROI(0,(textureSize.height-newSize.height)*0.5,newSize.width, newSize.height);
        croppedTextureToSprite = CCSprite::createWithTexture(srcTexture, ROI);
        //CCLog("case 2 :newSize is (height: %.2f, width: %.2f)", newSize.height, newSize.width);
    }
    //CCLog("cropped imageSize is (height: %.2f, width: %.2f)", croppedTextureToSprite->boundingBox().size.height, croppedTextureToSprite->boundingBox().size.width);
    // scaling
    float widthScale=(targetSize.width/croppedTextureToSprite->boundingBox().size.width);
    float heightScale=(targetSize.height/croppedTextureToSprite->boundingBox().size.height);
    float scale = widthScale<heightScale?widthScale:heightScale;
    
    croppedTextureToSprite->setScale(scale);
    //CCLog("scaled imageSize is (height: %.2f, width: %.2f)", croppedTextureToSprite->boundingBox().size.height, croppedTextureToSprite->boundingBox().size.width);
    
    CCRenderTexture* renderTexture = CCRenderTexture::create(croppedTextureToSprite->boundingBox().size.width, croppedTextureToSprite->boundingBox().size.height,kCCTexture2DPixelFormat_Default);
    //CC_SAFE_RETAIN(renderTexture);
    renderTexture->begin();
    croppedTextureToSprite->setAnchorPoint(ccp(0,0));
    croppedTextureToSprite->setPosition(ccp(0,0));
    croppedTextureToSprite->visit();
    renderTexture->end();
    
    CCImage* retImage=renderTexture->newCCImage();
    return retImage;
}

CCSprite* ImageUtils::getSpriteFromImage(CCImage* pCCImage){
    CCTexture2D* pTexture = new CCTexture2D();
    if (pTexture && pTexture->initWithImage(pCCImage)) {
        pTexture->autorelease();
        CCSprite* pSprite = CCSprite::createWithTexture(pTexture);
        return pSprite;
    }
    CC_SAFE_DELETE(pTexture);
    return NULL;
}

void ImageUtils::addNodeByCoordinateRatio(CCNode* canvas, CCNode* node, CCSize anchorRatio){
    CCPoint anchor(canvas->getContentSize().width*anchorRatio.width, canvas->getContentSize().height*anchorRatio.height);
    node->setPosition(anchor);
    canvas->addChild(node);
}

string ImageUtils::filenameFromUrlAndDate(const string& url, const string& date){
    // check if the url contains valid image formats
    for(int i=0;i<nImageTypes;++i){
        if(url.find(imageSuffix[i])!=std::string::npos){
            size_t dotPos = url.find_last_of(".");
            size_t slashPos = url.substr(0,dotPos).find_last_of("/");
            string ret = date;
            ret.push_back('-');
            ret.append(url.substr(slashPos+1, dotPos-1-slashPos));
            ret.append(imageSuffix[i]);
            return ret;
        }
    }
    return "";
}

bool ImageUtils::isLeapYear(int year){
    if(year%400==0) return true;
    else if(year%4==0 && year%100!=0) return true;
    else return false;
}

__int64_t ImageUtils::getDayFromOrigin(string date){
    
    // date must be in format %04d-%02d-%02d
    
    // Gregorian calendar
    int daysPerFourHundredYears= 146097;
    int daysPerHundredYears= 36524;
    int daysPerFourYears= 1461;
    int daysPerYear= 365;
    
    int year = atoi(date.substr(0,4).c_str());
    int month = atoi(date.substr(5,2).c_str());
    int day = atoi(date.substr(8,2).c_str());
    
    __int64_t ret=0;
    int r0=year;
    int q1=r0/400, r1=r0%400;
    ret+=(__int64_t)q1*daysPerFourHundredYears;
    int q2=r1/100, r2=r1%100;
    ret+=(__int64_t)q2*daysPerHundredYears;
    int q3=r2/4, r3=r2%4;
    ret+=(__int64_t)q3*daysPerFourYears+(__int64_t)r3*daysPerYear;
    
    for(int i=1;i<month;++i){
        if(i==1||i==3||i==5||i==7||i==8||i==10||i==12){
            ret+=(__int64_t)31;
        }
        else if(i==4||i==6||i==9||i==11){
            ret+=(__int64_t)30;
        }
        else{ // i==2
            if(isLeapYear(year)) {ret+=29.0;}
            else {ret+=(__int64_t)28;}
        }
    }
    ret+=(__int64_t)(day-1);
    return ret;
}

__int64_t ImageUtils::getDayDiffBetweenDates(string date1, string date2){
    __int64_t ret = getDayFromOrigin(date1)-getDayFromOrigin(date2);
    return ret;
}

bool ImageUtils::isInNameMap(CCString* url){
    bool bRet=false;
    do{
        CCString* key=CCString::createWithFormat("%s%s",kNameMapPrefix,url->getCString());
        string valueStr=kSharedSaveFile->getStringForKey(key->getCString());
        bRet=(valueStr.size()>0);
    }while(false);
    return bRet;
}

CCAction* ImageUtils::buildLoadingAction(){
    CCRotateBy* rotate=CCRotateBy::create(_loadingActionCycle, 360/_nLoadingActionFrames);
    CCRepeatForever* repeatForever=CCRepeatForever::create(rotate);
    return dynamic_cast<CCAction*>(repeatForever);
}

bool ImageUtils::containsImage(string &url){
    for(int i=0;i<nImageTypes;++i){
        if(url.find(imageSuffix[i])!=std::string::npos){
            return true;
        }
    }
    return false;
}
