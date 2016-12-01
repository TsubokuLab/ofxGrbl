#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	ofSetBackgroundColor(40);

	udp_ip = "192.168.0.2";
	udp_port = 20000;

	isUdpBind = false;
	isUdpConnected = false;

	_font.loadFont("./GUI/NewMedia Fett.ttf", 20);

	sendInterval = 0.2f;

	offsetXY = ofVec3f::zero();

	grbl.setup();
	ofAddListener(grbl.UpDownEvent, this, &ofApp::UpDownHandler);

	initUI();
	ConnectUdp();
}

//--------------------------------------------------------------
void ofApp::update(){
	grbl.update();

	if (isUdpBind) {
		char _udpMessage[100000];
		if (udpReceiver.Receive(_udpMessage, 100000) > 0) {
			string message = _udpMessage;
			cout << "[Receive] " << message << endl;
			vector<string> strPoints = ofSplitString(message, ";");
			for (int i = 0; i < strPoints.size(); i++) {
				if (strPoints[i] == "PING") {
					string _pong = "PONG;";
					sendMessage(_pong);
				}
				if (strPoints[i] == "PONG") {
					isUdpConnected = true;
				}
				
				vector<string> _commands = ofSplitString(strPoints[i], "/");
				if (_commands.size() > 0) {
					if (_commands[0] == "BRUSH") {
						grbl.targetPos = ofVec3f(ofToFloat(_commands[1]), ofToFloat(_commands[2]), 0);
					}
				}
			}
		}
	}
	else {
		cout << "udpReceiver is not bind." << endl;
	}

	if (isUdpConnected) {
		if (ofGetElapsedTimef() - prevSentTime > sendInterval) {
			string _message = "BRUSH/" + ofToString(grbl.currentPos.x + offsetXY.x) + "/" + ofToString(grbl.currentPos.y + offsetXY.y) + "/MOVE;";
			sendMessage(_message);
			prevSentTime = ofGetElapsedTimef();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
	ofBackground(0);

	grbl.draw();

	ofColor _c = ofColor::white;
	int span = 256;
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < span; i++) {
			switch (j)
			{
			case 0:
				_c.setHsb((float)i / span * 255, 0, 255);
				break;
			case 1:
				_c.setHsb((float)i / span * 255, 127, 255);
				break;
			case 2:
				_c.setHsb((float)i / span * 255, 255, 255);
				break;
			case 3:
				_c.setHsb((float)i / span * 255, 255, 127);
				break;
			case 4:
				_c.setHsb((float)i / span * 255, 255, 0);
				break;
			default:
				break;
			}

			ofSetColor(_c);
			ofDrawRectangle((float)ofGetWidth() / span * i, ofGetHeight() - 100 + 20*(j), (float)ofGetWidth() / span, 20);
		}
	}

	if (isUdpConnected) {
		ofSetColor(0,255,0);
		_font.drawString("[ UDP ] " + udp_ip + " : " + ofToString(udp_port) + " CONNECTED", ofGetWidth() / 4, 60);
	}
	else {
		ofSetColor(255, 0, 0);
		_font.drawString("[ UDP ] " + udp_ip + " : " + ofToString(udp_port) + " DISCONNECT", ofGetWidth() / 4, 60);
	}

	if (grbl.isConnect) {
		ofSetColor(0, 255, 0);
		_font.drawString("[ Serial ] " + grbl.port + " @" + ofToString(grbl.baudrate) + " CONNECTED", ofGetWidth() / 4, 100);
	}
	else {
		ofSetColor(255, 0, 0);
		_font.drawString("[ Serial ] " + grbl.port + " @" + ofToString(grbl.baudrate) + " DISCONNECT", ofGetWidth() / 4, 100);
	}
	
	if (ofGetMousePressed(0) && !gui->isVisible() && mouseY > ofGetHeight() - 100){
		ofColor _c;
		unsigned char pixel[3];
		glReadPixels(mouseX, ofGetHeight() - mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		std::cout << "R: " << (int)pixel[0] << std::endl;
		std::cout << "G: " << (int)pixel[1] << std::endl;
		std::cout << "B: " << (int)pixel[2] << std::endl;
		std::cout << std::endl;
		_c.set((int)pixel[0], (int)pixel[1], (int)pixel[2]);
		changeColor(_c);

		ofSetColor(_c);
		ofDrawCircle(mouseX, mouseY, 50);
	}

}

//--------------------------------------------------------------
void ofApp::exit() {
	grbl.close();
	if(isUdpBind) udp.Close();
	if(isUdpConnected)udpReceiver.Close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	string _message = "";
	switch (key) {
	case OF_KEY_RETURN:
		gui->toggleVisible();
		break;
	case 'f':
	case 'F':
		ofToggleFullscreen();
		break;
	case OF_KEY_F1:
		grbl.saveSettings();
		saveSettings();
		break;
	case OF_KEY_F2:
		grbl.loadSettings();
		loadSettings();
		break;
	case OF_KEY_DEL:
	case OF_KEY_BACKSPACE:
		if (!gui->isVisible()) {
			string _message;
			_message = "BRUSH/DELETE;";
			sendMessage(_message);
		}
		break;
	}

	grbl.keyPressed(key);
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

	if (!grbl.gui->isVisible() && y > ofGetHeight() - 100) {
		ofColor _c;
		unsigned char pixel[3];
		glReadPixels(x, ofGetHeight() - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		std::cout << "R: " << (int)pixel[0] << std::endl;
		std::cout << "G: " << (int)pixel[1] << std::endl;
		std::cout << "B: " << (int)pixel[2] << std::endl;
		std::cout << std::endl;
		_c.set((int)pixel[0], (int)pixel[1], (int)pixel[2]);
		changeColor(_c);
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
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float length = 280 - xInit;
	if(gui)gui->setPosition(w - length + xInit, 5);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	grbl.dragEvent(dragInfo);
}

//--------------------------------------------------------------
void ofApp::initUI() {
	cout << "initUI()" << endl;

	float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float length = 280 - xInit;

	gui = new ofxUICanvas(ofGetWidth() - length + xInit, 5, length + xInit, ofGetHeight());
	gui->setFont("./GUI/NewMedia Fett.ttf");
	gui->addWidgetDown(new ofxUILabel("[ UDP Settings ]", OFX_UI_FONT_LARGE));

	gui->addSpacer(length - xInit, 2);
	gui->addWidgetDown(new ofxUILabel("UDP SETTINGS", OFX_UI_FONT_MEDIUM));
	gui->addToggle("IS SEND UDP", &isSendUdp);
	gui->addLabel("UDP_IP_LABEL", "UDP_IP");
	gui->addTextInput("UDP_IP", "192.168.0.2");
	gui->addLabel("UDP_PORT_LABEL", "UDP_PORT");
	gui->addTextInput("UDP_PORT", "20000");
	gui->addSlider("SEND INTERVAL", 0.01f, 1.0f, &sendInterval);
	gui->addLabelButton("CONNECT UDP", false);

	gui->add2DPad("OFFSET", ofxUIVec3f(-0.5f, 0.5f), ofxUIVec3f(-0.5f, 0.5f), &offsetXY);
	//gui->addToggle("IS UDP CONNECTED", &isUdpConnected);

	gui->addSpacer(length - xInit, 2);
	gui->addLabelButton("SAVE", false);
	gui->addLabelButton("LOAD", false);

	gui->setTheme(30);
	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);

	loadSettings();
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//cout << "got event from: " << name << " kind:" << ofToString(kind) << endl;
	
	if (name == "UDP_IP") {
		ofxUITextInput *input = (ofxUITextInput *)e.widget;
		udp_ip = input->getTextString();
	}
	else if (name == "UDP_PORT") {
		ofxUITextInput *input = (ofxUITextInput *)e.widget;
		udp_port = ofToInt(input->getTextString());
	}
	else if (name == "CONNECT UDP") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			ConnectUdp();
		}
	}
	else if (name == "SAVE") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			saveSettings();
		}
	}
	else if (name == "LOAD") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			loadSettings();
		}
	}

}

