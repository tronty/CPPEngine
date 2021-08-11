/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.1
 * 
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 * 
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to install, execute and perform the Spine Runtimes
 * Software (the "Software") solely for internal use. Without the written
 * permission of Esoteric Software (typically granted by licensing Spine), you
 * may not (a) modify, translate, adapt or otherwise create derivative works,
 * improvements of the Software or develop new applications using the Software
 * or (b) remove, delete, alter or obscure any trademarks or any copyright,
 * trademark, patent or other intellectual property or proprietary rights
 * notices on or in the Software, including any copy thereof. Redistributions
 * in binary or source form must include this license and terms.
 * 
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <iostream>
#include <string.h>
#include <spine/spine-sfml.h>
//include <SFML/Graphics.hpp>
//include <SFML/Window/Mouse.hpp>

#include <STX/STX.h>
#include <STX/STXCPP.h>


using namespace std;
using namespace spine;
#include <stdio.h>

Atlas* atlasspineboy = 0;
SkeletonData *skeletonDataspineboy = 0;
SkeletonDrawable* drawablespineboy = 0;
SkeletonBounds* boundsspineboy = 0;
Atlas* atlasgoblins = 0;
SkeletonData *skeletonDatagoblins = 0;
SkeletonDrawable* drawablegoblins = 0;
Atlas* atlasraptor = 0;
SkeletonData *skeletonDataraptor = 0;
SkeletonDrawable* drawableraptor = 0;

void callback (AnimationState* state, int trackIndex, EventType type, Event* event, int loopCount) {
	TrackEntry* entry = AnimationState_getCurrent(state, trackIndex);
	const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;

	switch (type) {
	case ANIMATION_START:
		//STX_PRINT("%d start: %s\n", trackIndex, animationName);
		break;
	case ANIMATION_END:
		//STX_PRINT("%d end: %s\n", trackIndex, animationName);
		break;
	case ANIMATION_COMPLETE:
		//STX_PRINT("%d complete: %s, %d\n", trackIndex, animationName, loopCount);
		break;
	case ANIMATION_EVENT:
		//STX_PRINT("%d event: %s, %s: %d, %f, %s\n", trackIndex, animationName, event->data->name, event->intValue, event->floatValue, event->stringValue);
		break;
	}
	fflush(stdout);
}

void spineboy () {
	
	// Load atlas, skeleton, and animations.
	std::string filename="";
	filename.append("/spine/spineboy/export/spineboy.atlas");
	atlasspineboy = Atlas_createFromFile(filename.c_str(), 0);
	SkeletonJson* json = SkeletonJson_create(atlasspineboy);
	json->scale = 0.6f;
	filename="";
	filename.append("/spine/spineboy/export/spineboy.json");
	skeletonDataspineboy = SkeletonJson_readSkeletonDataFile(json, filename.c_str());
	if (!skeletonDataspineboy) {
		STX_PRINT("%s\n", json->error);
		exit(0);
	}
	SkeletonJson_dispose(json);
	boundsspineboy = SkeletonBounds_create();

	// Configure mixing.
	AnimationStateData* stateData = AnimationStateData_create(skeletonDataspineboy);
	AnimationStateData_setMixByName(stateData, "walk", "jump", 0.2f);
	AnimationStateData_setMixByName(stateData, "jump", "run", 0.2f);

	drawablespineboy = new SkeletonDrawable(skeletonDataspineboy, stateData);
	drawablespineboy->timeScale = 1;

	Skeleton* skeleton = drawablespineboy->skeleton;
	skeleton->flipX = false;
	skeleton->flipY = false;
	Skeleton_setToSetupPose(skeleton);

	skeleton->x = 320;
	skeleton->y = 460;
	Skeleton_updateWorldTransform(skeleton);

	Slot* headSlot = Skeleton_findSlot(skeleton, "head");

	drawablespineboy->state->listener = callback;
	AnimationState_setAnimationByName(drawablespineboy->state, 0, "walk", true);

	/*
	sf::RenderWindow window(sf::VideoMode(640, 480), "Spine SFML - spineboy");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		SkeletonBounds_update(bounds, skeleton, true);
		sf::Vector2i position = sf::Mouse::getPosition(window);
		if (SkeletonBounds_containsPoint(bounds, position.x, position.y)) {
			headSlot->g = 0;
			headSlot->b = 0;
		} else {
			headSlot->g = 1;
			headSlot->b = 1;
		}

		drawable->update(delta);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
	*/
}
void renderspineboy (const char* aani) {
	if (stx_strcmp("SpineBoyTest",aani)==0)
		AnimationState_setAnimationByName(drawablespineboy->state, 0, "test", true);
	else if (stx_strcmp("SpineBoyWalk",aani)==0)
		AnimationState_setAnimationByName(drawablespineboy->state, 0, "walk", true);
	else if (stx_strcmp("SpineBoyJump",aani)==0)
		AnimationState_addAnimationByName(drawablespineboy->state, 0, "jump", false, 3);
	else if (stx_strcmp("SpineBoyRun",aani)==0)
		AnimationState_addAnimationByName(drawablespineboy->state, 0, "run", true, 0);

	float delta = timeGetTime()/1000000.0f;

	drawablespineboy->update(delta);		
	
	drawablespineboy->draw();

	//SkeletonData_dispose(skeletonDataspineboy);
	//SkeletonBounds_dispose(boundsspineboy);
	//Atlas_dispose(atlasspineboy);
}

