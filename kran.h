#pragma once
#include "container.h"
class container;

class kran
{
public:
	kran();
	void update();
	void draw();
	void setHeadlight();
	void setAttachableLight();
	void canAttach();
	float speedKrangerust;
	float speedHook;
	float speedHookdown;
	float posKrangerust;
	float posHook;
	float hookdown;
	float hookopen;
	float hookRotate;
	float hookMitteScale;
	bool rotatinglight;
	bool spotlight;
	bool isAttachable;
	bool hasAttached;
private:	
};