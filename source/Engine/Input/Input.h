#pragma once

#include <vector>

class Input
{
public:

	void Update();

	void KeyPressed(unsigned char aKey);
	void KeyReleased(unsigned char aKey);

	bool IsKeyPressed(unsigned char aKey) { return myCurrentFrame[aKey] && myPreviousFrame[aKey] == false; }
	bool IsKeyDown(unsigned char aKey) { return myCurrentFrame[aKey]; }
	bool IsKeyReleased(unsigned char aKey) { return myCurrentFrame[aKey] == false && myPreviousFrame[aKey]; }

private:
	bool myCurrentFrame[256] = { false };
	bool myPreviousFrame[256] = { false };
};
