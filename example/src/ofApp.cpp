#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	cam.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
	img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){
	if(cam.isInitialized()){
		if(cam.isFrameNew()){
			img.setFromPixels(cam.getVisualizePixels());
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	img.draw(10, 10);
}

//--------------------------------------------------------------
void ofApp::exit(){
	cam.close();
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