//--------------------------------------------------------------
void ofApp::saveSettings() {
	cout << "[ ofApp ] saveSettings" << endl;
	gui->saveSettings("./GUI/udpSettings.xml");
}
//--------------------------------------------------------------
void ofApp::loadSettings() {
	cout << "[ ofApp ] loadSettings" << endl;
	gui->loadSettings("./GUI/udpSettings.xml");
}

void ofApp::ConnectUdp(string _udp_ip, int _udp_port) {
	if (_udp_ip == "") _udp_ip = udp_ip;
	if (_udp_port <= 0) _udp_port = udp_port;
	cout << "ConnectUdp( " << _udp_ip << "," << _udp_port << " )" << endl;

	udp.Create();

	udpReceiver.Create();
	isUdpBind = udpReceiver.Bind(_udp_port);
	udpReceiver.SetNonBlocking(true);
	
	if (udp.Connect(_udp_ip.c_str(), _udp_port)) {
		udp.SetNonBlocking(true);
		string _ping = "PING;";
		sendMessage(_ping);
		isUdpConnected = true;
	}
	else {
		cout << "Udp connect failed." << endl;
	}
	
	
}

void ofApp::sendMessage(string _message) {
	if (isUdpConnected) {
		if (isSendUdp) {
			cout << "[ sendMessage ] SEND >> " << _message << endl;
			udp.Send(_message.c_str(), _message.size());
		}
	}
	else {
		cout << "[ sendMessage ] UDP is not connect." << endl;
	}
}

void ofApp::UpDownHandler(bool &_isDown) {
	cout << "[ ofApp ] UpDownHandler : " << ofToString(_isDown);
	string _message;
	if (_isDown) {
		_message = "BRUSH/" + ofToString(grbl.currentPos.x + offsetXY.x) + "/" + ofToString(grbl.currentPos.y + offsetXY.y) + "/DOWN;";
	}
	else {
		_message = "BRUSH/" + ofToString(grbl.currentPos.x + offsetXY.x) + "/" + ofToString(grbl.currentPos.y + offsetXY.y) + "/UP;";
	}
	sendMessage(_message);
}

void ofApp::changeColor(ofColor _color) {
	grbl.setColor(_color);

	string _message;
	_message = "BRUSH/COLOR/" + ofToString(int(_color.r)) + "/" + ofToString(int(_color.g)) + "/" + ofToString(int(_color.b)) + ";";
	sendMessage(_message);
}