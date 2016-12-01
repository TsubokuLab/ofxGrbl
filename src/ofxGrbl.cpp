#include "ofxGrbl.h"

//--------------------------------------------------------------
void ofxGrbl::setup() {
	cout << "[ ofxGrbl ] setup()" << endl;
	ofEnableAlphaBlending();
	readBuffer = "";

	// param
	isReadyToSend = true;
	isPause = false;
	isDrawMode = true;
	firstTimeLoad = true;

	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

	//baudrateList.push_back("9600");
	//baudrateList.push_back("19200");
	//baudrateList.push_back("38400");
	//baudrateList.push_back("57600");
	baudrateList.push_back("115200");

	_settings.MaxSpeed = ofVec3f(10000,10000,10000);
	_settings.Accel = ofVec3f(100, 100, 100);
	_settings.FeedbackInterval = 10;

	_settings.UpPos = 0;
	_settings.DownPos = -10.0f;

	port = "COM3";
	baudrate = 115200;

	initUI();
	Connect();

	//GRBL_WIDTH = 1800; // mm
	//GRBL_HEIGHT = 1200; // mm
	WINDOW_WIDTH = ofGetWidth();
	WINDOW_HEIGHT = ofGetHeight();

	color = ofColor::white;

	//ofSetWindowShape(GRBL_WIDTH, GRBL_HEIGHT);

	_fbo.allocate(WINDOW_WIDTH, WINDOW_HEIGHT);
}

//--------------------------------------------------------------
void ofxGrbl::update() {

	if (isConnect) {
		while (serial.available() > 0) {
			char _byte = (char)serial.readByte();
			if (_byte == '\n' || _byte == '\r') {
				if (readBuffer != "") {
					cout << "[ ofxGrblController ] [ RECEIVE ] " << readBuffer << endl;
					if (readBuffer == "ok") {
						isReadyToSend = true;
						//cout << "isReadyToSend !!" << endl;
					}
					if (readBuffer == "error: Unsupported command") {
						cout << "[ ofxGrblController ] [ PAUSED ]" << endl;
						isPause = true;
						isReadyToSend = true;
					}
					if (readBuffer[0] == '<') {
						// parse grbl state message
						vector<string> _status = ofSplitString(readBuffer, ",");
						vector<string> _posx = ofSplitString(_status[1], ":");
						vector<string> _posz = ofSplitString(_status[3], ">");
						cout << "[ ofxGrbl ] [ POSITION ] " << _posx[1] << ", " << _status[2] << ", " << _posz[0] << endl;
						currentPos = ofVec2f(ofToFloat(_posx[1]) / (float)GRBL_WIDTH, ofToFloat(_status[2]) / (float)GRBL_HEIGHT);

						if (ofToFloat(_posz[0]) < _settings.UpPos) {
							if (!isDown) {
								isDown = true;
								cout << "[ ofxGrbl ] DOWN" << endl;
								ofNotifyEvent(UpDownEvent, isDown);
							}
						}
						else {
							if (isDown) {
								isDown = false;
								cout << "[ ofxGrbl ] UP" << endl;
								ofNotifyEvent(UpDownEvent, isDown);
							}
						}
					}
					readBuffer = "";
				}
			}
			else {
				readBuffer += _byte;
			}
		}
	}

	// send
	if (isReadyToSend && !isPause) {
		if (sendQueList.size()>0) {
			sendMessage(sendQueList[0], true);

			sendQueList.erase(sendQueList.begin());
			cout << "[ ofxGrblController ] [ QUE ] " << sendQueList.size() << endl;

			isReadyToSend = false;
		}
	}
	else {
		//cout << "waiting..." << endl;
	}

	// get status
	if (isConnect) {
		if (ofGetFrameNum() % (int)_settings.FeedbackInterval == 0) sendMessage("?", true);
	}

	ofxUIImageToggle *toggle = (ofxUIImageToggle *)gui->getWidget("PAUSE");
	toggle->setValue(isPause);
}

