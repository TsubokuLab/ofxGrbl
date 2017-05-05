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


	//$0=10 (step pulse, usec)
	int StepPulse;
	//$1=25 (step idle delay, msec)
	int StepIdleDelay;
	//$2=0 (step port invert mask:00000000)
	int StepPortInvertMask;
	//$3=6 (dir port invert mask:00000110)
	int DirPortInvertMask;
	//$4=0 (step enable invert, bool)
	int StepEnableInvert;
	//$5=0 (limit pins invert, bool)
	int LimitPinsInvert;
	//$6=0 (probe pin invert, bool)
	int ProbePinInvert;
	//$10=3 (status report mask:00000011
	int StatusReportMask;
	//$11=0.020 (junction deviation, mm)
	float JunctionDeviation;
	//$12=0.002 (arc tolerance, mm)
	float ArcTolerance;
	//$13=0 (report inches, bool)
	int ReportInches;
	//$20=0 (soft limits, bool)
	int SoftLimits;
	//$21=0 (hard limits, bool)
	int HardLimits;
	//$22=0 (homing cycle, bool)
	int HomingCycles;
	//$23=1 (homing dir invert mask:00000001)
	int HomingDirInvertMask;
	//$24=50.000 (homing feed, mm/min)
	float HomingFeed;
	//$25=635.000 (homing seek, mm/min)
	float HomingSeek;
	//$26=250 (homing debounce, msec)
	int HomingDebounce;
	//$27=1.000 (homing pull-off, mm)
	float HomingPullOff;
	//$100=314.961 (x, step/mm)
	float StepX;
	//$101=314.961 (y, step/mm)
	float StepY;
	//$102=314.961 (z, step/mm)
	float StepZ;

	ofVec3f HomePosition;

	// Max Travel
	//$130=225.000 (x max travel, mm)
	//$131=125.000 (y max travel, mm)
	//$132=170.000 (z max travel, mm)
	ofVec3f MaxTravel;
	
	// Step Settings (step/mm)
	ofVec3f StepParMillimeter;

	// Accelaration
	//$120=50.000 (x accel, mm/sec^2)
	//$121=50.000 (y accel, mm/sec^2)
	//$122=50.000 (z accel, mm/sec^2)
	ofVec3f Accel;

	// Max Rate
	//$110=635.000 (x max rate, mm/min)
	//$111=635.000 (y max rate, mm/min)
	//$112=635.000 (z max rate, mm/min)
	ofVec3f MaxSpeed;

	// Step Invert
	bool StepInvertX;
	bool StepInvertY;
	bool StepInvertZ;

	// Mode (Spindle/Laser/Protter)
	string Mode;
	// Spindle Speed (S0-S100)
	float SpindleSpeed;
	// Protter Push Distance (0-100mm)
	float PushDistance;
	// Z Axis Settings
	bool isUseZAxis;
	float UpPos;
	float DownPos;

	float FeedbackInterval;
};
