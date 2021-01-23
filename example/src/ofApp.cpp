#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
#ifdef CREATE_FLATFIELD
    cam.setCreateFlatfield(300, 80, FLATFIELD_DATA_PATH);
	cam.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
#else
//    cam.setup(OFX_SEEK_THERMAL_CAM_COMPACT, FLATFIELD_DATA_PATH);
    cam.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
#endif
    cam.setVerbose(false);
    
	img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
    rawImg.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofApp::update(){
	if(cam.isInitialized()){
		if(cam.isFrameNew()){
			img.setFromPixels(cam.getVisualizePixels());
            rawImg.setFromPixels(cam.getRawPixels());
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	img.draw(10, 10, img.getWidth()*2, img.getHeight()*2);
    rawImg.draw(10, 10+img.getHeight()*2, rawImg.getWidth()*2, rawImg.getHeight()*2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
