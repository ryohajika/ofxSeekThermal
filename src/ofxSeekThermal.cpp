#include "ofxSeekThermal.h"

// most of the functions below are implemented based on @hoehermann's amazing work
// https://github.com/hoehermann/libseek-thermal/blob/temp-info_compact/examples/seek_viewer.cpp

#pragma mark ofxSeekThermalGrabber
ofxSeekThermalGrabber::ofxSeekThermalGrabber(){
    _b_init = false;
    _b_verbose = false;
    _b_newframe = false;
    this->setDesiredFrameRate(7);
    this->setCVColorMap(cv::COLORMAP_JET);
    this->setVerbose(true);
}
ofxSeekThermalGrabber::~ofxSeekThermalGrabber(){
    this->close();
}

bool ofxSeekThermalGrabber::setup(ofxSeekCamType type){
    _type = type;
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        if(_b_verbose){
            ofLogError("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Compact (classic) camera");
        }
        seek = &seekcompact;
        _rawPixels.allocate(THERMAL_RAW_WIDTH, THERMAL_RAW_HEIGHT, OF_PIXELS_GRAY);
        _visPixels.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_RGB);
    }else{
        if(_b_verbose){
            ofLogError("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Pro camera");
        }
        seek = &seekpro;
        _rawPixels.allocate(THERMAL_PRO_RAW_WIDTH, THERMAL_PRO_RAW_HEIGHT, OF_PIXELS_GRAY);
        _visPixels.allocate(THERMAL_PRO_WIDTH, THERMAL_PRO_HEIGHT, OF_PIXELS_RGB);
    }
    
    if(!seek->open()){
        if(_b_verbose){
            ofLogError("ofxSeekThermalGrabber::setup", "Error accessing camera");
        }
        return false;
    }else{
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Camera opened!");
        }
        if(!seek->read(_seekframe)){
            if(_b_verbose){
                ofLogError("ofxSeekThermalGrabber::setup", "Failed to read initial frame from camera");
            }
            return false;
        }else{
            if(_b_verbose){
                ofLogNotice("ofxSeekThermalGrabber::setup", "WxH: %d, %d", _seekframe.cols, _seekframe.rows);
            }
            _b_init = true;
            std::unique_lock<std::mutex> lock(_seekmtx);
            ofxSeekThermal::process_frame(_seekframe, _outframe, 1.0f, _cmType, seek->device_temp_sensor(), _b_verbose);
            this->startThread();
            return true;
        }
    }
}
bool ofxSeekThermalGrabber::isFrameNew() const{
    return _b_newframe;
}
void ofxSeekThermalGrabber::threadedFunction(){
    while(isThreadRunning()){
        _b_newframe = false;
        if(!seek->read(_seekframe)){
            if(_b_verbose){
                ofLogError("ofxSeekThermalGrabber::threadedFunction", "Failed to read a frame from camera");
            }
        }else{
            std::unique_lock<std::mutex> lock(_seekmtx);
            if(_b_verbose){
                ofLogNotice("ofxSeekThermalGrabber::threadedFunction",
                            "[RAW] Cols: %d, Rows: %d, Chs: %d",
                            _seekframe.cols, _seekframe.rows, _seekframe.channels());
            }
            //ofxSeekThermal::toOf(_seekframe, _rawPixels);
            ofxSeekThermal::process_frame(_seekframe, _outframe, 1.0f, _cmType, seek->device_temp_sensor(), _b_verbose);
            ofxSeekThermal::toOf(_outframe, _visPixels);
            if(_b_verbose){
                ofLogNotice("ofxSeekThermalGrabber::threadedFunction",
                            "[VIS] Cols: %d, Rows: %d, Chs: %d",
                            _outframe.cols, _outframe.rows, _outframe.channels());
            }
            _b_newframe = true;
        }
        ofSleepMillis(_desiredfrmtime);
    }
}
void ofxSeekThermalGrabber::close(){
    this->stopThread();
    seek->close();
    _b_init = false;
    if(_b_verbose){
        ofLogNotice("ofxSeekThermalGrabber::close", "camera closed");
    }
}

ofShortPixels & ofxSeekThermalGrabber::getRawPixels(){
    std::unique_lock<std::mutex> lock(_seekmtx);
    return _rawPixels;
}
const ofShortPixels & ofxSeekThermalGrabber::getRawPixels() const{
    //std::unique_lock<std::mutex> lock(_seekmtx);
    return _rawPixels;
}
ofPixels & ofxSeekThermalGrabber::getVisualizePixels(){
    std::unique_lock<std::mutex> lock(_seekmtx);
    return _visPixels;
}
const ofPixels & ofxSeekThermalGrabber::getVisualizePixels() const{
    //std::unique_lock<std::mutex> lock(_seekmtx);
    return _visPixels;
}

void ofxSeekThermalGrabber::setVerbose(bool bTalkToMe){
    _b_verbose = bTalkToMe;
}
void ofxSeekThermalGrabber::setDesiredFrameRate(int framerate){
    desiredFramerate = framerate;
    _desiredfrmtime = 1000.0f / (float)desiredFramerate;
}

float ofxSeekThermalGrabber::getHeight() const{
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        return THERMAL_HEIGHT;
    }else{
        return THERMAL_PRO_HEIGHT;
    }
}
float ofxSeekThermalGrabber::getWidth() const{
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        return THERMAL_WIDTH;
    }else{
        return THERMAL_PRO_WIDTH;
    }
}
float ofxSeekThermalGrabber::getRawHeight() const{
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        return THERMAL_RAW_HEIGHT;
    }else{
        return THERMAL_PRO_RAW_HEIGHT;
    }
}
float ofxSeekThermalGrabber::getRawWidth() const{
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        return THERMAL_RAW_WIDTH;
    }else{
        return THERMAL_PRO_RAW_WIDTH;
    }
}
bool ofxSeekThermalGrabber::isInitialized() const{
    return _b_init;
}

void ofxSeekThermalGrabber::setCVColorMap(cv::ColormapTypes colormap){
    _cmType = colormap;
}
cv::ColormapTypes ofxSeekThermalGrabber::getCVColorMap() const{
    return _cmType;
}