//--------------------------------------------------------------
void ofxGrbl::draw() {
	_fbo.begin();
	ofBackground(40);
	ofSetLineWidth(3);

	if (ofGetMousePressed(0)) {
		//ofSetColor(ofColor::fromHsb(int((float)ofGetFrameNum() / 2.0f) % 255, 255, 255));
		ofSetColor(color);
		ofDrawCircle((float)ofGetMouseX() / ofGetWidth() * WINDOW_WIDTH, (float)ofGetMouseY() / ofGetHeight() * WINDOW_HEIGHT, 15);
		ofSetColor(0, 255, 0);
		ofDrawBitmapString("X:" + ofToString(ofGetMouseX() / (float)ofGetWidth() * GRBL_WIDTH) + "\nY:" + ofToString(ofGetMouseY() / (float)ofGetHeight() * GRBL_HEIGHT), (float)ofGetMouseX() / ofGetWidth() * WINDOW_WIDTH + 20, (float)ofGetMouseY() / ofGetHeight() * WINDOW_HEIGHT + 20);
	}

	for (int i = 0; i < strokeList.size(); i++) {
		ofPath line = ofPath();
		line.setStrokeColor(ofColor(255, 255, 255));
		line.setFilled(false);
		line.setStrokeWidth(3);
		line.moveTo(strokeList[i][0].x * WINDOW_WIDTH, strokeList[i][0].y * WINDOW_HEIGHT);
		for (int j = 1; j < strokeList[i].size(); j++) {
			line.lineTo(strokeList[i][j].x * WINDOW_WIDTH, strokeList[i][j].y * WINDOW_HEIGHT);
		}
		line.draw();
		line.close();
	}

	if (tmpStroke.size() > 0) {
		ofPath line = ofPath();
		line.setStrokeColor(color);
		line.setFilled(false);
		line.setStrokeWidth(3);
		line.moveTo(tmpStroke[0].x * WINDOW_WIDTH, tmpStroke[0].y * WINDOW_HEIGHT);
		for (int i = 1; i < tmpStroke.size(); i++) {
			line.lineTo(tmpStroke[i].x * WINDOW_WIDTH, tmpStroke[i].y * WINDOW_HEIGHT);
		}
		line.draw();
		line.close();
	}

	ofSetColor(255, 0, 0);

	ofDrawCircle(currentPos.x * WINDOW_WIDTH, currentPos.y * WINDOW_HEIGHT, 20);
	ofDrawBitmapString("X:" + ofToString(currentPos.x, 3) + "\nY:" + ofToString(currentPos.y, 3), currentPos.x * WINDOW_WIDTH + 20, currentPos.y * WINDOW_HEIGHT + 20);
	
	// mousePos
	if (isDrawMode) {
		ofSetColor(0, 255, 0);
		ofDrawBitmapString("[ Draw Mode ]", targetPos.x * WINDOW_WIDTH + 20, targetPos.y * WINDOW_HEIGHT);
	}
	else {
		ofSetColor(255, 100, 0);
		ofDrawBitmapString("[ Move Mode ]", targetPos.x * WINDOW_WIDTH + 20, targetPos.y * WINDOW_HEIGHT);
	}
	ofDrawLine(targetPos.x * WINDOW_WIDTH - 10, targetPos.y * WINDOW_HEIGHT, targetPos.x * WINDOW_WIDTH + 10, targetPos.y * WINDOW_HEIGHT);
	ofDrawLine(targetPos.x * WINDOW_WIDTH, targetPos.y * WINDOW_HEIGHT - 10, targetPos.x * WINDOW_WIDTH, targetPos.y * WINDOW_HEIGHT + 10);
	ofDrawBitmapString("X:" + ofToString(targetPos.x, 3) + "\nY:" + ofToString(targetPos.y, 3), targetPos.x * WINDOW_WIDTH + 20, targetPos.y * WINDOW_HEIGHT + 20);

	_fbo.end();

	ofSetColor(255, 255, 255);
	_fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofxGrbl::close() {
	if (isConnect) sendMessage("G90 G0 X0 Y0 Z0", true);
}

//--------------------------------------------------------------
void ofxGrbl::keyPressed(int key) {
	string _message = "";
	switch (key) {
	case OF_KEY_RIGHT:
		sendMessage("G91 G1 X10 Y0 Z0", true);
		break;
	case OF_KEY_LEFT:
		sendMessage("G91 G1 X-10 Y0 Z0", true);
		break;
	case OF_KEY_UP:
		sendMessage("G91 G1 X0 Y10 Z0", true);
		break;
	case OF_KEY_DOWN:
		sendMessage("G91 G1 X0 Y-10 Z0", true);
		break;
	case OF_KEY_RETURN:
		gui->toggleVisible();
		break;
	case OF_KEY_DEL:
	case OF_KEY_BACKSPACE:
		if(!gui->isVisible()) resetCurrent();
		break;
	case OF_KEY_HOME:
		home();
		break;
	case ' ':
		isDrawMode = !isDrawMode;
		break;
	case 'f':
	case 'F':
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofxGrbl::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofxGrbl::mouseMoved(int x, int y) {
	
	if (x < 0)x = 0;
	if (x > ofGetWidth()) x = ofGetWidth();
	if (y < 0)y = 0;
	if (y > ofGetHeight()) y = ofGetHeight();
	targetPos = ofVec2f((float)x / (float)ofGetWidth(), (float)y / (float)ofGetHeight());

	if (gui->isVisible()) return;
	if (isDrawMode == false) {
		sendMessage(vec2fToGcode(targetPos));
	}
}

//--------------------------------------------------------------
void ofxGrbl::mouseDragged(int x, int y, int button) {
	if (gui->isVisible()) return;
	if (x < 0)x = 0;
	if (x > ofGetWidth()) x = ofGetWidth();
	if (y < 0)y = 0;
	if (y > ofGetHeight()) y = ofGetHeight();

	targetPos = ofVec2f(x / (float)ofGetWidth(), y / (float)ofGetHeight());

	// 最低移動距離
	if (ofDist(prevPos.x, prevPos.y, targetPos.x, targetPos.y) < 0.001f) {
		cout << ofDist(prevPos.x, prevPos.y, targetPos.x, targetPos.y) << endl;
		return;
	}

	//if (isDrawMode) {
		tmpStroke.push_back(targetPos);
	//}
	sendMessage(vec2fToGcode(targetPos));

	prevPos = targetPos;
}

//--------------------------------------------------------------
void ofxGrbl::mousePressed(int x, int y, int button) {
	if (gui->isVisible()) return;
	tmpStroke.clear();
	ofVec2f _tmpVec2 = ofVec2f(x / (float)ofGetWidth(), y / (float)ofGetHeight());
	//if (isDrawMode) {
		tmpStroke.push_back(_tmpVec2);
	//}
	sendMessage(vec2fToGcode(_tmpVec2));

	//if (isDrawMode) {
		if (_settings.isUseZAxis) sendMessage("G1 Z" + ofToString(_settings.DownPos, 2));
	//}
}

//--------------------------------------------------------------
void ofxGrbl::mouseReleased(int x, int y, int button) {
	if (gui->isVisible()) return;
	ofVec2f _tmpVec2 = ofVec2f(x / (float)ofGetWidth(), y / (float)ofGetHeight());
	//if (isDrawMode) {
		tmpStroke.push_back(_tmpVec2);
		if (tmpStroke.size() > 2) {
			strokeList.push_back(tmpStroke);
		}
	//}
	tmpStroke.clear();

	sendMessage(vec2fToGcode(_tmpVec2));

	//if (isDrawMode) {
		if (_settings.isUseZAxis) sendMessage("G1 Z" + ofToString(_settings.UpPos, 2));
	//}
}

//--------------------------------------------------------------
void ofxGrbl::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofxGrbl::dragEvent(ofDragInfo dragInfo) {
	for (int i = 0; i < dragInfo.files.size(); i++) {
		loadFromFile(ofToDataPath(dragInfo.files[i]));
	}
}

//--------------------------------------------------------------
void ofxGrbl::loadFromFile(string _path) {
	cout << "[ ofxGrblController ] loadFromFile( " << _path << " )" << endl;
	string _text = string(ofBufferFromFile(_path));
	vector<string> _linelist = ofSplitString(_text, "\n", true);
	cout << "[ ofxGrblController ] loadFromFile() : " << _linelist.size() << " lines." << endl;

	vector<ofVec2f> _tmpVec;
	bool isDown = false;
	for (int i = 0; i < _linelist.size(); i++) {

		// Zの状態で
		if (_linelist[i].find('Z') != string::npos) {
			if (checkZisDown(_linelist[i])) {
				_tmpVec.clear();
			}
			else {
				if (_tmpVec.size() > 1) strokeList.push_back(_tmpVec);
			}
		}

		if (checkXYCommand(_linelist[i])) {
			cout << "[ ofxGrblController ] Command added : " << _linelist[i] << endl;
			_tmpVec.push_back(gcodeToVec2f(_linelist[i]));
		}
		else {
			cout << "[ ofxGrblController ] Invalid command : " << _linelist[i] << endl;
		}
		sendMessage(_linelist[i]);
	}
}

//--------------------------------------------------------------
void ofxGrbl::sendMessage(string _msg, bool direct) {
	if (direct) {
		if (isConnect) {
			if (_msg != "") {
				string _message = _msg + "\n";
				unsigned char* writeByte = (unsigned char*)_message.c_str();
				serial.writeBytes(writeByte, _message.length());
				cout << "[ ofxGrblController ] sendMessage( " << _message << " )";
			}
			else {
				cout << "[ ofxGrblController ] sendMessage() : Message is empty." << endl;
			}
		}
		else {
			cout << "[ ofxGrblController ] sendMessage() : Serial is not connected." << endl;
		}
	}
	else {
		if (_msg != "") {
			string _message = _msg + "\n";
			sendQueList.push_back(_message);
		}
	}
}

bool ofxGrbl::checkXYCommand(string _line) {
	return (_line.find('X') != string::npos) || (_line.find('Y') != string::npos);
}

bool ofxGrbl::checkZisDown(string _line) {
	float _zPos;
	vector<string> _commands = ofSplitString(_line, " ", true);
	for (int i = 0; i < _commands.size(); i++) {
		if (_commands[i][0] == 'Z') {
			if (_commands[i].size() == 1) {
				//cout << "Zがスペース区切り" << endl;
				//「Z 99.00」のようにスペース区切りの時
				_zPos = ofToFloat(_commands[i + 1]); //次の文字を使う
			}
			else {
				//cout << "zがつながってる" << endl;
				//「Z99.00」のように表記がつながっている時
				_zPos = ofToFloat(_commands[i].substr(1));
			}
		}
	}

	return (_zPos < (_settings.DownPos + _settings.UpPos) / 2);
}

ofVec2f ofxGrbl::gcodeToVec2f(string _line) {
	bool _isXYEnable = false;
	ofVec2f _result = ofVec2f::zero();
	vector<string> _commands = ofSplitString(_line, " ", true);
	for (int i = 0; i < _commands.size(); i++) {
		if (_commands[i][0] == 'X') {
			if (_commands[i].size() == 1) {
				//cout << "xがスペース区切り" << endl;
				//「X 99.00」のようにスペース区切りの時
				_result.x = ofToFloat(_commands[i + 1]) / GRBL_WIDTH; //次の文字を使う
			}
			else {
				//cout << "xがつながってる" << endl;
				//「X99.00」のように表記がつながっている時
				_result.x = ofToFloat(_commands[i].substr(1)) / GRBL_WIDTH;
			}
			_isXYEnable = true;
		}
		else if (_commands[i][0] == 'Y') {
			if (_commands[i].size() == 1) {
				//cout << "yがスペース区切り" << endl;
				//「X 99.00」のようにスペース区切りの時
				_result.y = ofToFloat(_commands[i + 1]) / GRBL_HEIGHT; //次の文字を使う
			}
			else {
				//cout << "yがつながってる" << endl;
				//「X99.00」のように表記がつながっている時
				_result.y = ofToFloat(_commands[i].substr(1)) / GRBL_HEIGHT;
			}
			_isXYEnable = true;
		}
	}

	return _result;
}

string ofxGrbl::vec2fToGcode(ofVec2f _vec) {
	string _message;
	_message = "G90 G1 X" + ofToString(_vec.x * GRBL_WIDTH, 2) + " Y" + ofToString(_vec.y * GRBL_HEIGHT, 2);
	return _message;
}

//--------------------------------------------------------------
void ofxGrbl::initUI() {
	cout << "initUI()" << endl;

	float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float length = 280 - xInit;

	gui = new ofxUICanvas(0, 5, length + xInit, ofGetHeight());
	gui->setFont("./GUI/NewMedia Fett.ttf");
	gui->addWidgetDown(new ofxUILabel("[ Grbl Manager ]", OFX_UI_FONT_LARGE));
	gui->addSpacer(length - xInit, 2);

	gui->addWidgetDown(new ofxUILabel("GRBL SETTINGS", OFX_UI_FONT_MEDIUM));
	gui->addSlider("MAX SPEED X", 1, 20000, &_settings.MaxSpeed.x);
	gui->addSlider("MAX SPEED Y", 1, 20000, &_settings.MaxSpeed.y);
	gui->addSlider("MAX SPEED Z", 1, 20000, &_settings.MaxSpeed.z);
	gui->addSlider("ACCEL X", 10.0, 500.0, &_settings.Accel.x);
	gui->addSlider("ACCEL Y", 10.0, 500.0, &_settings.Accel.y);
	gui->addSlider("ACCEL Z", 10.0, 500.0, &_settings.Accel.z);
	gui->addSlider("MAX TRAVEL X", 100.0, 2000.0, &_settings.MaxTravel.x);
	gui->addSlider("MAX TRAVEL Y", 100.0, 2000.0, &_settings.MaxTravel.y);
	gui->addSlider("MAX TRAVEL Z", 100.0, 2000.0, &_settings.MaxTravel.z);
	
	//gui->add2DPad("HOME POSITION", ofVec3f(0, 1), ofVec3f(0, 1), &_settings.HomePosition, length - xInit, 120);

	gui->addToggle("Z AXIS", &_settings.isUseZAxis);
	gui->addSlider("UP POS", -50.0, 50.0, &_settings.UpPos);
	gui->addSlider("DOWN POS", -50.0, 50.0, &_settings.DownPos);
	gui->addSlider("FEEDBACK INTERVAL", 1, 300, &_settings.FeedbackInterval);
	
	gui->addLabelButton("SET", false);

	gui->addSpacer(length - xInit, 2);
	gui->addWidgetDown(new ofxUILabel("SERIAL SETTINGS", OFX_UI_FONT_MEDIUM));
	gui->addLabel("PORT_LABEL", "PORT NAME");
	gui->addTextInput("PORT", "COM3");
	//gui->addLabel("BAUDRATE_LABEL", "BAUDRATE");
	gui->addRadio("BAUDRATE", baudrateList);
	//gui->addLabel("SEND MESSAGE");
	//gui->addTextInput("INPUT", "$$");
	//gui->addLabelButton("CONNECT SERIAL", false);
	//gui->addLabelButton("SERIAL SEND", false);
	//gui->addLabelButton("GET INFO", false);

	//gui->addSpacer(length - xInit, 2);
	//gui->addWidgetDown(new ofxUILabel("UDP SETTINGS", OFX_UI_FONT_MEDIUM));
	//gui->addLabel("UDP_IP_LABEL", "UDP_IP");
	//gui->addTextInput("UDP_IP", "192.168.0.2");
	//gui->addLabel("UDP_PORT_LABEL", "UDP_PORT");
	//gui->addTextInput("UDP_PORT", "20000");
	//gui->addLabelButton("CONNECT UDP", false);
	//gui->addToggle("IS UDP CONNECTED", &isUdpConnected);

	gui->addSpacer(length - xInit, 2);
	gui->addWidgetDown(new ofxUILabel("CONTROL", OFX_UI_FONT_MEDIUM));
	//gui->addToggle("IS_READY_TO_SEND", &isReadyToSend);
	gui->addImageButton("HOME", "./GUI/icon/fa-home.png", false, 30, 30);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	gui->addImageButton("PLAY", "./GUI/icon/fa-play-circle-o.png", false, 30, 30);
	gui->addImageToggle("PAUSE", "./GUI/icon/fa-pause-circle-o.png", &isPause, 30, 30);
	gui->addImageButton("STOP", "./GUI/icon/fa-stop-circle-o.png", false, 30, 30);
	gui->addImageButton("OPEN", "./GUI/icon/fa-folder-open.png", false, 30, 30);
	gui->addImageButton("TRASH", "./GUI/icon/fa-trash.png", false, 30, 30);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

	//gui->addSpacer(length - xInit, 2);
	//gui->addLabelButton("RESET", false, length / 2 - xInit);
	//gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	//gui->addLabelButton("DRAW", false, length / 2 - xInit * 2);
	//gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

	gui->addSpacer(length - xInit, 2);
	gui->addLabelButton("SAVE", false);
	gui->addLabelButton("LOAD", false);


	gui->setTheme(30);
	ofAddListener(gui->newGUIEvent, this, &ofxGrbl::guiEvent);

	loadSettings();
}

//--------------------------------------------------------------
void ofxGrbl::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//cout << "got event from: " << name << " kind:" << ofToString(kind) << endl;

	if (name == "UP POS") {
		if (_settings.UpPos < _settings.DownPos) _settings.DownPos = _settings.UpPos;
		if (_settings.isUseZAxis && sendQueList.size() == 0) sendMessage("G90 G1 Z" + ofToString(_settings.UpPos, 2));
	}
	else if (name == "DOWN POS") {
		if (_settings.DownPos > _settings.UpPos) _settings.UpPos = _settings.DownPos;
		if (_settings.isUseZAxis && sendQueList.size() == 0) sendMessage("G90 G1 Z" + ofToString(_settings.DownPos, 2));
	}
	else if (name == "PORT") {
		ofxUITextInput *input = (ofxUITextInput *)e.widget;
		port = input->getTextString();
	}
	else if (name == "BAUDRATE") {
		ofxUIRadio *radio = (ofxUIRadio *)e.widget;
		baudrate = ofToInt(radio->getActiveName());
	}
	else if (name == "CONNECT SERIAL") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			Connect();
		}
	}
	else if (name == "SERIAL SEND") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			ofxUITextInput *input = (ofxUITextInput *)gui->getWidget("INPUT");
			sendMessage(input->getTextString(), true);
		}
	}
	else if (name == "GET INFO") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			sendMessage("$$", true);
		}
	}
	else if (name == "GET INFO") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			sendMessage("$$", true);
		}
	}
	else if (name == "SET") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			setSettings();
		}
	}

	else if (name == "HOME") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			home();
		}
	}
	else if (name == "PLAY") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			if (sendQueList.size() == 0) {
				drawCurrent();
			}
			isPause = false;
		}
	}
	else if (name == "STOP") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			resetCurrent();
		}
	}
	else if (name == "OPEN") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a GCODE(.gcode .nc .ngc) file");
			loadFromFile(openFileResult.getPath());
		}
	}
	else if (name == "TRASH") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			resetCurrent();
		}
	}
	else if (name == "DRAW") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			drawCurrent();
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
void ofxGrbl::saveSettings() {
	cout << "[ ofxGrbl ] saveSettings" << endl;
	gui->saveSettings("./GUI/guiSettings.xml");

	string path = "./strokeList.ngc";
	saveCurrent(path);
}
//--------------------------------------------------------------
void ofxGrbl::loadSettings() {
	cout << "[ ofxGrbl ] loadSettings" << endl;
	gui->loadSettings("./GUI/guiSettings.xml");

	setSettings();

	if (firstTimeLoad) {
		cout << "[ ofxGrbl ] FirstTimeLoad!" << endl;
		firstTimeLoad = false;
		currentPos = targetPos = prevPos = ofVec3f(_settings.HomePosition.x  * _settings.MaxTravel.x, _settings.HomePosition.y  * _settings.MaxTravel.y, _settings.HomePosition.z  * _settings.MaxTravel.z);

	}
	//string path = "./strokeList.ngc";
	//loadFromFile(path);
}

