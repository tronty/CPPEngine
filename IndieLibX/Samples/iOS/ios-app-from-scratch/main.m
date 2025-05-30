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
#import <UIKit/UIKit.h>

int main(int argc, char **argv)
{
	int ret = 0;
	NSURL* fileURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
	const char *filePath = [fileURL.path UTF8String];

	printf("filePath: %s\n", filePath);
	NSLog(@"filePath: %@", [NSString stringWithUTF8String:filePath]);

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	ret = UIApplicationMain(argc, argv, @"MobileApp", @"MobileAppDelegate");

	[pool release];
	return ret;
}
