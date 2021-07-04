#import <UIKit/UIKit.h>

#import <STX/iOS.h>

int main (int argc, char * argv[]) {
	int retVal = 0;
    @autoreleasepool {
        NSLog(@"Running app");
    	retVal = UIApplicationMain(argc, argv, nil, nil);
    	/* [pool release]; */
    }
    return retVal;
}

