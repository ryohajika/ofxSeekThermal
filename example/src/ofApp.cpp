#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	thermo.setup();
	buf.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){
	if(thermo.isInitialized()){
		thermo.update();
		if(thermo.isFrameNew()){
			ofPixels & pix = thermo.getPixels();
			buf.setFromPixels(pix);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	buf.draw(10, 10);
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
