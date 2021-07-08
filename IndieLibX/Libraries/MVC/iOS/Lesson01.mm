/*
 Copyright (C) 2011 by Carsten Haubold
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "Lesson01.h"

//lesson constructor
Lesson01::Lesson01()
{
    //initialize values
}

//lesson destructor
Lesson01::~Lesson01()
{
    //do cleanup
}

//initializing all OpenGL related things
void Lesson01::init()
{
    NSLog(@"Init..");
    
    //set the color we use for clearing our colorRenderbuffer to red
    glClearColor(1.0, 0.0, 0.0, 1.0);
	init(100, 100);
}

//drawing a frame
void Lesson01::draw()
{
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    //everything should be red now! yay :)
	render();
}
