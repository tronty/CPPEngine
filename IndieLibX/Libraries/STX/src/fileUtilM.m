/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <stdio.h>
#include <stdlib.h>
#include <Foundation/Foundation.h>

const char *iosDocumentsFolder() 
{
	NSURL* fileURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
	const char *filePath = [fileURL.path UTF8String];

	printf("STX filePath: %s\n", filePath);
	NSLog(@"STX filePath: %@", [NSString stringWithUTF8String:filePath]);

	return filePath;
}
void iosLog(const char * msg)
{
	NSString *string = [[NSString alloc] initWithUTF8String:msg];
	NSLog(@"%@", string);
}

