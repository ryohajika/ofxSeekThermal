//
//  ofxSeekThermalUtility.h
//  example
//
//  Created by Ryo Hajika on 18/12/20.
//

#pragma once

#include <vector>
#include <string>
#include <math.h>
#include <memory>
#include <iostream>
#include <cstring>

#include "opencv2/opencv.hpp"
#include "ofPixels.h"
#include "ofImage.h"
#include "ofLog.h"

// MARK:- static utility function

namespace ofxSeekThermal {

// absolute temperature reading
// https://github.com/hoehermann/libseek-thermal/blob/temp-info_compact/examples/seek_viewer.cpp
static double temp_from_raw(int x, int device) {
    // Constants below are taken from linear trend line in Excel.
    // -273 is translation of Kelvin to Celsius
    // 330 is max temperature supported by Seek device
    // 16384 is full 14 bits value, max possible ()
    //x = x + 9645.54 - 2.54454 * double(device);
    x = x + 8858.13 - 2.3598 * double(device);
    double base = x * 330 / 16384.0;
    return base - 273.0;
}

// Function to process a raw (corrected) seek frame
static void process_frame(cv::Mat &inframe, cv::Mat &outframe,
                          float scale, int colormap, int device_temp_sensor, bool b_verbose) {
    cv::Mat frame_g8_nograd, frame_g16; // Transient Mat containers for processing

    // get raw max/min/central values
    double min, max, central;
    cv::minMaxIdx(inframe, &min, &max);
    cv::Scalar valat=inframe.at<uint16_t>(cv::Point(inframe.cols/2.0, inframe.rows/2.0));
    central=valat[0];

    double mintemp=temp_from_raw(min, device_temp_sensor);
    double maxtemp=temp_from_raw(max, device_temp_sensor);
    double centraltemp=temp_from_raw(central, device_temp_sensor);

    if(b_verbose){
        ofLogNotice("ofxSeekThermal: process_frame",
                    "rmin: %d, rmax: %d, central: %d, devtempsns: %d, min: %.1f, max: %.1f, center: %.1f",
                    (int)min, (int)max, (int)central, (int)device_temp_sensor, mintemp, maxtemp, centraltemp);
    }

    cv::normalize(inframe, frame_g16, 0, 65535, cv::NORM_MINMAX);

    // Convert seek CV_16UC1 to CV_8UC1
    frame_g16.convertTo(frame_g8_nograd, CV_8UC1, 1.0/256.0 );

    cv::Point minp, maxp, centralp;
    cv::minMaxLoc(frame_g8_nograd, NULL, NULL, &minp, &maxp); // doing it here, so we take rotation into account
    centralp=cv::Point(frame_g8_nograd.cols/2.0, frame_g8_nograd.rows/2.0);
    minp*=scale;
    maxp*=scale;
    centralp*=scale;

    // Resize image: http://docs.opencv.org/3.2.0/da/d54/group__imgproc__transform.html#ga5bb5a1fea74ea38e1a5445ca803ff121
    // Note this is expensive computationally, only do if option set != 1
    if (scale != 1.0){
        resize(frame_g8_nograd, frame_g8_nograd, cv::Size(), scale, scale, cv::INTER_LINEAR);
    }
    
    // add gradient
    cv::Mat frame_g8(cv::Size(frame_g8_nograd.cols+20, frame_g8_nograd.rows), CV_8U, cv::Scalar(128));
    for (int r = 0; r < frame_g8.rows-1; r++){
        frame_g8.row(r).setTo(255.0*(frame_g8.rows-r)/((float)frame_g8.rows));
    }
    frame_g8_nograd.copyTo(frame_g8(cv::Rect(0,0,frame_g8_nograd.cols, frame_g8_nograd.rows)));

    // Apply colormap: http://docs.opencv.org/3.2.0/d3/d50/group__imgproc__colormap.html#ga9a805d8262bcbe273f16be9ea2055a65
    if (colormap != -1) {
        //applyColorMap(frame_g8, outframe, colormap);
        applyColorMap(frame_g8_nograd, outframe, colormap);
    } else {
        //cv::cvtColor(frame_g8, outframe, cv::COLOR_GRAY2BGR);
        cv::cvtColor(frame_g8_nograd, outframe, cv::COLOR_GRAY2BGR);
    }
}

// custom pgm file writer
static void writepgm(const cv::Mat & seekframe,
              const std::string & prefix,
              const int framenumber,
              const std::string & comment,
              const bool ascii){
    std::ostringstream pgmfilename;
    pgmfilename << prefix;
    pgmfilename << std::internal << std::setfill('0') << std::setw(6);
    pgmfilename << framenumber;
    pgmfilename << ".pgm";
    
    std::ofstream pgmfile;
    pgmfile.open(pgmfilename.str());
    if(ascii){
        pgmfile << "P2\n";
    }else{
        pgmfile << "P5\n";
    }
    pgmfile << "# " << comment << "\n";
    pgmfile << seekframe.cols << " " << seekframe.rows << "\n";
    pgmfile << std::to_string(std::numeric_limits<uint16_t>::max()) << "\n";
    
    if(ascii){
        for(int y=0; y<seekframe.rows; y++){
            for(int x=0; x<seekframe.cols; x++){
                pgmfile << std::to_string(seekframe.at<uint16_t>(y, x)) << " ";
            }
        }
    }else{
        pgmfile.write(reinterpret_cast<char*>(seekframe.data), seekframe.total() * seekframe.elemSize());
    }
    pgmfile.close();
}

// function to convert cv::Mat to ofPixels
// https://gist.github.com/saccadic/f116daaf191a1c94b5f3af7174f9afb9
template <class T>
static inline void toOf(const cv::Mat & src, ofPixels_<T> & pix){
    pix.setFromExternalPixels(src.data, src.cols, src.rows, src.channels());
}
template <class T>
static inline cv::Mat toCv(ofPixels_<T> &pix){
    static int depth;
    static int channel;
    switch(pix.getBytesPerChannel()){
        case 4:
            depth = CV_32F;
            break;
        case 2:
            depth = CV_16U;
            break;
        case 1:
            depth = CV_8U;
            break;
    }
    channel = pix.getNumChannels();
    return cv::Mat(pix.getHeight(), pix.getWidth(), CV_MAKETYPE(depth, channel), pix.getData(), 0);
}

static bool exportRawPixelBuffer(std::string file, cv::Mat &src, int64_t frameid, int fmt){
    std::ofstream ofs;
    ofs.open(file);
    
    ofs << frameid << "\n";
    ofs << src.rows << "\n";
    ofs << src.cols << "\n";
    //ofs << src.channels() << "\n";
    ofs << fmt << "\n";
    
    for(int i=0; i<src.rows; i++){
        for(int j=0; j<src.cols; j++){
            ofs << src.at<unsigned short>(i,j);
            if((i==src.rows-1) && (j==src.cols-1)){
                ofs << "\n";
            }else{
                ofs << ",";
            }
        }
    }
    
    ofs.close();
    return true;
}
static bool readRawPixelBuffer(std::string file, cv::Mat &dst, int64_t &frameid, int &fmt){
    std::ifstream ifs;
    ifs.open(file, std::ifstream::in);
    
    std::string bufline;
    unsigned ctr = 0;
    unsigned row, col = 0;
    int format = 0;
    std::vector<unsigned short> bufvals;
    
    while(std::getline(ifs, bufline)){
        switch(ctr){
            case 0:
                frameid = std::stoi(bufline);
                break;
            case 1:
                row = std::stoi(bufline);
                ofLogNotice("ofxSeekThermal::readRawPixelBuffer", "ROW: %d", row);
                break;
            case 2:
                col = std::stoi(bufline);
                ofLogNotice("ofxSeekThermal::readRawPixelBuffer", "COL: %d", col);
                break;
            case 3:
                format = std::stoi(bufline);
                ofLogNotice("ofxSeekThermal::readRawPixelBuffer", "FMT: %d", format);
                break;
            case 4:
                std::stringstream ss(bufline);
                unsigned short bufnum;
                while(ss >> bufnum){
                    bufvals.push_back(bufnum);
                    if(ss.peek() == ',') ss.ignore();
                }
                ofLogNotice("ofxSeekThermal::readRawPixelBuffer", "size: %lu", bufvals.size());
                ss.clear();
                break;
        }
        ctr++;
    }
    ifs.close();
    
    cv::Mat m(row, col, format, bufvals.data());
    dst.create(row, col, format);
    dst = m;
    m.release();
    
    return true;
}

}; // namespace ofxSeekThermal
