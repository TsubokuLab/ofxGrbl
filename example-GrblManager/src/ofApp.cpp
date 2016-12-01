#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	ofSetBackgroundColor(40);

	_font.loadFont("./GUI/NewMedia Fett.ttf", 20);

	grbl.setup();
	ofAddListener(grbl.UpDownEvent, this, &ofApp::UpDownHandler);
}

//--------------------------------------------------------------
void ofApp::update(){
	grbl.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
	ofBackground(0);

	grbl.draw();

	if (grbl.isDeviceReady) {
		ofSetColor(0, 255, 0);
		_font.drawString("[ Serial ] " + grbl.port + " @" + ofToString(grbl.baudrate) + " CONNECTED", ofGetWidth() / 4, 50);
	}
	else {
		ofSetColor(255, 0, 0);
		_font.drawString("[ Serial ] " + grbl.port + " @" + ofToString(grbl.baudrate) + " DISCONNECT", ofGetWidth() / 4, 50);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	grbl.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	string _message = "";
	switch (key) {
	case OF_KEY_RIGHT:
		grbl.moveRight(10);
		break;
	case OF_KEY_LEFT:
		grbl.moveLeft(10);
		break;
	case OF_KEY_UP:
		grbl.moveUp(10);
		break;
	case OF_KEY_DOWN:
		grbl.moveDown(10);
		break;
	case OF_KEY_RETURN:
		grbl.toggleVisible();
		break;
	case OF_KEY_DEL:
	case OF_KEY_BACKSPACE:
		if (!grbl.gui->isVisible()) grbl.resetCurrent();
		break;
	case OF_KEY_HOME:
		grbl.home();
		break;
	case ' ':
		grbl.isDrawMode = !grbl.isDrawMode;
		break;
	case 'f':
	case 'F':
		ofToggleFullscreen();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	grbl.keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
	if (grbl.gui->isVisible()) {
		grbl.mouseMoved(x, y);
	}
	else {
		if (y < ofGetHeight() - 100) {
			grbl.mouseMoved(x, y);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (grbl.gui->isVisible()) {
		grbl.mouseDragged(x, y, button);
	}
	else {
		if (y < ofGetHeight() - 100) {
			grbl.mouseDragged(x, y, button);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (grbl.gui->isVisible()) {
		grbl.mousePressed(x, y, button);
	}
	else {
		if (y < ofGetHeight() - 100) {
			grbl.mousePressed(x, y, button);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (grbl.gui->isVisible()) {
		grbl.mouseReleased(x, y, button);
	}
	else {
		if (y < ofGetHeight() - 100) {
			grbl.mouseReleased(x, y, button);
		}
	}
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
	grbl.dragEvent(dragInfo);
}

//--------------------------------------------------------------
void ofApp::UpDownHandler(bool &_isDown) {
	if (_isDown) {
		cout << "[ GrblManager ] DOWN" << endl;
	}
	else {
		cout << "[ GrblManager ] UP" << endl;
	}
	
}