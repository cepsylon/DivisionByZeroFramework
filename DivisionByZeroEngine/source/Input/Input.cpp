#include "Input.h"

void Input::Update()
{
	memcpy(myPreviousFrame, myCurrentFrame, sizeof(myCurrentFrame));
}

void Input::KeyPressed(unsigned char aKey)
{
	myCurrentFrame[aKey] = true;
}

void Input::KeyReleased(unsigned char aKey)
{
	myCurrentFrame[aKey] = false;
}
