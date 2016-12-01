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
	bool checkXYCommand(string _line);
	ofVec2f gcodeToVec2f(string _line);
	string vec2fToGcode(ofVec2f _vec);
	bool checkZisDown(string _line);
	
	void setDrawMode();
	void setMoveMode();
	void setColor(ofColor _color);
	ofColor color;

	void setSettings();
	void setArea(float x, float y);
	void setHome(float x, float y, float z = 0);
	void setHome(ofVec3f _homePos);

	// controll
	void drawCurrent();
	void saveCurrent(string _path = "./strokeList.ngc");
	void resetCurrent();
	void home();

	void moveRight(float _mm);
	void moveLeft(float _mm);
	void moveUp(float _mm);
	void moveDown(float _mm);

	// fbo
	ofFbo _fbo;

	// Settings
	void saveSettings();
	void loadSettings();
	bool firstTimeLoad;

	// stage
	int GRBL_WIDTH; // mm
	int GRBL_HEIGHT; // mm
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
	ofVec2f prevPos;
	ofVec2f targetPos;
	ofVec2f currentPos;
	vector<string> sendQueList;
	bool isPause;
	bool isReadyToSend;
	string readBuffer;
	bool isDrawMode = false;
	bool isDown = false;

	vector<vector<ofVec2f>> strokeList;
	vector<ofVec2f> tmpStroke;

	// UI
	void initUI();
	void guiEvent(ofxUIEventArgs &e);
	void toggleVisible();
	ofxUICanvas *gui;
	float theme;
	vector<string> baudrateList;
	float feedback_interval;

	// events
	ofEvent<bool> UpDownEvent;

	// Grbl Settings
	GrblSettings _settings;
};
