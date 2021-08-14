#include "animation.h"

#include "fps.h"

int animation_getframe(Animation *animation)
{
	//TODO: make this actually do something
	return animation->numImages;
}

int animation_get_frame(int msBetweenFrames, int numFrames)
{
	unsigned int ticks = ticks_game();

	return (ticks / msBetweenFrames) % numFrames;
}

int animation_get_frame_dt(int currentDt, int msBetweenFrames, int numFrames)
{
	return (currentDt / msBetweenFrames) % numFrames;
}
