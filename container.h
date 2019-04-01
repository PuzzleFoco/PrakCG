#pragma once

class kran;

 class container
{
public:
	container();
	void update(const kran &kra);
	void draw();
	void attatch(const kran & kra);
	float x;
	float y;
	float z;
	float rotated;
	bool isAttached;
private:
	
};