void ofxGrbl::drawCurrent() {
	for (int i = 0; i < strokeList.size(); i++) {


		for (int j = 0; j < strokeList[i].size(); j++) {
			sendMessage(vec2fToGcode(strokeList[i][j]));
			if (j == 0) {
				if (_settings.isUseZAxis) sendMessage("G1 Z" + ofToString(_settings.DownPos, 2));
			}
		}

		if (_settings.isUseZAxis) sendMessage("G1 Z" + ofToString(_settings.UpPos, 2));
	}
	home();
}

void ofxGrbl::saveCurrent(string _path) {
	string output;

	for (int i = 0; i < strokeList.size(); i++) {


		for (int j = 0; j < strokeList[i].size(); j++) {
			output += vec2fToGcode(strokeList[i][j]) + "\n";
			if (j == 0) {
				if (_settings.isUseZAxis) output += "G1 Z" + ofToString(_settings.DownPos, 2) + "\n";
			}
			output += vec2fToGcode(strokeList[i][j]) + "\n";
		}

		if (_settings.isUseZAxis) output += "G1 Z" + ofToString(_settings.UpPos, 2) + "\n";
	}
	output += "G90 G0 X0 Y0 Z0\n";


	//	string path = "./" + ofToString(ofGetYear()) + "-" + ofToString(ofGetMonth()) + "-" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "-" + ofToString(ofGetMinutes()) + "-" + ofToString(ofGetSeconds()) + ".ngc";
	string fileName = _path;
	ofBuffer buffer = ofBuffer(output);
	ofBufferToFile(fileName, buffer);
}

