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

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"
#include "Lesson.h"

//declare private methods, so they can be used everywhere in this file
@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end


//start the actual implementation of our view here
@implementation EAGLView

//generate getter and setter for the context
@synthesize context;

// We have to implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//our EAGLView is the view in our MainWindow which will be automatically loaded to be displayed.
//when the EAGLView gets loaded, it will be initialized by calling this method.
- (id)initWithCoder:(NSCoder*)coder
{
    //call the init method of our parent view
    self = [super initWithCoder:coder];
    
    //now we create the core animation EAGL layer
	if (!self) {
        return nil;
    }

    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    //we don't want a transparent surface
    eaglLayer.opaque = TRUE;
    
    //here we configure the properties of our canvas, most important is the color depth RGBA8 !
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                    nil];
    
    //create an OpenGL ES 2 context
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    //if this failed or we cannot set the context for some reason, quit
    if (!context || ![EAGLContext setCurrentContext:context]) {
        NSLog(@"Could not create context!");
        [self release];
        return nil;
    }

    //do we want to use a depth buffer?
    //for 3D applications we usually do, so we'll set it to true by default
    useDepthBuffer = FALSE;
    
    //we did not initialize our lesson yet:
    lessonIsInitialized = FALSE;
    
    //default values for our OpenGL buffers
    defaultFramebuffer = 0;
    colorRenderbuffer = 0;
    depthRenderbuffer = 0;
    
    
    return self;
}

//on iOS, all rendering goes into a renderbuffer,
//which is then copied to the window by "presenting" it.
//here we create it!
- (void)createFramebuffer
{
    //this method assumes, that the context is valid and current, and that the default framebuffer has not been created yet!
    //this works, because as soon as we call glGenFramebuffers the value will be > 0
    assert(defaultFramebuffer == 0);
    
    NSLog(@"EAGLView: creating Framebuffer");
    
    // Create default framebuffer object and bind it
    glGenFramebuffers(1, &defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    // Create color render buffer
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    //get the storage from iOS so it can be displayed in the view
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    //get the frame's width and height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
    
    //attach this color buffer to our framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    
    //our lesson needs to know the size of the renderbuffer so it can work with the right aspect ratio
    if(lesson != NULL)
    {
        lesson->setRenderbufferSize(framebufferWidth, framebufferHeight);
    }
    
    if(useDepthBuffer)
    {
        //create a depth renderbuffer
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        //create the storage for the buffer, optimized for depth values, same size as the colorRenderbuffer
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
        //attach the depth buffer to our framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }
    
    //check that our configuration of the framebuffer is valid
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

//deleting the framebuffer and all the buffers it contains
- (void)deleteFramebuffer
{
    //we need a valid and current context to access any OpenGL methods
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        //if the default framebuffer has been set, delete it.
        if (defaultFramebuffer) {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        //same for the renderbuffers, if they are set, delete them
        if (colorRenderbuffer) {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
        
        if (depthRenderbuffer) {
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            depthRenderbuffer = 0;
        }
    }
}


//this is where all the magic happens!
- (void)drawFrame
{
    //we need a context for rendering
    if (context != nil)
    {
        //make it the current context for rendering
        [EAGLContext setCurrentContext:context];
        
        //if our framebuffers have not been created yet, do that now!
        if (!defaultFramebuffer)
            [self createFramebuffer];

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        //we need a lesson to be able to render something
        if(lesson != nil)
        {
            //check whether we have to initialize the lesson
            if(lessonIsInitialized == FALSE)
            {
                lesson->init();
                lessonIsInitialized = TRUE;
            }
            
            //perform the actual drawing!
            lesson->draw();
        }
        
        //finally, get the color buffer we rendered to, and pass it to iOS
        //so it can display our awesome results!
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [context presentRenderbuffer:GL_RENDERBUFFER];
    }
    else
        NSLog(@"Context not set!");
}

//our render loop just tells the iOS device that we want to keep refreshing our view all the time
- (void)startRenderLoop
{
    //check whether the loop is already running
    if(displayLink == nil)
    {
        //the display link specifies what to do when the screen has to be redrawn,
        //here we use the selector (method) drawFrame
        displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(drawFrame)];
        
        //by adding the display link to the run loop our draw method will be called 60 times per second
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        NSLog(@"Starting Render Loop");
    }
}

//we have to be able to stop the render loop
- (void)stopRenderLoop
{
    if (displayLink != nil) {
        //if the display link is present, we invalidate it (so the loop stops)
        [displayLink invalidate];
        displayLink = nil;
        NSLog(@"Stopping Render Loop");
    }
}

//setter methods, should be straightforward
- (void) setDepthBufferNeeded:(BOOL)needed
{
    useDepthBuffer = needed;
}

- (void) setLesson:(Lesson*)newLesson
{
    lesson = newLesson;
    //if we set a new lesson, it is not yet initialized!
    lessonIsInitialized = FALSE;
}

//As soon as the view is resized or new subviews are added, this method is called,
//apparently the framebuffers are invalid in this case so we delete them
//and have them recreated the next time we draw to them
- (void)layoutSubviews
{
    [self deleteFramebuffer];
}

//cleanup our view
- (void)dealloc
{
    [self deleteFramebuffer];    
    [context release];
    [super dealloc];
}



@end