void goblins () {
	
	// Load atlas, skeleton, and animations.
	std::string filename="";
	filename.append("/spine/goblins/export/goblins-ffd.atlas");
	atlasgoblins = Atlas_createFromFile(filename.c_str(), 0);
	SkeletonJson* json = SkeletonJson_create(atlasgoblins);
	json->scale = 1.4f;
	filename="";
	filename.append("/spine/goblins/export/goblins-ffd.json");
	skeletonDatagoblins = SkeletonJson_readSkeletonDataFile(json, filename.c_str());
	if (!skeletonDatagoblins) {
		STX_PRINT("Error: %s\n", json->error);
		exit(0);
	}
	SkeletonJson_dispose(json);

	drawablegoblins = new SkeletonDrawable(skeletonDatagoblins);
	drawablegoblins->timeScale = 1;

	Skeleton* skeleton = drawablegoblins->skeleton;
	skeleton->flipX = false;
	skeleton->flipY = false;
	Skeleton_setSkinByName(skeleton, "goblin");
	Skeleton_setSlotsToSetupPose(skeleton);
	//Skeleton_setAttachment(skeleton, "left hand item", "dagger");

	skeleton->x = 320;
	skeleton->y = 590;
	Skeleton_updateWorldTransform(skeleton);

	AnimationState_setAnimationByName(drawablegoblins->state, 0, "walk", true);

	/*
	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - goblins");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
	*/
}
void rendergoblins () {
		
	float delta = timeGetTime()/1000000.0f;
	drawablegoblins->update(delta);
	
	drawablegoblins->draw();

	//SkeletonData_dispose(skeletonDatagoblins);
	//Atlas_dispose(atlasgoblins);
}

void raptor () {
	
	
	std::string filename="";
	filename.append("/spine/raptor/export/raptor.atlas");
	// Load atlas, skeleton, and animations.
	atlasraptor = Atlas_createFromFile(filename.c_str(), 0);
	
	SkeletonJson* json = SkeletonJson_create(atlasraptor);
	
	json->scale = 0.5f;
	
	filename="";
	filename.append("/spine/raptor/export/raptor.json");
	
	skeletonDataraptor = SkeletonJson_readSkeletonDataFile(json, filename.c_str());
	
	if (!skeletonDataraptor) {
		STX_PRINT("Error: %s\n", json->error);
		exit(0);
	}
	
	SkeletonJson_dispose(json);

	
	drawableraptor = new SkeletonDrawable(skeletonDataraptor);
	
	drawableraptor->timeScale = 1;

	
	Skeleton* skeleton = drawableraptor->skeleton;
	
	skeleton->x = 320;
	
	skeleton->y = 590;
	
	Skeleton_updateWorldTransform(skeleton);

	
	AnimationState_setAnimationByName(drawableraptor->state, 0, "walk", true);
	
	AnimationState_setAnimationByName(drawableraptor->state, 1, "empty", false);
	
	AnimationState_addAnimationByName(drawableraptor->state, 1, "gungrab", false, 2);

	/*
	sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - raptor");
	window.setFramerateLimit(60);
	sf::Event event;
	sf::Clock deltaClock;
	while (window.isOpen()) {
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();

		float delta = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();

		drawable->update(delta);

		window.clear();
		window.draw(*drawable);
		window.display();
	}
	*/

}
void renderraptor (const char* aani) {
	if (stx_strcmp("RaptorWalk",aani)==0)
		AnimationState_setAnimationByName(drawableraptor->state, 0, "walk", true);
	else if (stx_strcmp("RaptorEmpty",aani)==0)
		AnimationState_addAnimationByName(drawableraptor->state, 0, "empty", false, 3);
	else if (stx_strcmp("RaptorGungrab",aani)==0)
		AnimationState_addAnimationByName(drawableraptor->state, 0, "gungrab", true, 0);
		
	
	float delta = timeGetTime()/1000000.0f;
	
	
	drawableraptor->update(delta);
	
	
	
	drawableraptor->draw();

	
	
	//SkeletonData_dispose(skeletonDataraptor);
	
	
	//Atlas_dispose(atlasraptor);
	
}

int initSpine2DAnimations()
{	
	
	raptor();
	
	spineboy();
	
	goblins();
	
	return 0;
}

int renderSpine2DAnimations(const char* aani) {
	if (stx_strcmp("GoblinWalk",aani)==0)
		rendergoblins();
	else if (stx_strcmp("RaptorWalk",aani)==0)
		renderraptor(aani);
	else if (stx_strcmp("RaptorEmpty",aani)==0)
		renderraptor(aani);
	else if (stx_strcmp("RaptorGungrab",aani)==0)
		renderraptor(aani);
	else if (stx_strcmp("SpineBoyTest",aani)==0)
		renderspineboy(aani);
	else if (stx_strcmp("SpineBoyWalk",aani)==0)
		renderspineboy(aani);
	else if (stx_strcmp("SpineBoyJump",aani)==0)
		renderspineboy(aani);
	else if (stx_strcmp("SpineBoyRun",aani)==0)
		renderspineboy(aani);	
	return 0;
}

