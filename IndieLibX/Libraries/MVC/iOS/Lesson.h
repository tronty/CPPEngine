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

#ifndef _LESSON_H_
#define _LESSON_H_

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

//this is our general lesson class, providing the two most important methods init and draw
//which will be invoked by our EAGLView
class Lesson
{
public:
    //constructor
    Lesson();
    //the destructor has always to be virtual!
    virtual ~Lesson();
    
    //abstract methods init and draw have to be defined in derived classes
    virtual void init() = 0;
    virtual void draw() = 0;
    
    //we need to know the size of our drawing canvas (called renderbuffer here),
    //so this method just saves the parameters in the member variables
    virtual void setRenderbufferSize(unsigned int width, unsigned int height);
    
//all protected stuff will be visible within derived classes, but from nowhere else    
protected:
    //fields for the renderbuffer size
    unsigned int m_renderbufferWidth, m_renderbufferHeight;
};

#endif