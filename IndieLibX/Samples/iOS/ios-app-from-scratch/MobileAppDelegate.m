/*
 * This file is part of the iOSAppsFromScratch project.
 *
 * Copyright (C) 2010 Sean Nelson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
/*
- (void)applicationDidFinishLaunching:(id)unused;
- (void)applicationWillTerminate;
- (void)applicationWillSuspend;
- (void)deviceOrientationChanged:(GSEvent*)event;
- (void)applicationResume:(GSEvent*)event;
- (void)applicationSuspend:(GSEvent*)event;
- (void)menuButtonUp:(GSEvent*)event;
- (void)menuButtonDown:(GSEvent*)event;
- (BOOL)launchApplicationWithIdentifier:(NSString*)identifier suspended:(BOOL)flag;
- (void)openURL:(NSURL*)url;
- (void)openURL:(NSURL*)url asPanel:(BOOL)flag;
*/
#import "MobileAppDelegate.h"
#import "MainView.h"
@implementation MobileAppDelegate
@synthesize window;
@synthesize mainViewController;
- (id)init
{
	if  ((self = [super init]))
	{
		window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
		mainViewController = [[MainView alloc] init];
	}
	return self;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	[window addSubview:mainViewController.view];
	[window makeKeyAndVisible];
	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
}

- (void)dealloc
{
	[mainViewController release];
	[window release];
	[super dealloc];
}

@end
