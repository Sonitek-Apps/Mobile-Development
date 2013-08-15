//
//  ImageUtils.h
//  ScrollView
//
//  Created by Wing on 9/8/13.
//
//

#ifndef __ScrollView__ImageUtils__
#define __ScrollView__ImageUtils__

#include <iostream>
#include <iostream>
#include <string>
#include "cJSON.h"
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Constants.h"
using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class ImageUtils{

public:
    
    void cropRegionOfTextureToImageBySizeRatio(CCTexture2D* srcTexture, CCImage* image, CCSize targetSize);
    string filenameFromUrl(string& url);
private:
    
};
#endif /* defined(__ScrollView__ImageUtils__) */
