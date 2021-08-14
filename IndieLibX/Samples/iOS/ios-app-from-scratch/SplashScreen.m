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

#import "SplashScreenController.h"

@implementation SplashScreenController
- (void) loadView
{
}

- (void) displayScreen
{
	UIViewController *displayViewController = [[UIViewController alloc] init];
	displayViewController.view = displaySplashScreen;
	[self presentModelViewController:displayViewController animated:NO];
	[self performSelector:@selector(removeScreen) withObject:nil afterDelay:6.0];
}

- (void) removeScreen
{
	[[self modelViewController] dismissModelViewControllerAnimated:YES];
}

@end
/*
- (void)fadeOutAfterDelay:(NSTimeInterval)delay
{
    [self performSelector:@selector(fadeOut) withObject:nil afterDelay:delay];
}

- (void)fadeOut
{    
    [UIView beginAnimations:nil    context:NULL];
    [UIView setAnimationDuration:.5];
    [self setAlpha:0];
    [UIView commitAnimations];
    
    [self performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:1.5];    
}
*/
