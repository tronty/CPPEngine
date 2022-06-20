#ifdef _MSC_VER
#include <stdio.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
int main(int argc, char *argv[])
{
	int s9=sizeof(D3DVERTEXELEMENT9);
	printf("sizeof(D3DVERTEXELEMENT9)=%d\n", s9);
	return 0;
}
#else
#include <Framework3/IRenderer.h>
#include "SDKmesh.h"
int ApplicationLogic()
{
	int s9=sizeof(D3DVERTEXELEMENT9);
	printf("sizeof(D3DVERTEXELEMENT9)=%d\n", s9);
	return 0;
}
#endif

