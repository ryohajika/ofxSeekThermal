#pragma once

#include "seek.h"
#include "ofxSeekThermalBase.h"
#include "ofxSeekThermalUtility.h"
#include "ofThread.h"

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
        bool setup(ofxSeekCamType type, std::string flatfield_img_path);
        void setCreateFlatfield(unsigned warmupframes, unsigned smoothingframes,
                                std::string full_file_path_with_extension);
        
        ofShortPixels & getRawPixels();
        const ofShortPixels & getRawPixels() const;
        void getRawCVFrame(cv::Mat &dst);
        
        ofPixels & getVisualizePixels();
        const ofPixels & getVisualizePixels() const;
        void getVisualizeCVFrame(cv::Mat &dst);
        
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
        
        ofEvent<bool> new_frame_evt;
  
    private:
        bool init();
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
        bool _b_createff;
        
        ofxSeekCamType _type;
        LibSeek::SeekCam * seek;
        LibSeek::SeekThermalPro * seekpro;
        LibSeek::SeekThermal * seekcompact;
        cv::Mat _seekframe, _outframe;
        cv::ColormapTypes _cmType;
        
        unsigned _warmup_size;
        unsigned _warmup_ct;
        unsigned _smoothing_size;
        unsigned _smoothing_ct;
        cv::Mat _ff_avgframe, _ff_bufframe, _ff_u16frame;
        std::string _ff_path;
};

class ofxSeekThermalPlayer : public ofxSeekThermalBasePlayer,
                             public ofThread {
    public:
        ofxSeekThermalPlayer();
        ~ofxSeekThermalPlayer();
        
        bool isFrameNew() const;
        void close();
        bool isInitialized() const;
        bool load(std::string dirpath);
        bool load(ofxSeekCamType type, std::string dirpath);
        
        void play();
        void stop();
        
        float getHeight() const;
        float getWidth() const;
        float getPosition() const;
        float getSpeed() const;
        float getDuration() const;
        float getIsSequenceDone() const;
        int getCurrentFrame() const;
        int getTotalNumFrames() const;
        ofLoopType getLoopState() const;
        
        void setPaused(bool bPause);
        void setPosition(float pct);
        void setLoopState(ofLoopType state);
        void setSpeed(float speed);
        void setFrame(int frame);
        void setVerbose(bool bTalkToMe);
        void setDesiredFrameRate(int framerate);
        
        bool isPaused() const;
        bool isLoaded() const;
        bool isPlaying() const;
        
        void firstFrame();
        void nextFrame();
        void previousFrame();
        
        ofShortPixels & getRawPixels();
        const ofShortPixels & getRawPixels() const;
        void getRawCVFrame(cv::Mat &dst);
        
        ofPixels & getVisualizePixels();
        const ofPixels & getVisualizePixels() const;
        void getVisualizeCVFrame(cv::Mat &dst);
        
        void setCVColorMap(cv::ColormapTypes colormap);
        cv::ColormapTypes getCVColorMap() const;
    
    private:
        bool init();
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
        LibSeek::SeekThermalPro * seekpro;
        LibSeek::SeekThermal * seekcompact;
        cv::Mat _seekframe, _outframe;
        cv::ColormapTypes _cmType;
};
