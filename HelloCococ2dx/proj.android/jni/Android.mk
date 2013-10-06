LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

DIR_PATH_CLASSES:= $(LOCAL_PATH)/../../Classes

CPP_FILES:= $(shell find $(DIR_PATH_CLASSES) -type f -name "*.cpp")
C_FILES:= $(shell find $(DIR_PATH_CLASSES) -type f -name "*.c")

ALL_SRC_FILES:= $(CPP_FILES)
ALL_SRC_FILES+= $(C_FILES)
ALL_SRC_FILES+= $(shell find $(LOCAL_PATH)/hellocpp -type f -name "*.cpp")

ALL_INCLUDE_FOLDERS:= $(shell find $(DIR_PATH_CLASSES) -type d -print)

TRANSLATED_SRC_FILES:= $(subst $(LOCAL_PATH)/,,$(ALL_SRC_FILES))

$(warning $(TRANSLATED_SRC_FILES))

LOCAL_SRC_FILES:= $(TRANSLATED_SRC_FILES)
                  
LOCAL_C_INCLUDES := $(ALL_INCLUDE_FOLDERS)

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)
