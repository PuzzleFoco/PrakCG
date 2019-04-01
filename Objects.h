#pragma once
#include <list>
#include "kran.h"

using namespace std;

class container;

class Objects
{
public:
	Objects();
	list<container> listContainer;
	kran kra;
};

extern Objects objects;