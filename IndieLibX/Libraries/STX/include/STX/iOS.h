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

#import <UIKit/UIKit.h>
//@import UIKit;

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

//this is our general lesson class, providing the two most important methods init and draw
//which will be invoked by our EAGLView
class Lesson
{
public:
    //constructor
    Lesson():
    m_renderbufferWidth(0),
    m_renderbufferHeight(0)
{}
    //the destructor has always to be virtual!
    virtual ~Lesson(){}
    
    //abstract methods init and draw have to be defined in derived classes
    virtual void initLesson() = 0;
    virtual void draw() = 0;
    
    //we need to know the size of our drawing canvas (called renderbuffer here),
    //so this method just saves the parameters in the member variables
    virtual void setRenderbufferSize(unsigned int width, unsigned int height)
{
    m_renderbufferWidth = width;
    m_renderbufferHeight = height;
    
    glViewport(0, 0, m_renderbufferWidth, m_renderbufferHeight);
}
    
//all protected stuff will be visible within derived classes, but from nowhere else    
protected:
    //fields for the renderbuffer size
    unsigned int m_renderbufferWidth, m_renderbufferHeight;
};

int init(int width, int height);
int render();

//We derive our current lesson class from the general lesson class
class Lesson01 : public Lesson
{
public:
    //overwrite all important methods
    Lesson01(){}
    virtual ~Lesson01(){}
    
//initializing all OpenGL related things
    virtual void initLesson()
{
    NSLog(@"Init..");
    
    //set the color we use for clearing our colorRenderbuffer to red
    glClearColor(1.0, 0.0, 0.0, 1.0);
	init(100, 100);
}

//drawing a frame
    virtual void draw()
{
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    //everything should be red now! yay :)
	render();
}

};

//forward declarations again
@class EAGLContext;

// This class combines our OpenGL context (which is our access to all drawing functionality)
// with a UIView that can be displayed on the iOS device. It handles the creation and presentation
// of our drawing surface, as well as handling the render loop which allows for seamless animations.
@interface EAGLView : UIView {
@private
    // The pixel dimensions of the CAEAGLLayer.
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    // These are the buffers we render to: the colorRenderbuffer will contain the color that we will
    // finaly see on the screen, the depth renderbuffer has to be used if we want to make sure, that
    // we always see only the closest object and not just the one that has been drawn most recently.
    // The framebuffer is a collection of buffers to use together while rendering, here it is either
    // just the color buffer, or color and depth renderbuffer.
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
    
    // The display link is used to create a render loop
    CADisplayLink* displayLink;
    
    // Do we need a depth buffer
    BOOL useDepthBuffer;
    
    // The pointer to the lesson which we're rendering
    Lesson* lesson;
    
    // Did we already initialize our lesson?
    BOOL lessonIsInitialized;
}

// The OpenGL context as a property (has autogenerated getter and setter)
@property (nonatomic, retain) EAGLContext *context;

// Configuration setters
- (void) setDepthBufferNeeded:(BOOL)needed;
- (void) setLesson:(Lesson*)newLesson;

//if we want OpenGL to repaint with the screens refresh rate, we use this render loop
- (void) startRenderLoop;
- (void) stopRenderLoop;
@end

//This is our delegate class. It handles all messages from the device's operating system
@interface Lesson01AppDelegate : NSObject <UIApplicationDelegate> {
@private
    //we store a pointer to our lesson so we can delete it at program shutdown
    Lesson *lesson;
}

//we configure these variables in the interface builder (IB), thus they have to be declared as IBOutlet
//properties get accessor methods by synthesizing them in the source file (.mm)

//in this window we will embed a view which acts as OpenGL context
@property (nonatomic, retain) IBOutlet UIWindow *window;

//our main window, covering the whole screen
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

