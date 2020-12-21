//
//  ofxSeekThermalBase.h
//  example
//
//  Created by Ryo Hajika on 11/12/20.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "ofLog.h"
#include "ofUtils.h"
#include "ofVideoBaseTypes.h"
#include "ofEvents.h"
#include "ofTexture.h"
#include "ofPixels.h"
#include "ofImage.h"

#include "opencv2/opencv.hpp"

enum ofxSeekCamType {
    OFX_SEEK_THERMAL_CAM_COMPACT,
    OFX_SEEK_THERMAL_CAM_PRO
};

// based on ofBaseVideo / ofVideoGrabber / ofVideoPlayer implementation
// NOTE: multiple seek cam connection is NOT available
class ofxSeekThermalBase {
    public:
        virtual ~ofxSeekThermalBase(){}
        
        virtual bool isFrameNew() const = 0;
        virtual void close() = 0;
        virtual bool isInitialized() const = 0;
};

class ofxSeekThermalBaseDraws : virtual public ofxSeekThermalBase,
                                public ofBaseDraws,
                                public ofBaseHasTexturePlanes {
    public:
        virtual ~ofxSeekThermalBaseDraws(){}
};

class ofxSeekThermalBaseGrabber : virtual public ofxSeekThermalBase {
    public:
        virtual ~ofxSeekThermalBaseGrabber(){}
        
        virtual bool setup(ofxSeekCamType type) = 0;
        virtual float getHeight() const = 0;
        virtual float getWidth() const = 0;
        virtual float getRawHeight() const = 0;
        virtual float getRawWidth() const = 0;
        //virtual ofTexture * getTexturePtr(){ return nullptr; }
        virtual void setVerbose(bool bTalkToMe) = 0;
        virtual void setDesiredFrameRate(int framerate) = 0;
        //virtual void cameraSettings();
};

class ofxSeekThermalBasePlayer : virtual public ofxSeekThermalBase {
    public:
        virtual ~ofxSeekThermalBasePlayer();
        
        virtual bool load(std::string name) = 0;
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual float getHeight() const = 0;
        virtual float getWidth() const = 0;
        virtual ofTexture * getTexturePtr(){ return nullptr; }
        virtual bool isPaused() const = 0;
        virtual bool isLoaded() const = 0;
        virtual bool isPlaying() const = 0;
        virtual bool isInitialized() const { return isLoaded(); }
        virtual float getPosition() const;
        virtual float getSpeed() const;
        virtual float getDuration() const;
        virtual float getIsSequenceDone() const;
        virtual void setPaused(bool bPause);
        virtual void setPosition(float pct);
        virtual void setLoopState(ofLoopType state);
        virtual void setSpeed(float speed);
        virtual void setFrame(int frame);
        virtual int getCurrentFrame() const;
        virtual int getTotalNumFrames() const;
        virtual ofLoopType getLoopState() const;
        virtual void firstFrame();
        virtual void nextFrame();
        virtual void previousFrame();
        virtual void setVerbose(bool bTalkToMe);
        virtual void setDesiredFrameRate(int framerate);
        virtual void cameraSettings();
};
