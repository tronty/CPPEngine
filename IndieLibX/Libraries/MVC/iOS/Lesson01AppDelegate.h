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

//we want to create variables of these classes, but don't need their implementation yet,
//so we just tell the compiler that they exist - called forward declaration
@class EAGLView;
class Lesson;

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
