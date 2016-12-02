#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	grbl.setup();
	grbl.Connect("COM3", 115200);

	// set position directly
	grbl.setPosition(300, 100);
	grbl.setPosition(900, 100);
	grbl.setPosition(900, 600);
	grbl.setPosition(300, 600);
	grbl.setPosition(300, 100);

	// move from stroke file
	grbl.loadFromFile("./star.gcode");

}

//--------------------------------------------------------------
void ofApp::update(){
	grbl.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	grbl.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{
	case OF_KEY_RETURN:
		grbl.toggleVisible();
		break;
	case OF_KEY_RIGHT:
		grbl.moveRight(10);
		break;
	case OF_KEY_LEFT:
		grbl.moveLeft(10);
		break;
	case OF_KEY_UP:
		grbl.moveDown(10);
		break;
	case OF_KEY_DOWN:
		grbl.moveUp(10);
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	grbl.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	grbl.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	grbl.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	grbl.mouseReleased(x, y, button);
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
