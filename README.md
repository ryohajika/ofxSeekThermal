# ofxSeekThermal
simple addon to use SeekThermal compact thermo cam in openframeworks app
![alt text](https://github.com/hzikajr/ofxSeekThermal/raw/master/ofxaddons_thumbnail.png "Preview")

## Mega thanks:
- maartenvds's amazing repo
https://github.com/OpenThermal/libseek-thermal
- hoehermann's work on absolute temperature reading
https://github.com/hoehermann/libseek-thermal/tree/temp-info_compact

## tested on:
MacBook Pro (15-inch, mid 2014), MacOSX 10.14.6, oF0.11.0
MacBook Pro (15-inch, late 2018), macOS 11.0, oF0.11.0

## How to use:
1. Update `opencv.a` in ofxOpenCv in your OF directory
⋅⋅⋅The original static library bundled to ofxOpenCv doesn't support some core OpenCV functions by default. I put modified versions in [my another repo](https://github.com/ryohajika/AnotherCvLibForOfxOpenCv) and you can download `.a` file for your platform.
```
(terminal, example for osx)
$ cd dir/to/your/OF/addons/ofxOpenCv/libs/opencv/lib/osx
$ mv opencv.a opencv.a.old
$ wget https://github.com/ryohajika/AnotherCvLibForOfxOpenCv/raw/main/osx/opencv.a
```
**caution: the modified versions supports 64bit only**

2. Clone this project along with submodules
⋅⋅⋅This addon uses [my fork of libseek-thermal](https://github.com/ryohajika/libseek-thermal), and you need to clone it along with this repo as a submodule.
```
(terminal)
$ cd dir/to/your/OF/addons
$ git clone https://github.com/ryohajika/ofxSeekThermal
$ cd ofxSeekThermal
$ git submodule init
$ git submodule update
```
After these you can run the example project.

## Implementation details:
The `ofxSeekThermalGrabber` is implemented based on `ofVideoGrabber` and `ofAVFoundationGrabber`. You can use the same method most of the case to setup, update, and draw pixels. Please check out `ofxSeekThermal.h` for the methods available.

## TODO:
- support open/close capability of seek camera while an ofApp is running
- put some info how to use this
- add thread error handler for the stability

### created by:
Ryo Hajika (imaginaryShort)
