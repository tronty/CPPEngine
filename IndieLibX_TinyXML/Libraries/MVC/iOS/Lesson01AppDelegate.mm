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

#import "Lesson01AppDelegate.h"

#import "EAGLView.h"
#include "Lesson01.h"

//now we implement all methods needed by our delegate
@implementation Lesson01AppDelegate

//
@synthesize window;
@synthesize glView;

//this method tells us, that our application has started and we can set up our OpenGL things,
//as the window is set up, and thus our glView is going to be displayed soon
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    //for the first lesson we don't need a depth buffer as we're not drawing any geometry yet
    [glView setDepthBufferNeeded:FALSE];
    
    //we create our lesson which contains the OpenGL code
    //(allocated with new -> has to be cleaned up with delete!)
    lesson = new Lesson01();
    
    //we tell our OpenGL view which lesson we want to use for rendering.
    [glView setLesson:lesson];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    //the app is going to be suspended to the background,
    //so we should stop our render loop
    [glView stopRenderLoop];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    //we could do something here when the application entered the background
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    //we could start preparing stuff for becoming active again
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    //we're on stage! so let's draw some nice stuff
    [glView startRenderLoop];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    //before shutdown we stop the render loop
    [glView stopRenderLoop];
}

//dealloc is the destructor in Objective-C, so clean up all allocated things
- (void)dealloc
{
    [window release];
    [glView release];
    delete lesson;
    [super dealloc];
}

@end
