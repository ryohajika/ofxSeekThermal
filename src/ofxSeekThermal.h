#pragma once

#include "ofxSeekThermalBase.h"
#include "ofThread.h"

// forward declaration from ofxSeekThermalBase.h
double ofxSeekThermal::temp_from_raw(int, int);
void ofxSeekThermal::process_frame(cv::Mat&, cv::Mat&, float, int, int, bool);
void ofxSeekThermal::writepgm(const cv::Mat&, const std::string&, const int, const std::string&, const bool);
inline void ofxSeekThermal::toOf(const cv::Mat&, ofPixels&);

#pragma mark ofxSeekThermalGrabber
// referred to ofAVFoundationGrabber impl
class ofxSeekThermalGrabber : public ofxSeekThermalBaseGrabber,
                              public ofThread {
    public:
        ofxSeekThermalGrabber();
        ~ofxSeekThermalGrabber();
        
        bool isFrameNew() const;
        void close();
        bool setup(ofxSeekCamType type);
        
        ofShortPixels & getRawPixels();
        const ofShortPixels & getRawPixels() const;
        ofPixels & getVisualizePixels();
        const ofPixels & getVisualizePixels() const;
        
        void setVerbose(bool bTalkToMe);
        void setDesiredFrameRate(int framerate);
        
        // getWidth, getHeight -> vizualizer size (processed)
        // getRawWidth, getRawHeight -> original raw size (non-processed)
        float getHeight() const;
        float getWidth() const;
        float getRawHeight() const;
        float getRawWidth() const;
        bool isInitialized() const;
        
        void setCVColorMap(cv::ColormapTypes colormap);
        cv::ColormapTypes getCVColorMap() const;
  
    private:
        void threadedFunction();
        float _desiredfrmtime;
        std::mutex _seekmtx;
        std::condition_variable _seekcv;
        
        ofShortPixels _rawPixels;
        ofPixels _visPixels;
        int desiredFramerate;
        
        bool _b_init;
        bool _b_verbose;
        bool _b_newframe;
        
        ofxSeekCamType _type;
        LibSeek::SeekCam * seek;
        LibSeek::SeekThermalPro seekpro;
        LibSeek::SeekThermal seekcompact;
        cv::Mat _seekframe, _outframe;
        cv::ColormapTypes _cmType;
};

//class ofxSeekThermalPlayer : public ofxSeekThermalBase{
//
//};
