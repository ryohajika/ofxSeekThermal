meta:
	ADDON_NAME = ofxSeekThermal
	ADDON_DESCRIPTION = openFrameworks addon using @maartenvds driver software and @undera and @hoehermanns works to use SeekThermal cameras
	ADDON_AUTHOR = Ryo Hajika
	ADDON_TAGS = "camera" "computervision" "thermalcamera" "thermograpy" "seekthermal"
	ADDON_URL = https://github.com/ryohajika/ofxSeekThermal.git

common:
	ADDON_DEPENDENCIES = ofxOpenCv

	ADDON_INCLUDES += libs/libusb/include
	ADDON_INCLUDES += libs/libseek-thermal/src
	ADDON_INCLUDES += src
	ADDON_INCLUDES += libs/libseek-thermal/win

	#ADDON_SOURCES += libs/libseek-thermal/src
	ADDON_SOURCES += src

	ADDON_SOURCES_EXCLUDE += libs/libseek-thermal/build/%
	#ADDON_SOURCES_EXCLUDE += libs/libseek-thermal/src/%.cpp
	ADDON_SOURCES_EXCLUDE += libs/libseek-thermal/cmake/%
	ADDON_SOURCES_EXCLUDE += libs/libseek-thermal/doc/%
	ADDON_SOURCES_EXCLUDE += libs/libseek-thermal/examples/%

osx:
	ADDON_LIBS += libs/libusb/lib/osx/usb-1.0.0.a
	#ADDON_LIBS += libs/libseek-thermal/build/src/libseek_static.a
