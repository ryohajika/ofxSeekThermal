#include "ofxSeekThermal.h"

// most of the functions below are implemented based on @hoehermann's amazing work
// https://github.com/hoehermann/libseek-thermal/blob/temp-info_compact/examples/seek_viewer.cpp

#pragma mark ofxSeekThermalGrabber
ofxSeekThermalGrabber::ofxSeekThermalGrabber(){
    _b_init = false;
    _b_verbose = false;
    _b_newframe = false;
    _b_createff = false;
    seek = nullptr;
    seekpro = nullptr;
    seekcompact = nullptr;
    this->setDesiredFrameRate(7);
    this->setCVColorMap(cv::COLORMAP_JET);
    this->setVerbose(true);
}
ofxSeekThermalGrabber::~ofxSeekThermalGrabber(){
    if(_b_init) this->close();
}

bool ofxSeekThermalGrabber::setup(ofxSeekCamType type){
    _type = type;
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Compact (classic) camera");
        }
        seekcompact = new LibSeek::SeekThermal();
        seek = seekcompact;
        _rawPixels.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_MONO);
        _visPixels.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_RGB);
    }else{
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Pro camera");
        }
        seekpro = new LibSeek::SeekThermalPro();
        seek = seekpro;
        _rawPixels.allocate(THERMAL_PRO_WIDTH, THERMAL_PRO_HEIGHT, OF_PIXELS_MONO);
        _visPixels.allocate(THERMAL_PRO_WIDTH, THERMAL_PRO_HEIGHT, OF_PIXELS_RGB);
    }
    
    return this->init();
}
bool ofxSeekThermalGrabber::setup(ofxSeekCamType type, std::string flatfield_img_path){
    _type = type;
    
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Compact (classic) camera");
        }
        seekcompact = new LibSeek::SeekThermal(flatfield_img_path);
        seek = seekcompact;
        _rawPixels.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_MONO);
        _visPixels.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_RGB);
    }else{
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Going for Seek Thermal Pro camera");
        }
        seekpro = new LibSeek::SeekThermalPro(flatfield_img_path);
        seek = seekpro;
        _rawPixels.allocate(THERMAL_PRO_WIDTH, THERMAL_PRO_HEIGHT, OF_PIXELS_MONO);
        _visPixels.allocate(THERMAL_PRO_WIDTH, THERMAL_PRO_HEIGHT, OF_PIXELS_RGB);
    }
    
    return this->init();
}
bool ofxSeekThermalGrabber::init(){
    if(!seek->open()){
        if(_b_verbose){
            ofLogError("ofxSeekThermalGrabber::setup", "Error accessing camera");
        }
        return false;
    }else{
        if(_b_verbose){
            ofLogNotice("ofxSeekThermalGrabber::setup", "Camera opened!");
        }
        if(_b_createff){
            if(!seek->grab()){
                if(_b_verbose){
                    ofLogError("ofxSeekThermalGrabber::setup(FlatField)", "Failed to read initial frame from camera");
                }
                return false;
            }else{
                _b_init = true;
                std::unique_lock<std::mutex> lock(_seekmtx);
                seek->retrieve(_ff_u16frame);
                _warmup_ct++;
                this->startThread();
                return true;
            }
        }else{
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
}
void ofxSeekThermalGrabber::setCreateFlatfield(unsigned int warmupframes, unsigned int smoothingframes,
                                               std::string full_file_path_with_extension){
    _b_createff = true;
    
    _warmup_size = warmupframes;
    _warmup_ct = 0;
    _smoothing_size = smoothingframes;
    _smoothing_ct = 0;
    _ff_path = full_file_path_with_extension;
}
bool ofxSeekThermalGrabber::isFrameNew() const{
    return _b_newframe;
}
void ofxSeekThermalGrabber::threadedFunction(){
    while(isThreadRunning()){
        _b_newframe = false;
        if(_b_createff){
            if(!seek->grab()){
                if(_b_verbose){
                    ofLogError("ofxSeekThermalGrabber::threadedFunction(FlatField)", "Failed to read a frame from camera");
                }
            }else{
                std::unique_lock<std::mutex> lock(_seekmtx);
                if(_warmup_size > _warmup_ct){
                    seek->retrieve(_ff_u16frame);
                    _warmup_ct++;
                    if(_b_verbose){
                        ofLogNotice("ofxSeekThermalGrabber::threadedFunction(FlatField)",
                                    "warm up... %d/%d", _warmup_ct, _warmup_size);
                    }
                }else{
                    if(_smoothing_size > _smoothing_ct){
                        seek->retrieve(_ff_u16frame);
                        _ff_u16frame.convertTo(_ff_bufframe, CV_32FC1);
                        if(_smoothing_ct == 0){
                            _ff_bufframe.copyTo(_ff_avgframe);
                        }else{
                            _ff_avgframe += _ff_bufframe;
                        }
                        _smoothing_ct++;
                        if(_b_verbose){
                            ofLogNotice("ofxSeekThermalGrabber::threadedFunction(FlatField)",
                                        "smoothing... %d/%d", _smoothing_ct, _smoothing_size);
                        }
                    }else{
                        _ff_avgframe /= _smoothing_size;
                        _ff_avgframe.convertTo(_ff_u16frame, CV_16UC1);
//                      std::cout << _ff_u16frame.cols << ", " << _ff_u16frame.rows << ", " << _ff_u16frame.channels() << std::endl;
                        cv::imwrite(_ff_path, _ff_u16frame);
                        ofLogNotice("ofxSeekThermalGrabber::threadedFunction(FlatField)",
                                    "Exported Flatfield Information: %s", _ff_path.c_str());
                        _b_createff = false;
                    }
                }
            }
        }else{
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
                
                for(int i=0; i<_seekframe.rows; i++){
                    for(int j=0; j<_seekframe.cols; j++){
                        _rawPixels.setColor(j, i, _seekframe.at<unsigned short>(i,j));
                    }
                }
                ofxSeekThermal::process_frame(_seekframe, _outframe, 1.0f, _cmType, seek->device_temp_sensor(), _b_verbose);
                cv::cvtColor(_outframe, _outframe, cv::COLOR_BGR2RGB);
                ofxSeekThermal::toOf(_outframe, _visPixels);
                if(_b_verbose){
                    ofLogNotice("ofxSeekThermalGrabber::threadedFunction",
                                "[VIS] Cols: %d, Rows: %d, Chs: %d",
                                _outframe.cols, _outframe.rows, _outframe.channels());
                }
                _b_newframe = true;
            }
        }
        ofSleepMillis(_desiredfrmtime);
    }
}
void ofxSeekThermalGrabber::close(){
    this->waitForThread();
    this->stopThread();
    seek->close();
    seek = nullptr;
    if(_type == OFX_SEEK_THERMAL_CAM_COMPACT){
        delete seekcompact;
        seekcompact = nullptr;
    }else{
        delete seekpro;
        seekpro = nullptr;
    }
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
void ofxSeekThermalGrabber::getRawCVFrame(cv::Mat &dst){
    std::unique_lock<std::mutex> lock(_seekmtx);
    _seekframe.copyTo(dst);
}
ofPixels & ofxSeekThermalGrabber::getVisualizePixels(){
    std::unique_lock<std::mutex> lock(_seekmtx);
    return _visPixels;
}
const ofPixels & ofxSeekThermalGrabber::getVisualizePixels() const{
    //std::unique_lock<std::mutex> lock(_seekmtx);
    return _visPixels;
}
void ofxSeekThermalGrabber::getVisualizeCVFrame(cv::Mat &dst){
    std::unique_lock<std::mutex> lock(_seekmtx);
    _outframe.copyTo(dst);
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
