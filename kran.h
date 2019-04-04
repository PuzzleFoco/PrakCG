#pragma once
#include "container.h"
class container;

class kran
{
public:
	kran();
	void update();
	void draw();
	void rotatingLight();
	void setHeadlight();
	void setAttachableLight();
	void canAttach();
	void setSchild();
	void drawground();
	void drawRader();
	float speedKrangerust;
	float speedHook;
	float speedHookdown;
	float posKrangerust;
	float posHook;
	float hookdown;
	float hookopen;
	float hookRotate;
	float hookMitteScale;
	float wheelrot;
	bool rotatinglight;
	bool spotlight;
	bool isAttachable;
	bool hasAttached;
private:	
};