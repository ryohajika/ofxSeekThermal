//
//  ofxSeekThermal.cpp
//  newseekthermal
//
//  Created by Ryo Hajika on 4/17/18.
//

// referred to maartenvd's amazing work
// https://github.com/maartenvds/libseek-thermal
/***
 The MIT License (MIT)
 
 Copyright (c) 2017 Eavise Research Group
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ***/

#include "ofxSeekThermal.h"

ofxSeekThermal::ofxSeekThermal(){
	fps = 7;
	colormap = COLORMAP_JET;
	bCamInit = false;
	bIsFrameNew = false;
}
ofxSeekThermal::~ofxSeekThermal(){
	pix.clear();
	close();
}

void ofxSeekThermal::process_frame(Mat &inframe, Mat &outframe, int colormap) {
	Mat frame_g8, frame_g16; // Transient Mat containers for processing
	
	normalize(inframe, frame_g16, 0, 65535, NORM_MINMAX);
	
	// Convert seek CV_16UC1 to CV_8UC1
	frame_g16.convertTo(frame_g8, CV_8UC1, ONE_DIV_BY_TWOFIVESIX );
	
	// Apply colormap: http://docs.opencv.org/3.2.0/d3/d50/group__imgproc__colormap.html#ga9a805d8262bcbe273f16be9ea2055a65
	if(colormap != -1){
		applyColorMap(frame_g8, outframe, colormap);
	} else {
		cv::cvtColor(frame_g8, outframe, cv::COLOR_GRAY2BGR);
	}
	cv::cvtColor(outframe, outframe, CV_BGR2RGB);
}

void ofxSeekThermal::update(){
	if((ofGetElapsedTimeMillis() - lastElapsedTime) > FRAME_INTERVAL_MS){
		frameUpdate();
		lastElapsedTime = ofGetElapsedTimeMillis();
	}
}
void ofxSeekThermal::frameUpdate(){
	// If signal for interrupt/termination was received, break out of main loop and exit
	if(!seek->read(seekframe)){
		ofLogError("ofxSeekThermal", "Error happend on cam reading frame");
		return -1;
	}
	
	// Retrieve frame from seek and process
	process_frame(seekframe, outframe, colormap);
	pix.setFromPixels(outframe.data, THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_RGB);
	
	bIsFrameNew = true;
}

int ofxSeekThermal::setup(){
	lastElapsedTime = ofGetElapsedTimeMillis();
	seek = &classiccam;
	
	if(!init()){
	
		pix.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_PIXELS_RGB);
	
		// kick-off!
		process_frame(seekframe, outframe, colormap);
	
		return 0;
		
	}else{
		return -1;
	}
}

int ofxSeekThermal::init(){
	if(!seek->open()){
		ofLogError("ofxSeekThermal", "Error happend on cam initalization");
		bCamInit = false;
		return 1;
	}
	bCamInit = true;
	
	// Retrieve a single frame, resize to requested scaling value and then determine size of matrix
	//  so we can size the VideoWriter stream correctly
	if(!seek->read(seekframe)){
		ofLogError("ofxSeekThermal", "Failed to read initial frame from camera, exiting");
		return -1;
	}
	
	return 0;
}

void ofxSeekThermal::close(){
	seek->close();
	bCamInit = false;
}

bool ofxSeekThermal::isFrameNew(){
	return bIsFrameNew;
}

bool ofxSeekThermal::isInitialized(){
	return bCamInit;
}

inline unsigned int ofxSeekThermal::getWidth(){
	return THERMAL_WIDTH;
}
inline unsigned int ofxSeekThermal::getHeight(){
	return THERMAL_HEIGHT;
}

ofPixels& ofxSeekThermal::getPixels(){
	bIsFrameNew = false;
	return pix;
}

void ofxSeekThermal::setColorMode(int color){
	colormap = color;
}
inline int ofxSeekThermal::getColorMode(){
	return colormap;
}

void ofxSeekThermal::draw(float x = 0, float y = 0, float w = THERMAL_WIDTH, float h = THERMAL_HEIGHT){
	ofImage buf;
	buf.setFromPixels(pix);
	buf.draw(x, y, w, h);
}
