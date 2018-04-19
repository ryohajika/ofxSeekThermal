//
//  ofxSeekThermal.h
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
//

#pragma once

#include "seek.h"
#include "SeekCam.h"
#include "cv.h"
#include "ofImage.h"
#include "ofEvents.h"
#include "ofLog.h"

#define ONE_DIV_BY_TWOFIVESIX 0.00390625
#define FRAME_INTERVAL_MS 120

using namespace cv;
using namespace LibSeek;

class ofxSeekThermal{
	private:
		// Function to process a raw (corrected) seek frame
		void process_frame(Mat &inframe, Mat &outframe, int colormap);
		void frameUpdate();
		int init();
	
		int fps;
		int colormap;
		Mat seekframe, outframe;
		LibSeek::SeekCam*	seek;
		LibSeek::SeekThermal	classiccam;
		bool bCamInit;
		bool bIsFrameNew;
		float lastElapsedTime;
		ofPixels pix;
	
	public:
		ofxSeekThermal();
		~ofxSeekThermal();
	
		int setup();
		void close();
		void update();
	
		bool isFrameNew();
		bool isInitialized();
	
		unsigned int getWidth();
		unsigned int getHeight();
		ofPixels&	 getPixels();
	
		void setColorMode(int color);
		int getColorMode();
	
		void draw(float x, float y, float w, float h);
};
