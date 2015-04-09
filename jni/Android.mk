LOCAL_PATH := $(call my-dir)
 
 

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
include /home/sepehr/Programming/workspace/OpenCV-2.4.9-android-sdk/sdk/native/jni/OpenCV.mk
 
OPENGLES_LIB  := -lGLESv1_CM
OPENGLES_DEF  := -DUSE_OPENGL_ES_1_1
LOCAL_MODULE    := rana
#LOCAL_SHARED_LIBRARIES := opencv-prebuilt 
LOCAL_SRC_FILES := Alvar.cpp Bitset.cpp Camera.cpp ConnectedComponents.cpp Draw.cpp Filter.cpp IntegralImage.cpp Kalman.cpp Line.cpp Marker.cpp MarkerDetector.cpp Optimization.cpp Pose.cpp ProjectImage.cpp rana.cpp RanaDrawable.cpp RanaGoli.cpp RanaHandler.cpp Ransac.cpp Rotation.cpp TrackerFeatures.cpp TrackerOrientation.cpp TrackerPsa.cpp TrackerStat.cpp TrifocalTensor.cpp UnscentedKalman.cpp Util.cpp
LOCAL_CXXFLAGS += -mthumb-interwork -pipe -fpic -fasm -ffast-math -ftree-vectorize -march=armv7-a -mvectorize-with-neon-quad -mfpu=neon -mfloat-abi=softfp -O2
#LOCAL_CXXFLAGS += -mthumb-interwork -pipe -fpic -fasm -ffast-math -ftree-vectorize -march=armv6 -mfloat-abi=softfp -O2
LOCAL_LDLIBS +=  $(OPENGLES_LIB) -llog -ldl
 
include $(BUILD_SHARED_LIBRARY)