void ofxGrbl::resetCurrent() {
	strokeList.clear();
	sendQueList.clear();
}

void ofxGrbl::home() {
	sendMessage("G90 G0 X" + ofToString(_settings.HomePosition.x) + " Y" + ofToString(_settings.HomePosition.y) + " Z" + ofToString(_settings.HomePosition.z), true);
}

void ofxGrbl::Connect(string _port, int _baudrate) {
	if (_port == "") _port = port;
	if (_baudrate <= 0) _baudrate = baudrate;
	cout << "Connect( " << _port << ", " << _baudrate << " )" << endl;

	isConnect = serial.setup(_port, _baudrate);
	if (isConnect) {
		int waitCount = 10000;
		while (!serial.available() && waitCount > 0) {
			cout << "[ waiting ]" << waitCount << endl;
			waitCount--;
		}
		if (waitCount == 0) {
			isConnect = false;
			cout << "[ timeout ]" << _port << " is busy." << endl;
		}
		else {
			isConnect = true;
		}
	}
	else {
		cout << _port << " is not exists." << endl;
		isConnect = false;
	}
}

void ofxGrbl::setDrawMode() {
	isDrawMode = true;
}
void ofxGrbl::setMoveMode() {
	isDrawMode = false;
}
void ofxGrbl::setColor(ofColor _color) {
	cout << "[ ofxGrbl ] SetColor() : R" << (int)_color.r << " G" << (int)_color.g << " B" << (int)_color.b << " A" << (int)_color.a << endl;
	color = _color;
}


