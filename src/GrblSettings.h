#pragma once
#include "ofMain.h"

class GrblSettings : public ofBaseApp {

	/*
	// About Grbl Settings
	https://github.com/grbl/grbl/wiki/Configuring-Grbl-v0.9

	$0=10 (step pulse, usec)
	$1=25 (step idle delay, msec)
	$2=0 (step port invert mask:00000000)
	$3=6 (dir port invert mask:00000110)
	$4=0 (step enable invert, bool)
	$5=0 (limit pins invert, bool)
	$6=0 (probe pin invert, bool)
	$10=3 (status report mask:00000011)
	$11=0.020 (junction deviation, mm)
	$12=0.002 (arc tolerance, mm)
	$13=0 (report inches, bool)
	$20=0 (soft limits, bool)
	$21=0 (hard limits, bool)
	$22=0 (homing cycle, bool)
	$23=1 (homing dir invert mask:00000001)
	$24=50.000 (homing feed, mm/min)
	$25=635.000 (homing seek, mm/min)
	$26=250 (homing debounce, msec)
	$27=1.000 (homing pull-off, mm)
	$100=314.961 (x, step/mm)
	$101=314.961 (y, step/mm)
	$102=314.961 (z, step/mm)
	$110=635.000 (x max rate, mm/min)
	$111=635.000 (y max rate, mm/min)
	$112=635.000 (z max rate, mm/min)
	$120=50.000 (x accel, mm/sec^2)
	$121=50.000 (y accel, mm/sec^2)
	$122=50.000 (z accel, mm/sec^2)
	$130=225.000 (x max travel, mm)
	$131=125.000 (y max travel, mm)
	$132=170.000 (z max travel, mm)
	*/

public:

	ofVec3f HomePosition;

	// Area Settings (max travel, mm)
	ofVec3f MaxTravel;
	
	// Step Settings (step/mm)
	ofVec3f StepParMillimeter;

	// Accelaration (mm/sec^2)
	ofVec3f Accel;

	// Max Rate (mm/min)
	ofVec3f MaxSpeed;

	// Step Invert
	bool StepInvertX;
	bool StepInvertY;
	bool StepInvertZ;

	// Z Axis Settings
	bool isUseZAxis;
	float UpPos;
	float DownPos;

	float FeedbackInterval;
};
