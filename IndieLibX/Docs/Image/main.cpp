#include <stdio.h>
#include <Framework3/IRenderer.h>
int main()
{
	#ifdef ANDROID
	printf("ANDROID BUILD\n");
	#endif
	#ifdef IPHONE_BUILD
	printf("IPHONE_BUILD\n");
	#endif
	#ifdef LINUX
	printf("LINUX_BUILD\n");
	#endif
}

