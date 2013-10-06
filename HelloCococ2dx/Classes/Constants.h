//
//  Constants.h
//  ScrollTest
//
//  Created by Wing on 5/9/13.
//
//

#ifndef ScrollTest_Constants_h
#define ScrollTest_Constants_h

#define kConnnectionTimeout 60
#define kReadTimeout 120
#define kFeed "http://www.tactsky.com/ws/list/?view=list&from=tomlinsonphoto&type=latest"
#define kFeedListPath CCFileUtils::sharedFileUtils()->getWritablePath()+"urlList"
#define kSettingPath CCFileUtils::sharedFileUtils()->getWritablePath()+"setting"
#define kDefaultBufferSize 1024

#define kXmlKeyNumberOfLocalImages "numberOfLocalImages"
#define kActiveIndicatorTips "Processing..."
#define kSharedSaveFile CCUserDefault::sharedUserDefault()
#define kNameMapPrefix "$%$#$-"
#define kHttpPreix "http"
#define kDownloadedPhotosListPath (CCFileUtils::sharedFileUtils()->getWritablePath().append("downloadedPhotosList")).c_str()

#endif