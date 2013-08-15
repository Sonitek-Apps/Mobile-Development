//
//  ImageUtils.cpp
//  ScrollView
//
//  Created by Wing on 9/8/13.
//
//

#include "ImageUtils.h"

void ImageUtils::cropRegionOfTextureToImageBySizeRatio(CCTexture2D* srcTexture, CCImage* image, CCSize targetSize)
{
    CCSize textureSize = srcTexture->getContentSize();
    CCSprite* croppedSprite;
    
    if((textureSize.width/textureSize.height)>(targetSize.width/targetSize.height)) // image is wider than expected
    {
        CCSize newSize;
        newSize.setSize(textureSize.height*((targetSize.width/targetSize.height)), textureSize.height);
        CCRect ROI((textureSize.width-newSize.width)*0.5,0,newSize.width, newSize.height);
        croppedSprite = CCSprite::createWithTexture(srcTexture, ROI);
    }
    else // image is higher than expected
    {
        CCSize newSize;
        newSize.setSize(textureSize.width, textureSize.width*(targetSize.height/targetSize.width));
        CCRect ROI(0,(textureSize.height-newSize.height)*0.5,newSize.width, newSize.height);
        croppedSprite = CCSprite::createWithTexture(srcTexture, ROI);
    }
    croppedSprite->setScale(targetSize.width/croppedSprite->getContentSize().width);
    
    CCRenderTexture* renderTexture = new CCRenderTexture();
    renderTexture->initWithWidthAndHeight(croppedSprite->getContentSize().width, croppedSprite->getContentSize().height,kCCTexture2DPixelFormat_Default);
    
    renderTexture->addChild(croppedSprite);
    renderTexture->begin();
    croppedSprite->setAnchorPoint(ccp(0,0));
    croppedSprite->setPosition(ccp(0,0));
    croppedSprite->visit();
    renderTexture->end();
    image = renderTexture->newCCImage();
    CC_SAFE_RELEASE_NULL(croppedSprite);
    CC_SAFE_RELEASE_NULL(renderTexture);
}

string ImageUtils::filenameFromUrl(string& url){
    for(int i=0;i<nImageTypes;++i){
        if(url.find(imageSuffix[i])!=string::npos){
            int dotPos = url.find_last_of(".");
            int slashPos = url.substr(0,dotPos).find_last_of("/");
            return url.substr(slashPos+1, dotPos-slashPos-1);
        }
    }
}