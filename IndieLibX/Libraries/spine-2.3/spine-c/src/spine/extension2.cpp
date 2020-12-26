#include <spine/spine-sfml.h>
#include <spine/extension.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
void _spAtlasPage_createTexture (spAtlasPage* self, const char* path);
char* __readFile__ (const char* path, int* length);
#ifdef __cplusplus
}
#endif
char* __readFile__ (const char* path, int* length) {
	char *data;
	//STX_PRINT("spine path=%s\n", path);
	FILE *file = fopen(path, "rb");
	if (!file) return 0;

	fseek(file, 0, SEEK_END);
	*length = (int)ftell(file);
	fseek(file, 0, SEEK_SET);

	data = MALLOC(char, *length);
	fread(data, 1, *length, file);
	fclose(file);

	return data;
}
void _spAtlasPage_createTexture (spAtlasPage* self, const char* path)
{
	//_spAtlasPage_createTexture` Loads a texture and stores it and its size in the void* rendererObject

	TextureID id=-1;
	#if 0
	id=IRenderer::GetRendererInstance()->addImageLibTexture(
		path
		,false,
		//IRenderer::GetRendererInstance()->GetbilinearClamp());
		IRenderer::GetRendererInstance()->Getlinear());
	#else
	Image3 img;
	img.loadImageLibImage(path, false);
	img.flipY();
	id=IRenderer::GetRendererInstance()->addTexture(&img);
	#endif
	#if 0
	STX_PRINT("%s:\n%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	STX_PRINT("_spAtlasPage_createTexture: %x %s\n", id, path);
	#endif

	self->width=IRenderer::GetRendererInstance()->GetTextureWidth (id);
	self->height=IRenderer::GetRendererInstance()->GetTextureHeight(id);

	#if 0
	STX_PRINT("self->width =%d\n", self->width);
	STX_PRINT("self->height=%d\n", self->height);
	#endif

	spine::spineTexture* tex=new spine::spineTexture();
	tex->id=id;
	#if 0
	tex->width=self->width;
	tex->height=self->height;
	#else
	tex->width=IRenderer::GetRendererInstance()->GetTextureWidth (id);
	tex->height=IRenderer::GetRendererInstance()->GetTextureHeight(id);
	#endif
	#if 0
	STX_PRINT("tex->width =%d\n", tex->width);
	STX_PRINT("tex->height=%d\n", tex->height);
	#endif
	self->rendererObject=tex;
}

