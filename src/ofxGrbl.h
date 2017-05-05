#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "GrblSettings.h"
class ofxGrbl : public ofBaseApp {

public:
	
	void setup();
	void update();
	void draw();
	void draw(int x, int y, int w, int h);
	void close();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);

	void loadFromFile(string _path);
	void sendMessage(string _msg, bool direct = false);
	bool checkMoveCommand(string _line);
	ofVec3f gcodeToVec3f(string _line);
	string vec3fToGcode(ofVec3f _vec);
	bool checkZisDown(string _line);
	
	void setDrawMode();
	void setMoveMode();
	void setColor(ofColor _color);
	ofColor color;

	void setSettings();
	void setArea(float x, float y, float z = 0);
	void setHome(float x, float y, float z = 0);
	void setHome(ofVec3f _homePos);
	void setSpindle(bool _enable, bool _direct = false);
	void setSpindleSpeed(int _speed, bool _direct = false);

	// controll
	void drawStrokes();
	void saveStrokes(string _path = "./strokeList.ngc");
	void resetStrokes();
	void home();
	void homing();
	void killAlarmLock();

	void moveRight(float _mm);
	void moveLeft(float _mm);
	void moveUp(float _mm);
	void moveDown(float _mm);
	void setPosition(float _mmX, float _mmY, float _mmZ = 0.0f);
	void setPosition(ofVec3f _pos);

	// fbo
	ofFbo _fbo;

	// Settings
	void saveSettings();
	void loadSettings();
	bool firstTimeLoad;

	// stage
	int GRBL_WIDTH; // mm
	int GRBL_HEIGHT; // mm
	int GRBL_DEPTH; // mm
	int WINDOW_WIDTH; // px
	int WINDOW_HEIGHT; // px

	// serial
	ofSerial serial;
	bool isConnect;
	bool isDeviceReady;
	string port;
	int baudrate;
	void Connect(string _port = "", int _baudrate = -1);

	// pathes
	ofVec3f prevPos;
	ofVec3f targetPos;
	ofVec3f currentPos;
	vector<string> sendQueList;

	string status;
	bool isPause;
	bool isReadyToSend;
	//int sentCount;
	string readBuffer;
	bool isDrawMode = false;
	bool isDown = false;
	bool bSpindle = false;

	vector<vector<ofVec3f>> strokeList;
	vector<ofVec3f> tmpStroke;

	// UI
	void initUI();
	void guiEvent(ofxUIEventArgs &e);
	void toggleVisible();
	ofxUICanvas *gui;
	float theme;
	vector<string> baudrateList;
	vector<string> modeList;
	float feedback_interval;

	// events
	ofEvent<bool> UpDownEvent;

	// Grbl Settings
	GrblSettings _settings;
};
