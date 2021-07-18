#import <UIKit/UIKit.h>

int main(int argc, char **argv)
{
	int ret = 0;

	NSURL* fileURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
	const char *filePath = [fileURL.path UTF8String];

	printf("filePath: %s\n", filePath);
	NSLog(@"filePath: %@", [NSString stringWithUTF8String:filePath]);

	return ret;
}