void ofxGrbl::setSettings() {
	// スピード設定
	sendMessage("F" + ofToString(_settings.MaxSpeed.x, 2));
	sendMessage("$110=" + ofToString(_settings.MaxSpeed.x, 2));
	sendMessage("$111=" + ofToString(_settings.MaxSpeed.y, 2));
	sendMessage("$112=" + ofToString(_settings.MaxSpeed.z, 2));

	sendMessage("$120=" + ofToString(_settings.Accel.x, 2));
	sendMessage("$121=" + ofToString(_settings.Accel.y, 2));
	sendMessage("$122=" + ofToString(_settings.Accel.z, 2));

	sendMessage("$130=" + ofToString(_settings.MaxTravel.x, 2));
	sendMessage("$131=" + ofToString(_settings.MaxTravel.y, 2));
	sendMessage("$132=" + ofToString(_settings.MaxTravel.z, 2));

	setArea(_settings.MaxTravel.x, _settings.MaxTravel.y);
}

void ofxGrbl::setArea(float x, float y) {
	cout << "[ ofxGrbl ] setArea(" << (int)x << ", " << (int)y << ")" << endl;
	GRBL_WIDTH = x;
	GRBL_HEIGHT = y;

	ofSetWindowShape(GRBL_WIDTH, GRBL_HEIGHT);
	_fbo.allocate(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void ofxGrbl::setHome(float x, float y, float z) {
	cout << "[ ofxGrbl ] setArea(" << (int)x << ", " << (int)y << ")" << endl;
	_settings.HomePosition = ofVec3f(x, y, z);
}
void ofxGrbl::setHome(ofVec3f _homePos) {
	_settings.HomePosition = ofVec3f(_homePos.x, _homePos.y, _homePos.z);
}