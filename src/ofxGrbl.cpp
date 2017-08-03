#include "ofxGrbl.h"

//--------------------------------------------------------------
void ofxGrbl::setup() {
	cout << "[ ofxGrbl ] setup()" << endl;
	ofEnableAlphaBlending();
	readBuffer = "";
	status = "";

	// param
	isReadyToSend = true;
	isPause = false;
	isDrawMode = true;
	firstTimeLoad = true;

	// fbo
	WINDOW_WIDTH = ofGetWidth();
	WINDOW_HEIGHT = ofGetHeight();
	_fbo.allocate(WINDOW_WIDTH, WINDOW_HEIGHT);

	// serial
	serial.getDeviceList();
	isConnect = false;
	isDeviceReady = false;
	port = "COM3";
	baudrate = 115200;

	// settings
	//baudrateList.push_back("9600");
	//baudrateList.push_back("19200");
	//baudrateList.push_back("38400");
	//baudrateList.push_back("57600");
	baudrateList.push_back("115200");

	// mode
	modeList.push_back("Spindle");
	modeList.push_back("Laser");
	modeList.push_back("Plotter");

	_settings.MaxSpeed = ofVec3f(10000,10000,10000);
	_settings.Accel = ofVec3f(100, 100, 100);
	_settings.FeedbackInterval = 10;
	_settings.SpindleSpeed = 100;

	if(bgColor == ofColor())bgColor = ofColor(40, 255);

	if(settingsFileName == "") settingsFileName = "grblSettings.xml";

	initUI();
	Connect();

	color = ofColor::white;
}

//--------------------------------------------------------------
void ofxGrbl::update() {

	//int waitCount = 10000;
	//while (!serial.available() && waitCount > 0) {
	//	cout << "[ waiting ]" << waitCount << endl;
	//	waitCount--;
	//}
	//if (waitCount == 0) {
	//	isConnect = false;
	//	cout << "[ timeout ]" << _port << " is busy." << endl;
	//}

	if (isConnect) {
		while (serial.available() > 0) {
			if (!isDeviceReady) {
				isDeviceReady = true;
				sendMessage("$$", true);
				setSettings();
			}

			char _byte = (char)serial.readByte();
			if (_byte == '\n' || _byte == '\r') {
				if (readBuffer != "") {
					cout << "[ ofxGrbl ] [ RECEIVE ] " << readBuffer << endl;
					if (readBuffer == "ok") {
						isReadyToSend = true;
						//sentCount--;
						//cout << "[ ofxGrbl ] Sent: " << sentCount << endl;
					}
					if (readBuffer == "error: Unsupported command") {
						cout << "[ ofxGrbl ] [ PAUSED ]" << endl;
						isPause = true;
						isReadyToSend = true;
					}
					if (readBuffer[0] == '<') {
						// parse grbl state message
						/*
						vector<string> _status = ofSplitString(readBuffer, ",");
						vector<string> _posx = ofSplitString(_status[1], ":");
						vector<string> _posz = ofSplitString(_status[3], ">");
						cout << "[ ofxGrbl ] [ POSITION ] " << _posx[1] << ", " << _status[2] << ", " << _posz[0] << endl;
						currentPos = ofVec2f(ofToFloat(_posx[1]) / (float)GRBL_WIDTH, ofToFloat(_status[2]) / (float)GRBL_HEIGHT);
						*/

						readBuffer = readBuffer.substr(1, readBuffer.length() - 2);
						vector<string> _status = ofSplitString(readBuffer, "|");
						status = _status[0];
						vector<string> _pos_str = ofSplitString(_status[1], ":");
						vector<string> _pos = ofSplitString(_pos_str[1], ",");
						cout << "[ ofxGrbl ] [ POSITION ] " << _pos[0] << ", " << _pos[1] << ", " << _pos[2] << endl;
						currentPos = ofVec3f(ofToFloat(_pos[0]) / (float)GRBL_WIDTH, ofToFloat(_pos[1]) / (float)GRBL_HEIGHT);
						// Events
						ofNotifyEvent(PositionEvent, currentPos);
						if (ofToFloat(_pos[2]) < 0) {
							if (!isDown) {
								isDown = true;
								cout << "[ ofxGrbl ] DOWN" << endl;
								ofNotifyEvent(UpDownEvent, isDown);
								ofxUILabelToggle *_toggle = (ofxUILabelToggle *)gui->getWidget("bSpindle");
								_toggle->setValue(isDown);

								if (strokeList.size() > 1) strokeList.erase(strokeList.begin());
							}
						}
						else {
							if (isDown) {
								isDown = false;
								cout << "[ ofxGrbl ] UP" << endl;
								ofNotifyEvent(UpDownEvent, isDown);
								ofxUILabelToggle *_toggle = (ofxUILabelToggle *)gui->getWidget("bSpindle");
								_toggle->setValue(isDown);
							}
						}
					}
					readBuffer = "";
				}
			}
			else {
				readBuffer = readBuffer + _byte;
			}
		}
	}

	if (isConnect && isDeviceReady) {

		// send
		if (isReadyToSend && !isPause) {
			if (sendQueList.size() > 0) {
				sendMessage(sendQueList[0], true);

				sendQueList.erase(sendQueList.begin());
				cout << "[ ofxGrbl ] [ QUE ] " << sendQueList.size() << endl;

				isReadyToSend = false;
			}
		}

		// get status
		if (ofGetFrameNum() % (int)_settings.FeedbackInterval == 0) sendMessage("?", true);
	}

	ofxUIImageToggle *toggle = (ofxUIImageToggle *)gui->getWidget("PAUSE");
	toggle->setValue(isPause);
}

