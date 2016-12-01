#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxNetwork.h"
#include "ofxGrbl.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Settings
		void saveSettings();
		void loadSettings();

		// grbl
		ofxGrbl grbl;
		void UpDownHandler(bool &_isDown); // Event Handler
		ofVec3f offsetXY;
		void changeColor(ofColor _color);

		// gui
		void initUI();
		void guiEvent(ofxUIEventArgs &e);
		ofxUICanvas *gui;
		float theme;

		// udp
		void sendMessage(string _message);
		bool isSendUdp;
		bool isUdpConnected;
		bool isUdpBind;
		ofxUDPManager udp;
		ofxUDPManager udpReceiver;
		string udp_ip;
		int udp_port;
		float sendInterval;
		float prevSentTime;
		void ConnectUdp(string _udp_ip = "", int _udp_port = -1);

		ofTrueTypeFont _font;

	private:

};
