//
//  Constants.h
//  ScrollTest
//
//  Created by Wing on 5/9/13.
//
//

#ifndef ScrollTest_Constants_h
#define ScrollTest_Constants_h

#define connnectionTimeout 15
#define readTimeout 60
#define kFeed "http://www.tactsky.com/ws/list/?view=list&from=tomlinsonphoto&type=latest"
#define feedListPath CCFileUtils::sharedFileUtils()->getWritablePath()+"urlList"
#define settingPath CCFileUtils::sharedFileUtils()->getWritablePath()+"setting"
#define defaultBufferSize 1024

#define xmlKeyNumberOfLocalImages "numberOfLocalImages"
#define activeIndicatorTips "Processing..."
#define sharedSaveFile CCUserDefault::sharedUserDefault()
#define nameMapPrefix "$%$#$-"
#define httpPreix "http"
#define numberOfDefaultPhotos 6
#define downloadedPhotosListPath (CCFileUtils::sharedFileUtils()->getWritablePath().append("downloadedPhotosList")).c_str()

#endif