//--------------------------------------------------------------
void ofxGrbl::draw() {
	draw(0, 0, ofGetWidth(), ofGetHeight());
}
//--------------------------------------------------------------
void ofxGrbl::draw(int x, int y, int w, int h) {
	_fbo.begin();
	ofBackground(bgColor);
	ofNoFill();
	ofSetColor(255);
	ofSetLineWidth(3);
	ofDrawRectangle(2, 2, WINDOW_WIDTH - 3, WINDOW_HEIGHT - 3);
	ofFill();
	ofSetLineWidth(3);

	if (ofGetMousePressed(0)) {
		//ofSetColor(color);
		//ofDrawCircle((float)ofGetMouseX() / ofGetWidth() * WINDOW_WIDTH, (float)ofGetMouseY() / ofGetHeight() * WINDOW_HEIGHT, 15);
		//ofSetColor(0, 255, 0);
		//ofDrawBitmapString("X:" + ofToString(ofGetMouseX() / (float)ofGetWidth() * GRBL_WIDTH) + "\nY:" + ofToString(ofGetMouseY() / (float)ofGetHeight() * GRBL_HEIGHT), (float)ofGetMouseX() / ofGetWidth() * WINDOW_WIDTH + 20, (float)ofGetMouseY() / ofGetHeight() * WINDOW_HEIGHT + 20);
	}

	for (int i = 0; i < strokeList.size(); i++) {
		ofPath line = ofPath();
		line.setFilled(false);
		line.setStrokeWidth(3);
		line.setStrokeColor(ofColor(255, 255, 255));
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
	_fbo.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxGrbl::close() {
	if (isConnect) {
		if (bSpindle) setSpindle(false, true);
		sendMessage("G90 G0 X0 Y0 Z0", true);
	}
}

//--------------------------------------------------------------
void ofxGrbl::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofxGrbl::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofxGrbl::mouseMoved(int x, int y) {
	
	if (x < 0)x = 0;
	if (x > GRBL_WIDTH) x = GRBL_WIDTH;
	if (y < 0)y = 0;
	if (y > GRBL_HEIGHT) y = GRBL_HEIGHT;
	targetPos = ofVec3f((float)x / GRBL_WIDTH, (float)y / GRBL_HEIGHT);

	if (gui->isVisible()) return;
	if (isDrawMode == false) {
		sendMessage(vec3fToGcode(targetPos));
	}
}

//--------------------------------------------------------------
void ofxGrbl::mouseDragged(int x, int y, int button) {
	if (gui->isVisible()) return;
	if (x < 0)x = 0;
	if (x > GRBL_WIDTH) x = GRBL_WIDTH;
	if (y < 0)y = 0;
	if (y > GRBL_HEIGHT) y = GRBL_HEIGHT;

	targetPos = ofVec3f((float)x / GRBL_WIDTH, (float)y / GRBL_HEIGHT);

	// minimum move
	if (ofDist(prevPos.x, prevPos.y, targetPos.x, targetPos.y) < 0.001f) {
		cout << "Minimum move : " << ofDist(prevPos.x, prevPos.y, targetPos.x, targetPos.y) << endl;
		return;
	}

	tmpStroke.push_back(targetPos);
	sendMessage(vec3fToGcode(targetPos));

	prevPos = targetPos;
}

//--------------------------------------------------------------
void ofxGrbl::mousePressed(int x, int y, int button) {
	if (gui->isVisible()) return;
	tmpStroke.clear();
	ofVec3f _tmpVec2 = ofVec3f((float)x / GRBL_WIDTH, (float)y / GRBL_HEIGHT);

	tmpStroke.push_back(_tmpVec2);
	sendMessage(vec3fToGcode(_tmpVec2));
	/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/setSpindle(true, false);
	if (_settings.Mode == "Plotter")  sendMessage("G1 Z" + ofToString(_settings.HomePosition.z - _settings.PushDistance, 2));
}

//--------------------------------------------------------------
void ofxGrbl::mouseReleased(int x, int y, int button) {
	if (gui->isVisible()) return;
	ofVec3f _tmpVec2 = ofVec3f((float)x / GRBL_WIDTH, (float)y / GRBL_HEIGHT);
	tmpStroke.push_back(_tmpVec2);
	if (tmpStroke.size() > 2) {
		strokeList.push_back(tmpStroke);
	}

	tmpStroke.clear();

	sendMessage(vec3fToGcode(_tmpVec2));
	/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/setSpindle(false, false);
	if(_settings.Mode == "Plotter")  sendMessage("G1 Z" + ofToString(_settings.HomePosition.z, 2));
}

//--------------------------------------------------------------
void ofxGrbl::windowResized(int w, int h) {
	//cout << "[ ofxGrbl ] windowResized( " << w << ", " << h << " )" << endl;
	gui->setScrollAreaHeight(h);
}

//--------------------------------------------------------------
void ofxGrbl::dragEvent(ofDragInfo dragInfo) {
	for (int i = 0; i < dragInfo.files.size(); i++) {
		string _ext = ofFile(dragInfo.files[i]).getExtension();
		if (_ext == "gcode" || _ext == "nc" || _ext == "ngc") {
			loadFromFile(ofToDataPath(dragInfo.files[i]));
		}
		else {
			cout << "[ ofxGrbl ] Invalid extension. Please use ( .gcode / .ngc / .nc ). " << endl;
		}
	}
}

//--------------------------------------------------------------
void ofxGrbl::loadFromFile(string _path) {
	cout << "[ ofxGrbl ] loadFromFile( " << _path << " )" << endl;
	string _text = string(ofBufferFromFile(_path));
	vector<string> _linelist = ofSplitString(_text, "\n", true);
	cout << "[ ofxGrbl ] loadFromFile() : " << _linelist.size() << " lines." << endl;

	vector<ofVec3f> _tmpVec;
	for (int i = 0; i < _linelist.size(); i++) {

		if (_settings.Mode == "Plotter") {
			if (_linelist[i].find('Z') != string::npos) {
				if (checkZisDown(_linelist[i])) {
					_tmpVec.clear();
				}
				else {
					if (_tmpVec.size() > 1) strokeList.push_back(_tmpVec);
				}
			}

			if (checkMoveCommand(_linelist[i])) {
				cout << "[ ofxGrbl ] Command added : " << _linelist[i] << endl;
				_tmpVec.push_back(gcodeToVec3f(_linelist[i]));
			}
			else if (_linelist[i][0] == 'M') {

			}
			else {
				cout << "[ ofxGrbl ] Invalid command : " << _linelist[i] << endl;
			}
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
				//sentCount++;
				cout << "[ ofxGrbl ] sendMessage( " << _msg << " )" << endl;
			}
			else {
				cout << "[ ofxGrbl ] sendMessage() : Message is empty." << endl;
			}
		}
		else {
			cout << "[ ofxGrbl ] sendMessage() : Serial is not connected." << endl;
		}
	}
	else {
		if (_msg != "") {
			string _message = _msg;
			sendQueList.push_back(_message);
		}
	}
}

bool ofxGrbl::checkMoveCommand(string _line) {
	return (_line.find('X') != string::npos) || (_line.find('Y') != string::npos) || (_line.find('Z') != string::npos);
}

bool ofxGrbl::checkZisDown(string _line) {
	float _zPos;
	vector<string> _commands = ofSplitString(_line, " ", true);
	for (int i = 0; i < _commands.size(); i++) {
		if (_commands[i][0] == 'Z') {
			if (_commands[i].size() == 1) {
				// space parse
				_zPos = ofToFloat(_commands[i + 1]); // use next character
			}
			else {
				// no space parce
				_zPos = ofToFloat(_commands[i].substr(1));
			}
		}
	}

	return (_zPos == _settings.HomePosition.z - _settings.PushDistance);
}

ofVec3f ofxGrbl::gcodeToVec3f(string _line) {
	ofVec3f _result = ofVec3f::zero();
	vector<string> _commands = ofSplitString(_line, " ", true);
	for (int i = 0; i < _commands.size(); i++) {
		if (_commands[i][0] == 'X') {
			if (_commands[i].size() == 1) {
				// space parse
				_result.x = ofToFloat(_commands[i + 1]) / GRBL_WIDTH; // use next character
			}
			else {
				// no space parse
				_result.x = ofToFloat(_commands[i].substr(1)) / GRBL_WIDTH;
			}
		}
		else if (_commands[i][0] == 'Y') {
			if (_commands[i].size() == 1) {
				// space parse
				_result.y = ofToFloat(_commands[i + 1]) / GRBL_HEIGHT; // use next character
			}
			else {
				// no space parce
				_result.y = ofToFloat(_commands[i].substr(1)) / GRBL_HEIGHT;
			}
		}
		else if (_commands[i][0] == 'Z') {
			if (_commands[i].size() == 1) {
				// space parse
				_result.z = ofToFloat(_commands[i + 1]) / GRBL_DEPTH; // use next character
			}
			else {
				// no space parce
				_result.z = ofToFloat(_commands[i].substr(1)) / GRBL_DEPTH;
			}
		}
	}

	return _result;
}

string ofxGrbl::vec3fToGcode(ofVec3f _vec) {
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

	gui = new ofxUIScrollableCanvas(0, 5, length + xInit, 1080);
	gui->setScrollAreaToScreen();
	gui->setScrollableDirections(false, true);

	gui->setFont("./GUI/NewMedia Fett.ttf");
	gui->addWidgetDown(new ofxUILabel("[ Grbl Manager ]", OFX_UI_FONT_LARGE));
	gui->addSpacer(length - xInit, 2);

	gui->addLabel("MODE_LABEL", "Mode");
	gui->addRadio("MODE", modeList);
	gui->addSpacer(length - xInit, 2);
	gui->addLabel("SPINDLE_LABEL", "Spindle");
	gui->addLabelToggle("bSpindle", &bSpindle);
	gui->addSlider("Spindle Speed(Laser Power)", 0.0, 1000.0, &_settings.SpindleSpeed);
	gui->addSpacer(length - xInit, 2);
	gui->addLabel("Plotter_LABEL", "Plotter");
	gui->addSlider("Push Distance", 0.0, 100.0, &_settings.PushDistance);
	
	gui->addSpacer(length - xInit, 2);
	gui->addWidgetDown(new ofxUILabel("Grbl Settings", OFX_UI_FONT_MEDIUM));
	gui->addSlider("MAX SPEED X", 1, 20000, &_settings.MaxSpeed.x);
	gui->addSlider("MAX SPEED Y", 1, 20000, &_settings.MaxSpeed.y);
	gui->addSlider("MAX SPEED Z", 1, 20000, &_settings.MaxSpeed.z);
	gui->addSlider("ACCEL X", 10.0, 500.0, &_settings.Accel.x);
	gui->addSlider("ACCEL Y", 10.0, 500.0, &_settings.Accel.y);
	gui->addSlider("ACCEL Z", 10.0, 500.0, &_settings.Accel.z);
	gui->addSlider("MAX TRAVEL X", 100.0, 2000.0, &_settings.MaxTravel.x);
	gui->addSlider("MAX TRAVEL Y", 100.0, 2000.0, &_settings.MaxTravel.y);
	gui->addSlider("MAX TRAVEL Z", 100.0, 2000.0, &_settings.MaxTravel.z);
	
	gui->addSlider("FEEDBACK INTERVAL", 1, 300, &_settings.FeedbackInterval);

	gui->addLabelButton("SET", false);

	//gui->addLabelButton("HOMING", false, (length/2) - xInit, dim);
	//gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	//gui->addLabelButton("KILL", false, (length / 2) - xInit, dim);
	//gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

	gui->addSpacer(length - xInit, 2);
	//gui->addWidgetDown(new ofxUILabel("SERIAL SETTINGS", OFX_UI_FONT_MEDIUM));
	gui->addLabel("PORT_LABEL", "Serial Port");
	gui->addTextInput("PORT", "COM3");
	//gui->addLabel("BAUDRATE_LABEL", "BAUDRATE");
	//gui->addRadio("BAUDRATE", baudrateList);
	gui->addLabelButton("CONNECT", false);
	//gui->addLabel("SEND MESSAGE");
	//gui->addTextInput("INPUT", "$$");
	//gui->addLabelButton("SERIAL SEND", false);
	//gui->addLabelButton("GET INFO", false);

	gui->addSpacer(length - xInit, 2);
	gui->addWidgetDown(new ofxUILabel("CONTROL", OFX_UI_FONT_MEDIUM));
	gui->addImageButton("HOME", "./GUI/icon/fa-home.png", false, 30, 30);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	gui->addImageButton("PLAY", "./GUI/icon/fa-play-circle-o.png", false, 30, 30);
	gui->addImageToggle("PAUSE", "./GUI/icon/fa-pause-circle-o.png", &isPause, 30, 30);
	gui->addImageButton("STOP", "./GUI/icon/fa-stop-circle-o.png", false, 30, 30);
	gui->addImageButton("PATH_SAVE", "./GUI/icon/fa-save.png", false, 30, 30);
	gui->addImageButton("PATH_LOAD", "./GUI/icon/fa-folder-open.png", false, 30, 30);
	gui->addImageButton("TRASH", "./GUI/icon/fa-trash.png", false, 30, 30);
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

	gui->addSpacer(length - xInit, 2);
	gui->addLabelButton("SAVE", false);
	gui->addLabelButton("LOAD", false);

	gui->setTheme(30);

	gui->autoSizeToFitWidgets();
	gui->getRect()->setWidth(ofGetWidth());

	ofAddListener(gui->newGUIEvent, this, &ofxGrbl::guiEvent);

	loadSettings();
}

//--------------------------------------------------------------
void ofxGrbl::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//cout << "got event from: " << name << " kind:" << ofToString(kind) << endl;
	
	if (name == "PORT") {
		ofxUITextInput *input = (ofxUITextInput *)e.widget;
		port = input->getTextString();
	}
	else if (name == "BAUDRATE") {
		ofxUIRadio *radio = (ofxUIRadio *)e.widget;
		baudrate = ofToInt(radio->getActiveName());
	}
	else if (name == "CONNECT") {
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
	else if (name == "MODE") {
		ofxUIRadio *radio = (ofxUIRadio *)e.widget;
		string _selected = radio->getActiveName();
		_settings.Mode = _selected;
	}
	else if (name == "Spindle" || name == "Laser" || name == "Plotter") {
		ofxUIToggle *toggle = (ofxUIToggle *)e.widget;
		if (toggle->getValue()) {
			_settings.Mode = toggle->getName();
			if (_settings.Mode == "Laser") {
				sendMessage("$32=1", true);
			}
			else {
				sendMessage("$32=0", true);
			}
		}
	}
	else if (name == "Spindle Speed(Laser Power)") {
		setSpindleSpeed(_settings.SpindleSpeed, true);
	}
	else if (name == "bSpindle") {
		ofxUILabelToggle *toggle = (ofxUILabelToggle *)e.widget;
		setSpindle(toggle->getValue(), true);
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
	else if (name == "HOMING") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			homing();
		}
	}
	else if (name == "KILL") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			killAlarmLock();
		}
	}
	else if (name == "PLAY") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			if (sendQueList.size() == 0) {
				drawStrokes();
			}
			isPause = false;
		}
	}
	else if (name == "STOP") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			resetStrokes();
		}
	}
	else if (name == "PATH_SAVE") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			if (tmpStroke.size() > 2) {
				strokeList.push_back(tmpStroke);
			}
			tmpStroke.clear();

			// Did'nt work on WindowsOS.
			/*
			ofFileDialogResult saveFileResult = ofSystemSaveDialog("strokeList.gcode", "Save stroke paths to GCODE(.gcode)");
			saveStrokes(saveFileResult.getPath());
			*/
			string _fileName = ofSystemTextBoxDialog("Please enter the file name.", "stroke");
			saveStrokes("./" + _fileName + ".gcode");
		}
	}
	else if (name == "PATH_LOAD") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a GCODE(.gcode .nc .ngc) file");
			loadFromFile(openFileResult.getPath());
		}
	}
	else if (name == "TRASH") {
		ofxUILabelButton *button = (ofxUILabelButton *)e.widget;
		if (button->getValue()) {
			resetStrokes();
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
void ofxGrbl::toggleVisible() {
	gui->toggleVisible();
}

//--------------------------------------------------------------
void ofxGrbl::saveSettings() {
	cout << "[ ofxGrbl ] saveSettings" << endl;
	gui->saveSettings("./GUI/" + settingsFileName);
}
//--------------------------------------------------------------
void ofxGrbl::loadSettings() {
	cout << "[ ofxGrbl ] loadSettings" << endl;
	gui->loadSettings("./GUI/" + settingsFileName);

	setSettings();

	if (firstTimeLoad) {
		cout << "[ ofxGrbl ] FirstTimeLoad!" << endl;
		firstTimeLoad = false;
		currentPos = targetPos = prevPos = ofVec3f(_settings.HomePosition.x  * _settings.MaxTravel.x, _settings.HomePosition.y  * _settings.MaxTravel.y, _settings.HomePosition.z  * _settings.MaxTravel.z);
	}
}

void ofxGrbl::drawStrokes() {
	for (int i = 0; i < strokeList.size(); i++) {

		for (int j = 0; j < strokeList[i].size(); j++) {
			sendMessage(vec3fToGcode(strokeList[i][j]));
			if (j == 0) {
				if (_settings.Mode == "Plotter") sendMessage("G1 Z" + ofToString(_settings.HomePosition.z - _settings.PushDistance, 4));
				/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/setSpindle(true, false);
			}
			if (j == strokeList[i].size() - 1) {
				if (_settings.Mode == "Plotter") sendMessage("G1 Z" + ofToString(_settings.HomePosition.z, 4));
				/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/setSpindle(false, false);
			}
		}
	}
}

void ofxGrbl::saveStrokes(string _path) {
	string output;

	for (int i = 0; i < strokeList.size(); i++) {

		for (int j = 0; j < strokeList[i].size(); j++) {
			output += vec3fToGcode(strokeList[i][j]) + "\n";
			if (j == 0){
				if (_settings.Mode == "Plotter") output += "G1 Z" + ofToString(_settings.HomePosition.z - _settings.PushDistance, 4) + "\n";
				output += vec3fToGcode(strokeList[i][j]) + "\n";
				/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/ output += "M3\n";
			}
			if (j == strokeList[i].size() - 1) {
				if (_settings.Mode == "Plotter") output += "G1 Z" + ofToString(_settings.HomePosition.z, 4) + "\n";
				/*if (_settings.Mode == "Spindle" || _settings.Mode == "Laser")*/ output += "M5\n";
			}
		}
	}

	string fileName = _path;
	ofBuffer buffer = ofBuffer(output);
	ofBufferToFile(fileName, buffer);
}

void ofxGrbl::resetStrokes() {
	strokeList.clear();
	sendQueList.clear();
}

void ofxGrbl::home() {
	sendMessage("G90 G0 X" + ofToString(_settings.HomePosition.x) + " Y" + ofToString(_settings.HomePosition.y) + " Z" + ofToString(_settings.HomePosition.z), true);
}

void ofxGrbl::homing() {
	sendMessage("$H", true);
}

void ofxGrbl::killAlarmLock() {
	sendMessage("$X", true);
}

void ofxGrbl::Connect(string _port, int _baudrate) {
	if (_port == "") _port = port;
	if (_baudrate <= 0) _baudrate = baudrate;

	cout << "[ ofxGrbl ] Connect( " << _port << ", " << _baudrate << " )" << endl;

	// reset serial
	if (isConnect || isDeviceReady) {
		serial.close();
		isConnect = false;
		isDeviceReady = false;
	}

	isConnect = serial.setup(_port, _baudrate);
	if (isConnect) {
		cout << "[ ofxGrbl ] Connected to " << _port << "@" << _baudrate << " !" << endl;
	} else {
		cout << _port << " is not exists." << endl;
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
void ofxGrbl::setBGColor(ofColor _color) {
	cout << "[ ofxGrbl ] setBGColor() : R" << (int)_color.r << " G" << (int)_color.g << " B" << (int)_color.b << " A" << (int)_color.a << endl;
	bgColor = _color;
}

void ofxGrbl::setSettings() {
	// set mode
	if (_settings.Mode == "Laser") {
		sendMessage("$32=1");
	}
	else {
		sendMessage("$32=0");
	}
	// set spindle speed
	sendMessage("S" + ofToString((int)_settings.SpindleSpeed));

	// set max speed
	sendMessage("F" + ofToString(_settings.MaxSpeed.x, 4));
	sendMessage("$110=" + ofToString(_settings.MaxSpeed.x, 4));
	sendMessage("$111=" + ofToString(_settings.MaxSpeed.y, 4));
	sendMessage("$112=" + ofToString(_settings.MaxSpeed.z, 4));
	// set accesl
	sendMessage("$120=" + ofToString(_settings.Accel.x, 4));
	sendMessage("$121=" + ofToString(_settings.Accel.y, 4));
	sendMessage("$122=" + ofToString(_settings.Accel.z, 4));
	// set max travel
	sendMessage("$130=" + ofToString(_settings.MaxTravel.x, 4));
	sendMessage("$131=" + ofToString(_settings.MaxTravel.y, 4));
	sendMessage("$132=" + ofToString(_settings.MaxTravel.z, 4));

	setArea(_settings.MaxTravel.x, _settings.MaxTravel.y);
}

void ofxGrbl::setArea(float x, float y, float z) {
	cout << "[ ofxGrbl ] setArea(" << (int)x << ", " << (int)y << ", " << (int)z << ")" << endl;
	GRBL_WIDTH = x;
	GRBL_HEIGHT = y;
	GRBL_DEPTH = z;

	ofSetWindowShape(GRBL_WIDTH, GRBL_HEIGHT);

	WINDOW_WIDTH = ofGetWidth();
	WINDOW_HEIGHT = ofGetHeight();
	_fbo.allocate(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void ofxGrbl::setHome(float x, float y, float z) {
	cout << "[ ofxGrbl ] setHome(" << (int)x << ", " << (int)y << ", " << (int)z << ")" << endl;
	_settings.HomePosition = ofVec3f(x, y, z);
}
void ofxGrbl::setHome(ofVec3f _homePos) {
	cout << "[ ofxGrbl ] setHome(" << _homePos.x << ", " << _homePos.y << ", " << _homePos.z << ")" << endl;
	_settings.HomePosition = ofVec3f(_homePos.x, _homePos.y, _homePos.z);
}

void ofxGrbl::setSpindle(bool _enable, bool _direct) {
	cout << "[ ofxGrbl ] setArea(" << _enable << ", " << _direct << ")" << endl;
	bSpindle = _enable;
	if (bSpindle) {
		sendMessage("M3", _direct);
	}
	else {
		sendMessage("M5", _direct);
	}
}
void ofxGrbl::setSpindleSpeed(int _speed, bool _direct) {
	cout << "[ ofxGrbl ] setSpindleSpeed(" << _speed << ", " << _direct << ")" << endl;
	_settings.SpindleSpeed = _speed;
	sendMessage("S" + ofToString((int)_settings.SpindleSpeed), _direct);
}

void ofxGrbl::setPosition(float _mmX, float _mmY, float _mmZ) {
	tmpStroke.push_back(ofVec3f(_mmX / GRBL_WIDTH, _mmY / GRBL_HEIGHT));
	sendMessage("G90 G1 X" + ofToString(_mmX) + " Y" + ofToString(_mmY) + "  Z" + ofToString(_mmZ) + " ", false);
}
void ofxGrbl::setPosition(ofVec3f _pos) {
	setPosition(_pos.x, _pos.y, _pos.z);
}

void ofxGrbl::moveRight(float _mm) {
	sendMessage("G91 G1 X" + ofToString(_mm) + " Y0 Z0", true);
}

void ofxGrbl::moveLeft(float _mm) {
	sendMessage("G91 G1 X" + ofToString(-_mm) + " Y0 Z0", true);
}

void ofxGrbl::moveUp(float _mm) {
	sendMessage("G91 G1 X0 Y" + ofToString(_mm) + " Z0", true);
}

void ofxGrbl::moveDown(float _mm) {
	sendMessage("G91 G1 X0 Y" + ofToString(-_mm) + " Z0", true);
}