
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/d3dx9mathFromWine.h>
/*
‘isPowerOf2’
‘rgbeToRGB’
‘half’
‘rgbToRGBE8’
‘rgbToRGB9E5’
‘saturate’
*/
#if 1
#include <STX/Matrix.h>
#else
struct half {
	unsigned short sh;
	half(){};
	half(const float x){
	union {
		float floatI;
		unsigned int i;
	};
	floatI = x;

//	unsigned int i = *((unsigned int *) &x);
	int e = ((i >> 23) & 0xFF) - 112;
	int m =  i & 0x007FFFFF;

	sh = (i >> 16) & 0x8000;
	if (e <= 0){
		// Denorm
		m = ((m | 0x00800000) >> (1 - e)) + 0x1000;
		sh |= (m >> 13);
	} else if (e == 143){
		sh |= 0x7C00;
		if (m != 0){
			// NAN
			m >>= 13;
			sh |= m | (m == 0);
		}
	} else {
		m += 0x1000;
		if (m & 0x00800000){
			// Mantissa overflow
			m = 0;
			e++;
		}
		if (e >= 31){
			// Exponent overflow
			sh |= 0x7C00;
		} else {
			sh |= (e << 10) | (m >> 13);
		}
	}
}

	operator float () const {
	union {
		unsigned int s;
		float result;
	};

	s = (sh & 0x8000) << 16;
	unsigned int e = (sh >> 10) & 0x1F;
	unsigned int m = sh & 0x03FF;

	if (e == 0){
		// +/- 0
		if (m == 0) return result;

		// Denorm
		while ((m & 0x0400) == 0){
			m += m;
			e--;
		}
		e++;
		m &= ~0x0400;
	} else if (e == 31){
		// INF / NAN
		s |= 0x7F800000 | (m << 13);
		return result;
	}

	s |= ((e + 112) << 23) | (m << 13);

	return result;
}
};

// Note: returns true for 0
inline bool isPowerOf2(const int x){
	return (x & (x - 1)) == 0;
}

#define saturate(x) clamp(x, 0, 1)

float clamp(const float v, const float c0, const float c1){
	return min(max(v, c0), c1);
}

vec2 clamp(const vec2 &v, const float c0, const float c1){
	return vec2(min(max(v.x, c0), c1), min(max(v.y, c0), c1));
}

vec2 clamp(const vec2 &v, const vec2 &c0, const vec2 &c1){
	return vec2(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y));
}

vec3 clamp(const vec3 &v, const float c0, const float c1){
	return vec3(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1));
}

vec3 clamp(const vec3 &v, const vec3 &c0, const vec3 &c1){
	return vec3(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z));
}

vec4 clamp(const vec4 &v, const float c0, const float c1){
	return vec4(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1), min(max(v.z, c0), c1));
}

vec4 clamp(const vec4 &v, const vec4 &c0, const vec4 &c1){
	return vec4(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z), min(max(v.w, c0.w), c1.w));
}

unsigned int toRGBA(const vec4 &u){
	return (int(u.x * 255) | (int(u.y * 255) << 8) | (int(u.z * 255) << 16) | (int(u.w * 255) << 24));
}

unsigned int toBGRA(const vec4 &u){
	return (int(u.z * 255) | (int(u.y * 255) << 8) | (int(u.x * 255) << 16) | (int(u.w * 255) << 24));
}

vec3 rgbeToRGB(ubyte *rgbe){
	if (rgbe[3]){
		return vec3(rgbe[0], rgbe[1], rgbe[2]) * ldexpf(1.0f, rgbe[3] - (int) (128 + 8));
	} else return vec3(0, 0, 0);
}

unsigned int rgbToRGBE8(const vec3 &rgb){
	float v = max(rgb.x, rgb.y);
	v = max(v, rgb.z);

	if (v < 1e-32f){
		return 0;
	} else {
		int ex;
		float m = frexpf(v, &ex) * 256.0f / v;

		unsigned int r = (unsigned int) (m * rgb.x);
		unsigned int g = (unsigned int) (m * rgb.y);
		unsigned int b = (unsigned int) (m * rgb.z);
		unsigned int e = (unsigned int) (ex + 128);

		return r | (g << 8) | (b << 16) | (e << 24);
	}
}

unsigned int rgbToRGB9E5(const vec3 &rgb){
	float v = max(rgb.x, rgb.y);
	v = max(v, rgb.z);

	if (v < 1.52587890625e-5f){
		return 0;
	} else if (v < 65536){
		int ex;
		float m = frexpf(v, &ex) * 512.0f / v;

		unsigned int r = (unsigned int) (m * rgb.x);
		unsigned int g = (unsigned int) (m * rgb.y);
		unsigned int b = (unsigned int) (m * rgb.z);
		unsigned int e = (unsigned int) (ex + 15);

		return r | (g << 9) | (b << 18) | (e << 27);
	} else {
		unsigned int r = (rgb.x < 65536)? (unsigned int) (rgb.x * (1.0f / 128.0f)) : 0x1FF;
		unsigned int g = (rgb.y < 65536)? (unsigned int) (rgb.y * (1.0f / 128.0f)) : 0x1FF;
		unsigned int b = (rgb.z < 65536)? (unsigned int) (rgb.z * (1.0f / 128.0f)) : 0x1FF;
		unsigned int e = 31;

		return r | (g << 9) | (b << 18) | (e << 27);
	}
}
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//include <tga.h>

#ifndef NO_JPEG
extern "C" {
#include <jpeglib.h>
}
#endif // NO_JPEG

#ifndef NO_PNG
#include "png.h"

int getMipMapCountFromDim(int maxDim){
	int i = 0;

	while (maxDim > 0){
		maxDim >>= 1;
		i++;
	}
	return i;
}


void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length){
	fwrite(data, length, 1, (FILE *) png_get_io_ptr(png_ptr));
}

void user_flush_data(png_structp png_ptr){
	fflush((FILE *) png_get_io_ptr(png_ptr));
}

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length){
	fread(data, length, 1, (FILE *) png_get_io_ptr(png_ptr));
}

png_voidp malloc_fn(png_structp png_ptr, png_size_t size){
	return malloc(size);
}
void free_fn(png_structp png_ptr, png_voidp ptr){
	free(ptr);
}

#endif // NO_PNG

#pragma pack (push, 1)

struct HTexHeader {
	uint32 identifier;
	uint32 version;
	uint64 format;
	uint32 width;
	uint32 height;
	uint32 depth;
	uint32 nMipMaps;
};

struct HTexHeader2 {
	uint32 flags;
	uint32 nExtraData;
};

#define DDPF_ALPHAPIXELS 0x00000001 
#define DDPF_FOURCC      0x00000004 
#define DDPF_RGB         0x00000040

#define DDSD_CAPS        0x00000001
#define DDSD_HEIGHT      0x00000002
#define DDSD_WIDTH       0x00000004
#define DDSD_PITCH       0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_LINEARSIZE  0x00080000
#define DDSD_DEPTH       0x00800000

#define DDSCAPS_COMPLEX  0x00000008 
#define DDSCAPS_TEXTURE  0x00001000 
#define DDSCAPS_MIPMAP   0x00400000 

#define DDSCAPS2_CUBEMAP 0x00000200 
#define DDSCAPS2_VOLUME  0x00200000 

#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define D3D10_RESOURCE_MISC_TEXTURECUBE 0x4
#define D3D10_RESOURCE_DIMENSION_BUFFER    1
#define D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define D3D10_RESOURCE_DIMENSION_TEXTURE3D 4

struct DDSHeader {
	uint32 dwMagic;
	uint32 dwSize;
	uint32 dwFlags;
	uint32 dwHeight;
	uint32 dwWidth;
	uint32 dwPitchOrLinearSize;
	uint32 dwDepth; 
	uint32 dwMipMapCount;
	uint32 dwReserved[11];

	struct {
        uint32 dwSize;
		uint32 dwFlags;
		uint32 dwFourCC;
		uint32 dwRGBBitCount;
		uint32 dwRBitMask;
		uint32 dwGBitMask;
		uint32 dwBBitMask;
		uint32 dwRGBAlphaBitMask; 
	} ddpfPixelFormat;

	struct {
		uint32 dwCaps1;
		uint32 dwCaps2;
		uint32 Reserved[2];
	} ddsCaps;

	uint32 dwReserved2;
};

struct DDSHeaderDX10 {
    uint32 dxgiFormat;
    uint32 resourceDimension;
    uint32 miscFlag;
    uint32 arraySize;
    uint32 reserved;
};
#if 1
struct TGAHeader {
	uint8  descriptionlen;
	uint8  cmaptype;
	uint8  imagetype;
	uint16 cmapstart;
	uint16 cmapentries;
	uint8  cmapbits;
	uint16 xoffset;
	uint16 yoffset;
	uint16 width;
	uint16 height;
	uint8  bpp;
	uint8  attrib;
};
#endif

struct BMPHeader {
	uint16 bmpIdentifier;
	uint8  junk[16];
	uint32 width;
	uint32 height;
	uint16 junk2;
	uint16 bpp;
	uint16 compression;
	uint8  junk3[22];
};

struct PCXHeader {
	uint8  junk[3];
	uint8  bitsPerChannel;
	uint8  junk2[4];
	uint16 width;
	uint16 height;
	uint8  junk3[53];
	uint8  nChannels;
	uint16 scanlineLen;
	uint8  junk4[60];
};

#pragma pack (pop)


/* ---------------------------------------------- */

struct FormatString {
	FORMAT format;
	char *string;
};

static const FormatString formatStrings[] = {
	{ FORMAT_NONE,   "NONE"  },

	{ FORMAT_R8,     "R8"    },
	{ FORMAT_RG8,    "RG8"   },
	{ FORMAT_RGB8,   "RGB8"  },
	{ FORMAT_RGBA8,  "RGBA8" },
	
	{ FORMAT_R16,    "R16"   },
	{ FORMAT_RG16,   "RG16"  },
	{ FORMAT_RGB16,  "RGB16" },
	{ FORMAT_RGBA16, "RGBA16"},

	{ FORMAT_R16F,   "R16F"    },
	{ FORMAT_RG16F,  "RG16F"   },
	{ FORMAT_RGB16F, "RGB16F"  },
	{ FORMAT_RGBA16F,"RGBA16F" },

	{ FORMAT_R32F,   "R32F"    },
	{ FORMAT_RG32F,  "RG32F"   },
	{ FORMAT_RGB32F, "RGB32F"  },
	{ FORMAT_RGBA32F,"RGBA32F" },

	{ FORMAT_RGBE8,  "RGBE8"   },
	{ FORMAT_RGB565, "RGB565"  },
	{ FORMAT_RGBA4,  "RGBA4"   },
	{ FORMAT_RGB10A2,"RGB10A2" },

	{ FORMAT_DXT1,   "DXT1"  },
	{ FORMAT_DXT3,   "DXT3"  },
	{ FORMAT_DXT5,   "DXT5"  },
	{ FORMAT_ATI1N,  "ATI1N" },
	{ FORMAT_ATI2N,  "ATI2N" },
};

const char *getFormatString(const FORMAT format){
	for (unsigned int i = 0; i < elementsOf(formatStrings); i++){
		if (format == formatStrings[i].format) return formatStrings[i].string;
	}
	return NULL;
}

FORMAT getFormatFromString(char *string){
	for (unsigned int i = 0; i < elementsOf(formatStrings); i++){
		if (stricmp(string, formatStrings[i].string) == 0) return formatStrings[i].format;
	}
	return FORMAT_NONE;
}


template <typename DATA_TYPE>
inline void swapChannels(DATA_TYPE *pixels, int nPixels, const int channels, const int ch0, const int ch1){
	do {
		DATA_TYPE tmp = pixels[ch1];
		pixels[ch1] = pixels[ch0];
		pixels[ch0] = tmp;
		pixels += channels;
	} while (--nPixels);
}

#if 1
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

std::map<std::string, unsigned int> Image3::m_Image3_filecache;
std::vector<Image3*> Image3::m_Image3s;

unsigned int Image3::find_from_Image3_cache(const char* fileName)
{
	std::map<std::string, unsigned int>::iterator it;
	it = m_Image3_filecache.find(fileName);
	if (it != m_Image3_filecache.end())
		return it->second;
	return 0;
}

void Image3::insert_to_Image3_cache(const char* fileName, unsigned int id)
{
	std::map<std::string, unsigned int>::iterator it;
	it = m_Image3_filecache.find(fileName);
	if (it == m_Image3_filecache.end())
	{
		m_Image3_filecache.insert ( std::pair<std::string,unsigned int>(fileName, id) );
	}
}



	int Image3::IsImageLoaded () {return (m_id>=0);}
	unsigned int Image3::getPitch(int ml) const 
	{
		//LOG_FNLN;LOG_PRINT("Implement.");
		unsigned int bpp=getChannelCount(format);
		unsigned int pitch=getWidth()*bpp*sizeof(ubyte);
		return pitch;
	}
	void Image3::Dump()
	{
		LOG_FNLN;
		LOG_PRINT("width=%x\n", width);
		LOG_PRINT("height=%x\n", height);
		LOG_PRINT("format=%x\n", format);
		LOG_PRINT("pixels=%x\n", &pixels[0]);
		LOG_PRINT("depth=%x\n", depth);
		LOG_PRINT("nMipMaps=%x\n", nMipMaps);
		LOG_PRINT("arraySize=%x\n", arraySize);
		LOG_PRINT("nExtraData=%x\n", nExtraData);
		LOG_PRINT("extraData=%x\n", &extraData[0]);
	}
#include <stb_image_aug.h>
	bool Image3::loadImageLibImage(const char *filename, const bool swapColors, const bool useMipMaps) 
	{
		#if 0
		std::string f=std::string(filename);
		std::string pName=stx_convertpath(f);
		stx_fileExists(pName.c_str());
		#endif
		LOG_FNLN;
		LOG_PRINT("filename=%s\n", filename);
		unsigned int mImageName=Image3::find_from_Image3_cache(filename);
		if(1)//!mImageName)
		{			
			bool r=false;
			#if 0
			int comp=0;int req_comp=0;
   			FILE *f = fopen(filename, "rb");
   			unsigned char *result;
   			if (!f){r=false;}
			else
			{
   				result = stbi_load_from_file(f,&width,&height,&comp,req_comp);
   				fclose(f);

				if(comp==3)
					format=FORMAT_RGB8;
				else if(comp==4)
					format=FORMAT_RGBA8;
				else
					format=FORMAT_NONE;

				setPixels(result);
				r=true;
			}
			#else
			LOG_FNLN;
			r=loadImage(filename);
			LOG_FNLN;
			#endif			

			if(!r)
				return r;

			if(swapColors)
				swap(0, 2);

			if(useMipMaps)
				createMipMaps(ALL_MIPMAPS);	

			Image3::insert_to_Image3_cache(filename, m_id);
			m_Image3s.push_back(this);
			m_id=m_Image3s.size()-1;
		}
		else
			*this=*m_Image3s[mImageName];
/*
		//LOG_FNLN;
		LOG_PRINT("width=%x\n", width);
		LOG_PRINT("height=%x\n", height);
		LOG_PRINT("pixels=%x\n", pixels);
		LOG_PRINT("format=%x\n", format);
*/
		LOG_FNLN;
		return true;
	}
	bool Image3::saveImageLibImage(const char *fileName, const bool useMipMaps)
	{
		//LOG_FNLN;LOG_PRINT("Implement.");
		return saveImage(fileName);
	}

/* ---------------------------------------------- */

Image3::Image3(){
	m_id=-1;
	pixels.clear();
	width  = 0;
	height = 0;
	depth  = 0;
	nMipMaps = 0;
	arraySize = 0;
	format = FORMAT_NONE;

	nExtraData = 0;
	extraData.clear();
}

Image3::Image3(const Image3 &img){
	m_id=img.m_id;
	width  = img.width;
	height = img.height;
	depth  = img.depth;
	nMipMaps = img.nMipMaps;
	arraySize = img.arraySize;
	format = img.format;

	int size = getMipMappedSize(0, nMipMaps) * arraySize;
	pixels.resize(size);
	stx_memcpy(&pixels[0], &img.pixels[0], size);

	nExtraData = img.nExtraData;
	extraData.clear();
	extraData.resize(nExtraData);
	stx_memcpy(&extraData[0], &img.extraData[0], nExtraData);
		//LOG_FNLN;
		LOG_PRINT("width=%x\n", width);
		LOG_PRINT("height=%x\n", height);
		LOG_PRINT("pixels=%x\n",  &pixels[0]);
		LOG_PRINT("format=%x\n", format);
}

Image3::~Image3(){
	free();
}


	void Image3::setPixels (ubyte * src)
	{
		int size = getMipMappedSize(0, nMipMaps) * arraySize;
		#if 0
		//LOG_FNLN;
		LOG_PRINT("width=%x\n", width);
		LOG_PRINT("height=%x\n", height);
		LOG_PRINT("pixels=%x\n",  pixels);
		LOG_PRINT("format=%x\n", format);
		LOG_PRINT("src=%x\n",  src);
		LOG_PRINT("size=%x\n",  size);
		#endif
		if(src&&size)
		{

			pixels.resize(size);

		//LOG_FNLN;
			stx_memcpy(&pixels[0], src, size);
		//LOG_FNLN;

			//if(format!=FORMAT_RGBA8)
			//	convert(FORMAT_RGBA8);
		}
	}

ubyte *Image3::create(const FORMAT fmt, const int w, const int h, const int d, const int mipMapCount, const int arraysize, D3DXFROMWINEVECTOR3* aColor){
    format = fmt;
    width  = w;
	height = h;
	depth  = d;
	nMipMaps = mipMapCount;
	arraySize = arraysize;
	pixels.clear();
	pixels.resize(getMipMappedSize(0, nMipMaps) * arraySize);
	extraData.clear();
	if(aColor)
	{
	int nIndex=0;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			pixels[nIndex++] = (ubyte)aColor->x;
			pixels[nIndex++] = (ubyte)aColor->y;
			pixels[nIndex++] = (ubyte)aColor->z;
			if(fmt==FORMAT_RGBA8)
				pixels[nIndex++] = (ubyte)255;
		}
	}
	}
	/*
	LOG_FNLN;
	LOG_PRINT("width=%x\n", width);
	LOG_PRINT("height=%x\n", height);
	LOG_PRINT("pixels=%x\n", pixels);
	LOG_PRINT("format=%x\n", format);
	*/
	return &pixels[0];
}

void Image3::free(){
		
	pixels.clear();
	extraData.clear();
}

void Image3::clear(){
	free();

	width  = 0;
	height = 0;
	depth  = 0;
	nMipMaps = 0;
	arraySize = 0;
	format = FORMAT_NONE;

	nExtraData = 0;
}

ubyte *Image3::getPixels(const int mipMapLevel) const {
	return (mipMapLevel < nMipMaps)? (ubyte*) &pixels[0] + getMipMappedSize(0, mipMapLevel) : NULL;
}

ubyte *Image3::getPixels(const int mipMapLevel, const int arraySlice) const {
	if (mipMapLevel >= nMipMaps || arraySlice >= arraySize) return NULL;

	return (ubyte*) &pixels[0] + getMipMappedSize(0, nMipMaps) * arraySlice + getMipMappedSize(0, mipMapLevel);
}


int Image3::getMipMapCountFromDimesions() const {
	int m = max(width, height);
	m = max(m, depth);

	int i = 0;
	while (m > 0){
		m >>= 1;
		i++;
	}

	return i;
}

int Image3::getMipMappedSize(const int firstMipMapLevel, int nMipMapLevels, FORMAT srcFormat) const {
	int w = getWidth (firstMipMapLevel);
	int h = getHeight(firstMipMapLevel);
	int d = getDepth (firstMipMapLevel);

	if (srcFormat == FORMAT_NONE) srcFormat = format;
	
	int size = 0;
	while (nMipMapLevels){
		if (isCompressedFormat(srcFormat)){
			size += ((w + 3) >> 2) * ((h + 3) >> 2) * d;
		} else {
			size += w * h * d;
		}
		w >>= 1;
		h >>= 1;
		d >>= 1;
		if (w + h + d == 0) break;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
		if (d == 0) d = 1;

		nMipMapLevels--;
	}

	if (isCompressedFormat(srcFormat)){
		size *= getBytesPerBlock(srcFormat);
	} else {
		size *= getBytesPerPixel(srcFormat);
	}

	return (depth == 0)? 6 * size : size;
}

int Image3::getSliceSize(const int mipMapLevel, FORMAT srcFormat) const {
	int w = getWidth (mipMapLevel);
	int h = getHeight(mipMapLevel);

	if (srcFormat == FORMAT_NONE) srcFormat = format;
	
	int size;
	if (isCompressedFormat(srcFormat)){
		size = ((w + 3) >> 2) * ((h + 3) >> 2) * getBytesPerBlock(srcFormat);
	} else {
		size = w * h * getBytesPerPixel(srcFormat);
	}

	return size;
}

int Image3::getPixelCount(const int firstMipMapLevel, int nMipMapLevels) const {
	int w = getWidth (firstMipMapLevel);
	int h = getHeight(firstMipMapLevel);
	int d = getDepth (firstMipMapLevel);
	int size = 0;
	while (nMipMapLevels){
		size += w * h * d;
		w >>= 1;
		h >>= 1;
		d >>= 1;
		if (w + h + d == 0) break;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
		if (d == 0) d = 1;

		nMipMapLevels--;
	}

	return (depth == 0)? 6 * size : size;
}

int Image3::getWidth(const int mipMapLevel) const {
	int a = width >> mipMapLevel;
	return (a == 0)? 1 : a;
}

int Image3::getHeight(const int mipMapLevel) const {
	int a = height >> mipMapLevel;
	return (a == 0)? 1 : a;
}

int Image3::getDepth(const int mipMapLevel) const {
	int a = depth >> mipMapLevel;
	return (a == 0)? 1 : a;
}

bool Image3::loadHTEX(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	HTexHeader header;
	fread(&header, sizeof(header), 1, file);

	if (header.identifier != MCHAR4('H','T','E','X') || header.version > 2){
		fclose(file);
		return false;
	}

	HTexHeader2 header2;
	if (header.version >= 2){
		fread(&header2, sizeof(header2), 1, file);
	} else {
		stx_memset(&header2, 0, sizeof(header2));
	}

	format = getFormatFromString((char *) &header.format);
	width  = header.width;
	height = header.height;
	depth  = header.depth;
	nMipMaps = header.nMipMaps;
	arraySize = 1;

	nExtraData = header2.nExtraData;
	
	pixels.resize(getMipMappedSize(0, nMipMaps));

	if (isPlainFormat(format)){
		/*int nChannels = getChannelCount(format);
		int bpc = getBytesPerChannel(format);

		for (int ch = 0; ch < nChannels; ch++){
			for (int level = 0; level < nMipMaps; level++){
				int nPixels = getPixelCount(level, 1);

				ubyte *dest = getPixels(level) + ch * bpc;
				for (int i = 0; i < nPixels; i++){
					fread(dest + i * nChannels * bpc, bpc, 1, file);
				}
			}
		}*/
		fread(&pixels[0], getMipMappedSize(0, nMipMaps), 1, file);

	} else if (isCompressedFormat(format)){
		if (format == FORMAT_DXT1 || format == FORMAT_DXT5){
			char shift = (format == FORMAT_DXT1)? 3 : 4;

			for (int level = 0; level < nMipMaps; level++){
				ubyte *dest = getPixels(level);
				if (format != FORMAT_DXT1) dest += 8;
				int w = getWidth(level);
				int h = getHeight(level);
				int d = getDepth(level);

				int nBlocks = d * ((w + 3) >> 2) * ((h + 3) >> 2);

				for (int n = 0; n < nBlocks; n++){
					fread(dest + (n << shift), 1, 2, file);
				}

				for (int n = 0; n < nBlocks; n++){
					fread(dest + (n << shift) + 2, 1, 2, file);
				}

				for (int n = 0; n < nBlocks; n++){
					fread(dest + (n << shift) + 4, 1, 4, file);
				}
			}
		}
		if (format == FORMAT_DXT5 || format == FORMAT_ATI1N || format == FORMAT_ATI2N){
			char shift = (format != FORMAT_DXT5)? 3 : 4;

			for (int level = 0; level < nMipMaps; level++){
				ubyte *src = getPixels(level);
				int w = getWidth(level);
				int h = getHeight(level);
				int d = getDepth(level);

				int nBlocks = d * ((w + 3) >> 2) * ((h + 3) >> 2);

				for (int n = 0; n < nBlocks; n++){
					fread(src + (n << shift), 1, 1, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fread(src + 8 * (nBlocks + n), 1, 1, file);
					}
				}

				for (int n = 0; n < nBlocks; n++){
					fread(src + (n << shift) + 1, 1, 1, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fread(src + 8 * (nBlocks + n) + 1, 1, 1, file);
					}
				}

				for (int n = 0; n < nBlocks; n++){
					fread(src + (n << shift) + 2, 1, 6, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fread(src + 8 * (nBlocks + n) + 2, 1, 6, file);
					}
				}
			}
		}
	}

	if (nExtraData){
		extraData.resize(nExtraData);
		fread(&extraData[0], 1, nExtraData, file);
	}

	fclose(file);

	return true;
}

bool Image3::loadDDS(const char *fileName, uint flags){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	DDSHeader header;

	FILE *file;
	//LOG_FNLN;
	if ((file = fopen(fileName, "rb")) == NULL) return false;
	//LOG_FNLN;

	fread(&header, sizeof(header), 1, file);
	//LOG_FNLN;
	if (header.dwMagic != MCHAR4('D','D','S',' ')){
		fclose(file);
		return false;
	}
	//LOG_FNLN;
	//LOG_PRINT("header.dwWidth=%x\n", header.dwWidth);
	//LOG_PRINT("header.dwHeight=%x\n", header.dwHeight);
	//LOG_PRINT("header.dwMipMapCount=%x\n", header.dwMipMapCount);
	//LOG_PRINT("header.ddpfPixelFormat.dwRGBBitCount=%x\n", header.ddpfPixelFormat.dwRGBBitCount);

	width  = header.dwWidth;
	height = header.dwHeight;
	depth  = (header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP)? 0 : (header.dwDepth == 0)? 1 : header.dwDepth;
	nMipMaps = ((flags & DONT_LOAD_MIPMAPS) || (header.dwMipMapCount == 0))? 1 : header.dwMipMapCount;
	arraySize = 1;

	if (header.ddpfPixelFormat.dwFourCC == MCHAR4('D','X','1','0')){
		DDSHeaderDX10 dx10Header;
		fread(&dx10Header, sizeof(dx10Header), 1, file);

		switch (dx10Header.dxgiFormat){
			case 61: format = FORMAT_R8; break;
			case 49: format = FORMAT_RG8; break;
			case 28: format = FORMAT_RGBA8; break;

			case 56: format = FORMAT_R16; break;
			case 35: format = FORMAT_RG16; break;
			case 11: format = FORMAT_RGBA16; break;

			case 54: format = FORMAT_R16F; break;
			case 34: format = FORMAT_RG16F; break;
			case 10: format = FORMAT_RGBA16F; break;

			case 41: format = FORMAT_R32F; break;
			case 16: format = FORMAT_RG32F; break;
			case 6:  format = FORMAT_RGB32F; break;
			case 2:  format = FORMAT_RGBA32F; break;

			case 67: format = FORMAT_RGB9E5; break;
			case 26: format = FORMAT_RG11B10F; break;
			case 24: format = FORMAT_RGB10A2; break;

			case 71: format = FORMAT_DXT1; break;
			case 74: format = FORMAT_DXT3; break;
			case 77: format = FORMAT_DXT5; break;
			case 80: format = FORMAT_ATI1N; break;
			case 83: format = FORMAT_ATI2N; break;
			default:
				fclose(file);
				return false;
		}

	} else {

		switch (header.ddpfPixelFormat.dwFourCC){
			case 34:  format = FORMAT_RG16; break;
			case 36:  format = FORMAT_RGBA16; break;
			case 111: format = FORMAT_R16F; break;
			case 112: format = FORMAT_RG16F; break;
			case 113: format = FORMAT_RGBA16F; break;
			case 114: format = FORMAT_R32F; break;
			case 115: format = FORMAT_RG32F; break;
			case 116: format = FORMAT_RGBA32F; break;
			case MCHAR4('D','X','T','1'): format = FORMAT_DXT1; break;
			case MCHAR4('D','X','T','3'): format = FORMAT_DXT3; break;
			case MCHAR4('D','X','T','5'): format = FORMAT_DXT5; break;
			case MCHAR4('A','T','I','1'): format = FORMAT_ATI1N; break;
			case MCHAR4('A','T','I','2'): format = FORMAT_ATI2N; break;
			default:
				switch (header.ddpfPixelFormat.dwRGBBitCount){
					case 8: format = FORMAT_I8; break;
					case 16:
						format = (header.ddpfPixelFormat.dwRGBAlphaBitMask == 0xF000)? FORMAT_RGBA4 : 
								 (header.ddpfPixelFormat.dwRGBAlphaBitMask == 0xFF00)? FORMAT_IA8 : 
								 (header.ddpfPixelFormat.dwBBitMask == 0x1F)? FORMAT_RGB565 : FORMAT_I16;
						break;
					case 24: format = FORMAT_RGB8; break;
					case 32:
						format = (header.ddpfPixelFormat.dwRBitMask == 0x3FF00000)? FORMAT_RGB10A2 : FORMAT_RGBA8;
						break;
					default:
						fclose(file);
						return false;
				}
		}
	}
	//LOG_FNLN;

	int size = getMipMappedSize(0, nMipMaps);
	pixels.resize(size);
	//LOG_FNLN;
	//LOG_PRINT("size=%x\n", size);
	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
                ubyte *src = getPixels(mipMapLevel) + face * faceSize;

				fread(src, 1, faceSize, file);
			}
			if ((flags & DONT_LOAD_MIPMAPS) && header.dwMipMapCount > 1){
				fseek(file, getMipMappedSize(1, header.dwMipMapCount - 1) / 6, SEEK_CUR);
			}
		}
	} else {
		fread(&pixels[0], 1, size, file);
	}
	//LOG_FNLN;

	if ((format == FORMAT_RGB8 || format == FORMAT_RGBA8) && header.ddpfPixelFormat.dwBBitMask == 0xFF){
		int nChannels = getChannelCount(format);
		swapChannels(&pixels[0], size / nChannels, nChannels, 0, 2);
	}
	//LOG_FNLN;

	fclose(file);
	return true;
}

#ifndef NO_HDR

bool Image3::loadHDR(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	char *str, header[256];
	fread(header, 1, sizeof(header), file);
	header[255] = '\0';

	if ((str = strstr(header, "-Y ")) != NULL){
		height = atoi(str + 3);
		if ((str = strstr(header, "+X ")) != NULL){
			str += 3;
            width = atoi(str);
			// Find the end of the width field, which should be where the header ends
			while (*str >= '0' && *str <= '9') str++;
			uint headerSize = (uint) (str + 1 - header);

			depth = 1;
			nMipMaps = 1;
			arraySize = 1;
			format = FORMAT_RGBE8;

			// Load the whole file
			fseek(file, 0, SEEK_END);
			int size = ftell(file) - headerSize;
			fseek(file, headerSize, SEEK_SET);
			ubyte *buffer = new ubyte[size];
			fread(buffer, 1, size, file);
			fclose(file);

			// No RLE encoding
			if (width < 8 || width > 0x7FFF || buffer[0] != 2 || buffer[1] != 2){
				pixels = buffer;
			} else {
				int w = width * 4;

				pixels.resize(w * height];
				ubyte *src = buffer;

				for (int j = 0; j < height; j++){
					src += 4;

					ubyte *sEnd = &pixels[0] + (j + 1) * w;
					for (int i = 0; i < 4; i++){
						ubyte *sDest = &pixels[0] + j * w + i;
						do {
							uint count = *src++;
							if (count > 128){
								count -= 128;
								do {
									*sDest = *src;
									sDest += 4;
								} while (--count);
								src++;
							} else {
								do {
									*sDest = *src++;
									sDest += 4;
								} while (--count);
							}
						} while (sDest < sEnd);
					}
				}

		        delete [] buffer;
			}

			return true;
		}
	}

	fclose(file);
	return false;
}

#endif

#ifndef NO_TIFF
#include <tiffio.h>
#define uint32 uint
bool Image3::loadTIFF(const char *fileName){

            TIFF *tif=TIFFOpen(fileName, "rb");
	if (tif == 0)
		return false;

        unsigned int width_, height_;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width_);           // uint32 width;
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height_);        // uint32 height;

	//std::vector<ubyte> pixels;
	width=width_;
	height=height_;

        uint32 npixels=width_*height_*4;// ??? *4;
	pixels.resize(npixels);
        uint32 * raster=(uint32 *) _TIFFmalloc(npixels *sizeof(uint32));
        TIFFReadRGBAImage(tif, width_, height_, raster, 0);

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */ /*
#define TIFFGetR(abgr) ((abgr) & 0xff)
#define TIFFGetG(abgr) (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr) (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr) (((abgr) >> 24) & 0xff) */

	for (int i = 0; i < npixels; i++)
	{
        	//ubyte X=(ubyte)TIFFGetX(raster[i]);  // where X can be the channels R, G, B, and A.
        	// i is the index of the pixel in the raster.
		ubyte R=(ubyte)TIFFGetR(raster[i/4+0]);
		ubyte G=(ubyte)TIFFGetG(raster[i/4+1]);
		ubyte B=(ubyte)TIFFGetB(raster[i/4+2]);
		ubyte A=(ubyte)TIFFGetA(raster[i/4+3]);
#if 1
		pixels[i]=A+(R<<8)+(G<<16)+(B<<24);
#else
		pixels[i]=R+(G<<8)+(B<<16)+(A<<24);
#endif
	}
        _TIFFfree(raster);
	TIFFClose(tif);
	return false;
}
#endif

#ifndef NO_JPEG
bool Image3::loadJPEG(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE *file;
	//LOG_FNLN;
	if ((file = fopen(fileName, "rb")) == NULL) return false;
	//LOG_FNLN;
	
	jpeg_stdio_src(&cinfo, file);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	//LOG_FNLN;
	//LOG_PRINT("cinfo.num_components=%x\n", cinfo.num_components);

	switch (cinfo.num_components){
		case 1:
			format = FORMAT_I8;
			break;
		case 3:
			format = FORMAT_RGB8;
			break;
		case 4:
			format = FORMAT_RGBA8;
			break;
	}
	width  = cinfo.output_width;
	height = cinfo.output_height;
	depth  = 1;
	nMipMaps = 1;
	arraySize = 1;

	pixels.resize(width * height * cinfo.num_components);
	ubyte *curr_scanline = &pixels[0];

	while (cinfo.output_scanline < cinfo.output_height){
		jpeg_read_scanlines(&cinfo, &curr_scanline, 1);
		curr_scanline += width * cinfo.num_components;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(file);

	return true;
}
#endif // NO_JPEG

#ifndef NO_PNG
bool Image3::loadPNG(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
    FILE *file;
	//LOG_FNLN;

    // open the PNG input file
    if ((file = fopen(fileName, "rb")) == NULL) return false;
	//LOG_FNLN;

    // first check the eight byte PNG signature
    png_byte pbSig[8];
    fread(pbSig, 1, 8, file);
    if (!png_check_sig(pbSig, 8)){
		fclose(file);
		return false;
	}
	//LOG_FNLN;

    // create the two png(-info) structures
    if ((png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL, NULL, malloc_fn, free_fn)) == NULL){
		fclose(file);
        return false;
    }
	//LOG_FNLN;

    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL){
        png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(file);
        return false;
    }
	//LOG_FNLN;

	// initialize the png structure
	png_set_read_fn(png_ptr, file, user_read_data);
	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// get width, height, bit-depth and color-type
	png_uint_32 w, h;
    int bitDepth, colorType;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bitDepth, &colorType, NULL, NULL, NULL);

	width = w;
	height = h;
	depth = 1;
	nMipMaps = 1;
	arraySize = 1;

	int nChannels = png_get_channels(png_ptr, info_ptr);

	//LOG_FNLN;
	//LOG_PRINT("nChannels=%x\n", nChannels);
	switch (nChannels){
		case 1:
			format = (bitDepth > 8)? FORMAT_I16 : FORMAT_I8;
			break;
		case 2:
			format = (bitDepth > 8)? FORMAT_IA16 : FORMAT_IA8;
			break;
		case 3:
			format = (bitDepth > 8)? FORMAT_RGB16 : FORMAT_RGB8;
			break;
		case 4:
			format = (bitDepth > 8)? FORMAT_RGBA16 : FORMAT_RGBA8;
			break;
	}

	int rowSize = width * nChannels * bitDepth / 8;

	// now we can allocate memory to store the image
	pixels.resize(rowSize * height);
	
	// set the individual row-pointers to point at the correct offsets
    png_byte **ppbRowPointers = new png_bytep[height];
	for (int i = 0; i < height; i++)
		ppbRowPointers[i] = &pixels[0] + i * rowSize;

	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);
	
	delete [] ppbRowPointers;

	if (colorType == PNG_COLOR_TYPE_PALETTE){
		png_colorp palette;
		int num_palette;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

		std::vector<ubyte> newPixels(width * height * 3);
		if (bitDepth == 4){
			for (int i = 0; i < rowSize * height; i++){
				uint i0 = pixels[i] >> 4;
				uint i1 = pixels[i] & 0xF;
				newPixels[6 * i    ] = palette[i0].red;
				newPixels[6 * i + 1] = palette[i0].green;
				newPixels[6 * i + 2] = palette[i0].blue;
				newPixels[6 * i + 3] = palette[i1].red;
				newPixels[6 * i + 4] = palette[i1].green;
				newPixels[6 * i + 5] = palette[i1].blue;
			}
		} else {
			for (int i = 0; i < rowSize * height; i++){
				newPixels[3 * i    ] = palette[pixels[i]].red;
				newPixels[3 * i + 1] = palette[pixels[i]].green;
				newPixels[3 * i + 2] = palette[pixels[i]].blue;
			}
		}
		format = FORMAT_RGB8;

		pixels.resize(0);
		pixels = newPixels;
	}

	if (bitDepth == 16){
		// Fix endian
		int size = width * height * nChannels * sizeof(ushort);
		for (int i = 0; i < size; i += 2){
			ubyte tmp = pixels[i];
			pixels[i] = pixels[i + 1];
			pixels[i + 1] = tmp;
		}
	}

	// and we're done
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(file);

    return true;
}
#endif // NO_PNG

#ifndef NO_TGA
bool Image3::loadTGA(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
#if 1
	TGAHeader header;

	int size, x, y, pixelSize, palLength;
	std::vector<ubyte> tempBuffer, fBuffer;
	ubyte * dest, *src;
	unsigned int tempPixel;
	ubyte palette[768];
	FILE *file;
	//LOG_FNLN;


	if ((file = fopen(fileName, "rb")) == NULL) return false;
	//LOG_FNLN;
	
	// Find file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the header
	fread(&header, sizeof(header), 1, file);
	
	width  = header.width;
	height = header.height;
	depth  = 1;
	nMipMaps = 1;
	arraySize = 1;

	pixelSize = header.bpp / 8;

	if ((palLength = header.descriptionlen + header.cmapentries * header.cmapbits / 8) > 0) fread(palette, sizeof(palette), 1, file);

	//LOG_FNLN;
	//LOG_PRINT("header.width=%x\n", header.width);
	//LOG_PRINT("header.height=%x\n", header.height);
	//LOG_PRINT("header.bpp=%x\n", header.bpp);
	//LOG_PRINT("header.imagetype=%x\n", header.imagetype);
	
	// Read the file data
	fBuffer.resize(size - sizeof(header) - palLength);
	fread(&fBuffer[0], size - sizeof(header) - palLength, 1, file);
	fclose(file);

	size = width * height * pixelSize;

	tempBuffer.resize(size);
	//LOG_FNLN;
	//LOG_PRINT("size=%x\n", size);

	// Decode if rle compressed. Bit 3 of .imagetype tells if the file is compressed
	if (header.imagetype & 0x08){
		unsigned int c,count;

		dest = &tempBuffer[0];
		src = &fBuffer[0];

		while (size > 0) {
			// Get packet header
			c = *src++;

			count = (c & 0x7f) + 1;
			size -= count * pixelSize;
			
			if (c & 0x80) {
				// Rle packet
				do {
					stx_memcpy(dest,src,pixelSize);
					dest += pixelSize;
				} while (--count);
				src += pixelSize;
			} else {
				// Raw packet
				count *= pixelSize;
				stx_memcpy(dest,src,count);
				src += count;
				dest += count;
			}
		}
		
		src = &tempBuffer[0];
	} else {
		src = &fBuffer[0];
	}

	src += (header.bpp / 8) * width * (height - 1);

	//LOG_FNLN;
	//LOG_PRINT("header.bpp=%x\n", header.bpp);	

	switch(header.bpp) {
	case 8:
		if (palLength > 0){
			format = FORMAT_RGB8;
			pixels.resize(width * height * 3);
			dest = &pixels[0];
			for (y = 0; y < height; y++){
				for (x = 0; x < width; x++){
					tempPixel = 3 * (*src++);
					*dest++ = palette[tempPixel + 2];
					*dest++ = palette[tempPixel + 1];
					*dest++ = palette[tempPixel];
				}
				src -= 2 * width;
			}
		} else {
			format = FORMAT_I8;
			pixels.resize(width * height);
		dest = &pixels[0];
			for (y = 0; y < height; y++){
				stx_memcpy(dest, src, width);
				dest += width;
				src -= width;
			}
		}
		break;
	case 16:
		format = FORMAT_RGBA8;
		pixels.resize(width * height * 4);
		dest = &pixels[0];
		for (y = 0; y < height; y++){
			for (x = 0; x < width; x++){
				tempPixel = *((unsigned short *) src);

				dest[0] = ((tempPixel >> 10) & 0x1F) << 3;
				dest[1] = ((tempPixel >>  5) & 0x1F) << 3;
				dest[2] = ((tempPixel      ) & 0x1F) << 3;
				dest[3] = ((tempPixel >> 15) ? 0xFF : 0);
				dest += 4;
				src += 2;
			}
			src -= 4 * width;
		}
		break;
	case 24:
		format = FORMAT_RGB8;
		pixels.resize(width * height * 3);
		dest = &pixels[0];
		for (y = 0; y < height; y++){
			for (x = 0; x < width; x++){
				*dest++ = src[2];
				*dest++ = src[1];
				*dest++ = src[0];
				src += 3;
			}
			src -= 6 * width;
		}
		break;
	case 32:
		format = FORMAT_RGBA8;
		pixels.resize(width * height * 4);
		dest = &pixels[0];
		for (y = 0; y < height; y++){
			for (x = 0; x < width; x++){
				*dest++ = src[2];
				*dest++ = src[1];
				*dest++ = src[0];
				*dest++ = src[3];
				src += 4;
			}
			src -= 8 * width;
		}
		break;
	}
	//LOG_FNLN;

	tempBuffer.clear();
	fBuffer.clear();
#else
        TGA *tga;
	TGAData *data;
        
	if(argc < 2) {
		LOG_PRINT("No input file!\n");
		return 0;
	}

	data = (TGAData*)malloc(sizeof(TGAData));
	if(!data) {
		TGA_ERROR((TGA*)NULL, TGA_OOM);
		return 0;
	}
	
        LOG_PRINT("[open] name=%s, mode=%s\n", argv[1], "r");
	tga = TGAOpen(argv[1], "r");
	if (!tga || tga->last != TGA_OK) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		return 0;
	}
	
	LOG_PRINT("[read] image\n");
	data->flags = TGA_IMAGE_INFO;
	if (TGAReadImage(tga, data) != TGA_OK) {
		TGA_ERROR(tga, TGA_READ_FAIL);
		return 0;
	}

        if (data->flags & TGA_IMAGE_INFO) {
		LOG_PRINT("[info] width=%i\n", tga->hdr.width);
		LOG_PRINT("[info] height=%i\n", tga->hdr.height);
		
		LOG_PRINT("[info] color map type=%i\n", tga->hdr.map_t);
		LOG_PRINT("-> [text] %s\n", cmap[tga->hdr.map_t]);

		LOG_PRINT("[info] image type=%i\n", tga->hdr.img_t);
		LOG_PRINT("-> [text] %s\n", (tga->hdr.img_t > 8) ? 
			imgt[tga->hdr.img_t - 5] : imgt[tga->hdr.img_t]);

		LOG_PRINT("[info] depth=%i\n", tga->hdr.depth);
		LOG_PRINT("[info] x=%i\n", tga->hdr.x);
		LOG_PRINT("[info] y=%i\n", tga->hdr.y);
			LOG_PRINT("[info] orientation=%s-%s\n",
			       (tga->hdr.vert == TGA_BOTTOM) ?
			       "bottom" : "top",
		 	(tga->hdr.horz == TGA_LEFT) ?
			       "left" : "right");
	}

	LOG_PRINT("[close]\n");
        TGAClose(tga);
#endif
	//LOG_FNLN;
	return true;
}
#endif

#ifndef NO_BMP
bool Image3::loadBMP(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	BMPHeader header;

	FILE *file;
	ubyte *dest, *src, *tmp;
	int i, len;
	ubyte palette[1024];
	//LOG_FNLN;

	if ((file = fopen(fileName, "rb")) == NULL) return false;
	//LOG_FNLN;
	
	// Read the header
	fread(&header, sizeof(header), 1, file);
	//LOG_FNLN;
	if (header.bmpIdentifier != MCHAR2('B', 'M')){
	//LOG_FNLN;
		fclose(file);
		return false;
	}
	//LOG_FNLN;

	width  = header.width;
	height = header.height;
	depth  = 1;
	nMipMaps = 1;
	arraySize = 1;
	//LOG_FNLN;
	//LOG_PRINT("header.bpp=%x\n", header.bpp);

	switch (header.bpp){
	case 8:
	//LOG_FNLN;
		if (header.compression){
	//LOG_FNLN;
			// No support for RLE compressed bitmaps
			fclose(file);
	//LOG_FNLN;
			return false;
		}
	//LOG_FNLN;
		format = FORMAT_RGB8;
		pixels.resize(width * height * 3);
	//LOG_FNLN;

		fread(palette, sizeof(palette), 1, file);
	//LOG_FNLN;
		swapChannels(palette, 256, 4, 0, 2);

		// Normal unencoded 8 bit paletted bitmap
		tmp = new ubyte[width];
	//LOG_FNLN;
		for (i = height - 1; i >= 0; i--){
			dest = &pixels[0] + i * width * 3;
			fread(tmp, width, 1, file);
			len = width;
			do {
				src = palette + ((*tmp++) << 2);
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
			} while (--len);
			tmp -= width;
		}
	//LOG_FNLN;
		delete [] tmp;
	//LOG_FNLN;
		break;
	case 24:
	case 32:
	//LOG_FNLN;
		int nChannels;
		nChannels = (header.bpp == 24)? 3 : 4;
		format    = (header.bpp == 24)? FORMAT_RGB8 : FORMAT_RGBA8;
		pixels.resize(width * height * nChannels);
	//LOG_FNLN;
		for (i = height - 1; i >= 0; i--){
			dest = &pixels[0] + i * width * nChannels;
			fread(dest, width * nChannels, 1, file);
			swapChannels(dest, width, nChannels, 0, 2);
		}
	//LOG_FNLN;
		break;
	default:
		fclose(file);
		return false;
	}
	//LOG_FNLN;

	fclose(file);
	//LOG_FNLN;

	return true;
}
#endif // NO_BMP

#ifndef NO_PCX
bool Image3::loadPCX(const char *fileName){
		//LOG_FNLN;
		//LOG_PRINT("fileName=%s\n", fileName);
	PCXHeader header;

	FILE *file;
	int size, bpp, i, x,y,n, length, len, col;
	ubyte *fBuffer, *src, *palette;

	if ((file = fopen(fileName, "rb")) == NULL) return false;
	
	// Find file size
	fseek(file, 0, SEEK_END);
	size = ftell(file) - sizeof(header);
	fseek(file, 0, SEEK_SET);

	// Read the header
	fread(&header, sizeof(header), 1, file);
	
	width    = header.width  + 1;
	height   = header.height + 1;
	depth    = 1;
	format   = FORMAT_RGB8;
	nMipMaps = 1;
	arraySize = 1;

	bpp = header.nChannels * header.bitsPerChannel;

	pixels.resize(width * height * 3);
	fBuffer = new ubyte[size];
	fread(fBuffer, size, 1, file);
	fclose(file);

	src = fBuffer;

	//LOG_FNLN;
	//LOG_PRINT("bpp=%x\n", bpp);

	switch(bpp){
	case 8:
		palette = fBuffer + size - 768;
		len = width * height * 3;
		i = 0;
		do {
			if (*src < 192){
				col = 3 * (*src++);
				pixels[i++] = palette[col];
				pixels[i++] = palette[col + 1];
				pixels[i++] = palette[col + 2];
			} else {
				length = (*src++) - 192;
				col = 3 * (*src++);
				do {
					pixels[i++] = palette[col];
					pixels[i++] = palette[col + 1];
					pixels[i++] = palette[col + 2];
				} while (--length);
			}
		} while (i < len);
		break;
	case 24:
		len = width * height * 3;
		i = 0;

		x = width;
		y = n = 0;

		do {
			if (x == 0){
				x = width;
				if (n == 2){
					if (++y == height) break;
					n = 0;
				} else n++;
				i = y * width * 3 + n;
			}

			if (*src < 192){
				pixels[i += 3] = *src++;
				x--;
			} else {
				length = (*src++) - 192;
				x -= length;
				do {
					pixels[i += 3] = *src;
				} while (--length);
				src++;
			}

		} while (true);
		break;
	default:
		delete [] fBuffer;
		return false;
	}

	delete [] fBuffer;

	return true;
}
#endif // NO_PCX

bool Image3::loadImage(const char *aFileName, uint flags){
		STX_PRINT("Image3::loadImage:\n");
		STX_PRINT("fileName=%s\n", aFileName);
		std::string fn=aFileName;
		if(aFileName&&aFileName[0]=='/')
			fn=stx_convertpath(aFileName);
		const char *fileName=fn.c_str();
	const char *extension = strrchr(fileName, '.');
	//LOG_FNLN;

	clear();
	//LOG_FNLN;

	if (extension == NULL) return false;
	//LOG_FNLN;
	bool r=true;

	if (stricmp(extension, ".htex") == 0){
		if (!loadHTEX(fileName)) r=false;
	} 
	//LOG_FNLN;
	if (stricmp(extension, ".dds") == 0){
		if (!loadDDS(fileName, flags)) r=false;
	} 
	//LOG_FNLN;
#ifndef NO_HDR
	if (stricmp(extension, ".hdr") == 0){
		if (!loadHDR(fileName)) r=false;
	}
#endif // NO_HDR
	//LOG_FNLN;
#ifndef NO_JPEG
	if (stricmp(extension, ".jpeg") == 0){
		if (!loadJPEG(fileName)) r=false;
	}
	if (stricmp(extension, ".jpg") == 0){
		if (!loadJPEG(fileName)) r=false;
	}
#endif // NO_JPEG
	//LOG_FNLN;
#ifndef NO_PNG
	if (stricmp(extension, ".png") == 0){
		if (!loadPNG(fileName)) r=false;
	}
#endif // NO_PNG
	//LOG_FNLN;
#ifndef NO_TIFF
	if (stricmp(extension, ".tif") == 0 || stricmp(extension, ".tiff") == 0 ){
		if (!loadTIFF(fileName)) r=false;
	}
	if (stricmp(extension, ".tif") == 0 || stricmp(extension, ".tif") == 0 ){
		if (!loadTIFF(fileName)) r=false;
	}
#endif // NO_TIFF
	//LOG_FNLN;
#ifndef NO_TGA
	if (stricmp(extension, ".tga") == 0){
		if (!loadTGA(fileName)) r=false;
	}
#endif // NO_TGA
	//LOG_FNLN;
#ifndef NO_BMP
	if (stricmp(extension, ".bmp") == 0){
	//LOG_FNLN;
		if (!loadBMP(fileName)) r=false;
	//LOG_FNLN;
	}
#endif // NO_BMP
	//LOG_FNLN;
#ifndef NO_PCX
	if (stricmp(extension, ".pcx") == 0){
		if (!loadPCX(fileName)) r=false;
	}
#endif // NO_PCX
	//LOG_FNLN;
	//if(format!=FORMAT_RGBA8)
		//convert(FORMAT_RGBA8);

		/*
		LOG_FNLN;
		LOG_PRINT("width=%x\n", width);
		LOG_PRINT("height=%x\n", height);
		LOG_PRINT("pixels=%x\n",  pixels);
		LOG_PRINT("format=%x\n", format);
		*/
	if(getPixels()==0)
		Dump();
	return r;
}

bool Image3::loadSlicedImage(const char **fileNames, const int nImages, const int nArraySlices, uint flags){
	int maxImage = nImages? nImages : 6;

	Image3 *images = new Image3[maxImage * nArraySlices];

	for (int i = 0; i < maxImage * nArraySlices; i++){
		if (!images[i].loadImage(fileNames[i], flags)){
			delete [] images;
			return false;
		}
	}

	uint nMipMaps = images[0].nMipMaps;
	ubyte *dest = create(images[0].format, images[0].width, images[0].height, nImages, nMipMaps, nArraySlices);

	for (int arraySlice = 0; arraySlice < nArraySlices; arraySlice++){
		int base = arraySlice * maxImage;

		for (uint level = 0; level < nMipMaps; level++){
			int size = images[0].getMipMappedSize(level, 1);
			for (int i = 0; i < maxImage; i++){
				stx_memcpy(dest, images[base + i].getPixels(level), size);
				dest += size;
			}
		}
	}

	delete [] images;

	return true;
}

bool Image3::saveHTEX(const char *fileName){
	if (!(isPlainFormat(format) || format == FORMAT_DXT1 || format == FORMAT_DXT5 || format == FORMAT_ATI1N || format == FORMAT_ATI2N)) return false;

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	HTexHeader header;
	header.identifier = MCHAR4('H','T','E','X');
	header.version = 2;

	header.format = 0;
	strcpy((char *) &header.format, getFormatString(format));

	header.width  = width;
	header.height = height;
	header.depth  = depth;
	header.nMipMaps = nMipMaps;

	HTexHeader2 header2;
	header2.flags = 0;
	header2.nExtraData = nExtraData;

	fwrite(&header,  sizeof(header),  1, file);
	fwrite(&header2, sizeof(header2), 1, file);


	if (isPlainFormat(format)){
		/*int nChannels = getChannelCount(format);
		int bpc = getBytesPerChannel(format);

		for (int ch = 0; ch < nChannels; ch++){
			for (int level = 0; level < nMipMaps; level++){
				int nPixels = getPixelCount(level, 1);

				ubyte *src = getPixels(level) + ch * bpc;
				for (int i = 0; i < nPixels; i++){
					fwrite(src + i * nChannels * bpc, bpc, 1, file);
				}
			}
		}*/
		fwrite(&pixels[0], getMipMappedSize(0, nMipMaps), 1, file);

	} else if (isCompressedFormat(format)){
		if (format == FORMAT_DXT1 || format == FORMAT_DXT5){
			char shift = (format == FORMAT_DXT1)? 3 : 4;

			for (int level = 0; level < nMipMaps; level++){
				ubyte *src = getPixels(level);
				if (format != FORMAT_DXT1) src += 8;
				int w = getWidth(level);
				int h = getHeight(level);
				int d = getDepth(level);

				int nBlocks = d * ((w + 3) >> 2) * ((h + 3) >> 2);

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift), 1, 2, file);
				}

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift) + 2, 1, 2, file);
				}

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift) + 4, 1, 4, file);
				}
			}
		}

		if (format == FORMAT_DXT5 || format == FORMAT_ATI1N || format == FORMAT_ATI2N){
			char shift = (format != FORMAT_DXT5)? 3 : 4;

			for (int level = 0; level < nMipMaps; level++){
				ubyte *src = getPixels(level);
				int w = getWidth(level);
				int h = getHeight(level);
				int d = getDepth(level);

				int nBlocks = d * ((w + 3) >> 2) * ((h + 3) >> 2);

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift), 1, 1, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fwrite(src + 8 * (nBlocks + n), 1, 1, file);
					}
				}

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift) + 1, 1, 1, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fwrite(src + 8 * (nBlocks + n) + 1, 1, 1, file);
					}
				}

				for (int n = 0; n < nBlocks; n++){
					fwrite(src + (n << shift) + 2, 1, 6, file);
				}
				if (format == FORMAT_ATI2N){
					for (int n = 0; n < nBlocks; n++){
						fwrite(src + 8 * (nBlocks + n) + 2, 1, 6, file);
					}
				}
			}
		}
	}

	if (nExtraData > 0){
		fwrite(&extraData[0], 1, nExtraData, file);
	}

	fclose(file);

	return true;
}

bool Image3::saveDDS(const char *fileName){
	// Set up the header
	DDSHeader header;
	stx_memset(&header, 0, sizeof(header));
	DDSHeaderDX10 headerDX10;
	stx_memset(&headerDX10, 0, sizeof(headerDX10));

	header.dwMagic = MCHAR4('D', 'D', 'S', ' ');
	header.dwSize = 124;
	header.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | (nMipMaps > 1? DDSD_MIPMAPCOUNT : 0) | (depth > 1? DDSD_DEPTH : 0);
	header.dwHeight = height;
	header.dwWidth  = width;
	header.dwPitchOrLinearSize = 0;
	header.dwDepth = (depth > 1)? depth : 0;
	header.dwMipMapCount = (nMipMaps > 1)? nMipMaps : 0;

	int nChannels = getChannelCount(format);

	header.ddpfPixelFormat.dwSize = 32;
	if (format <= FORMAT_I16 || format == FORMAT_RGB10A2){
		header.ddpfPixelFormat.dwFlags = ((nChannels < 3)? 0x00020000 : DDPF_RGB) | ((nChannels & 1)? 0 : DDPF_ALPHAPIXELS);
		if (format <= FORMAT_RGBA8){
			header.ddpfPixelFormat.dwRGBBitCount = 8 * nChannels;
			header.ddpfPixelFormat.dwRBitMask = (nChannels > 2)? 0x00FF0000 : 0xFF;
			header.ddpfPixelFormat.dwGBitMask = (nChannels > 1)? 0x0000FF00 : 0;
			header.ddpfPixelFormat.dwBBitMask = (nChannels > 1)? 0x000000FF : 0;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = (nChannels == 4)? 0xFF000000 : (nChannels == 2)? 0xFF00 : 0;
		} else if (format == FORMAT_I16){
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0xFFFF;
		} else {
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x3FF00000;
			header.ddpfPixelFormat.dwGBitMask = 0x000FFC00;
			header.ddpfPixelFormat.dwBBitMask = 0x000003FF;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xC0000000;
		}
	} else {
		header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;

		switch (format){
			case FORMAT_RG16:    header.ddpfPixelFormat.dwFourCC = 34; break;
			case FORMAT_RGBA16:  header.ddpfPixelFormat.dwFourCC = 36; break;
			case FORMAT_R16F:    header.ddpfPixelFormat.dwFourCC = 111; break;
			case FORMAT_RG16F:   header.ddpfPixelFormat.dwFourCC = 112; break;
			case FORMAT_RGBA16F: header.ddpfPixelFormat.dwFourCC = 113; break;
			case FORMAT_R32F:    header.ddpfPixelFormat.dwFourCC = 114; break;
			case FORMAT_RG32F:   header.ddpfPixelFormat.dwFourCC = 115; break;
			case FORMAT_RGBA32F: header.ddpfPixelFormat.dwFourCC = 116; break;
			case FORMAT_DXT1:    header.ddpfPixelFormat.dwFourCC = MCHAR4('D','X','T','1'); break;
			case FORMAT_DXT3:    header.ddpfPixelFormat.dwFourCC = MCHAR4('D','X','T','3'); break;
			case FORMAT_DXT5:    header.ddpfPixelFormat.dwFourCC = MCHAR4('D','X','T','5'); break;
			case FORMAT_ATI1N:   header.ddpfPixelFormat.dwFourCC = MCHAR4('A','T','I','1'); break;
			case FORMAT_ATI2N:   header.ddpfPixelFormat.dwFourCC = MCHAR4('A','T','I','2'); break;
			default:
				header.ddpfPixelFormat.dwFourCC = MCHAR4('D','X','1','0');
				headerDX10.arraySize = 1;
				headerDX10.miscFlag = (depth == 0)? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
				headerDX10.resourceDimension = is1D()? D3D10_RESOURCE_DIMENSION_TEXTURE1D : is3D()? D3D10_RESOURCE_DIMENSION_TEXTURE3D : D3D10_RESOURCE_DIMENSION_TEXTURE2D;
				switch (format){
					//case FORMAT_RGBA8:    headerDX10.dxgiFormat = 28; break;
					case FORMAT_RGB32F:   headerDX10.dxgiFormat = 6; break;
					case FORMAT_RGB9E5:   headerDX10.dxgiFormat = 67; break;
					case FORMAT_RG11B10F: headerDX10.dxgiFormat = 26; break;
					default:
						return false;
				}
		}
	}

	header.ddsCaps.dwCaps1 = DDSCAPS_TEXTURE | (nMipMaps > 1? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) | (depth != 1? DDSCAPS_COMPLEX : 0);
	header.ddsCaps.dwCaps2 = (depth > 1)? DDSCAPS2_VOLUME : (depth == 0)? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
	header.ddsCaps.Reserved[0] = 0;
	header.ddsCaps.Reserved[1] = 0;
	header.dwReserved2 = 0;

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	fwrite(&header, sizeof(header), 1, file);
	if (headerDX10.dxgiFormat) fwrite(&headerDX10, sizeof(headerDX10), 1, file);


	int size = getMipMappedSize(0, nMipMaps);

	// RGB to BGR
	if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swapChannels(&pixels[0], size / nChannels, nChannels, 0, 2);

	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
                ubyte *src = getPixels(mipMapLevel) + face * faceSize;
				fwrite(src, 1, faceSize, file);
			}
		}
	} else {
		fwrite(&pixels[0], size, 1, file);
	}
	fclose(file);

	// Restore to RGB
	if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swapChannels(&pixels[0], size / nChannels, nChannels, 0, 2);

	return true;
}

#ifndef NO_HDR

bool Image3::saveHDR(const char *fileName){
	if (format != FORMAT_RGBE8 && format != FORMAT_RGB32F) return false;

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	fLOG_PRINT(file, "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y %d +X %d\n", height, width);

	int nPixels = width * height;

	if (format == FORMAT_RGBE8){
		fwrite(&pixels[0], sizeof(uint32), nPixels, file);
	} else {

		vec3 *src = (vec3 *) &pixels[0];
		uint32 *dst = new uint32[nPixels];
		for (int i = 0; i < nPixels; i++){
			dst[i] = rgbToRGBE8(src[i]);
		}

		fwrite(dst, sizeof(uint32), nPixels, file);

		delete [] dst;
	}

	fclose(file);
	return true;
}

#endif

#ifndef NO_JPEG
bool Image3::saveJPEG(const char *fileName, const int quality){
	if (format != FORMAT_I8 && format != FORMAT_RGB8) return false;

	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	int nChannels = getChannelCount(format);

	cinfo.in_color_space = (nChannels == 1)? JCS_GRAYSCALE : JCS_RGB;
	jpeg_set_defaults(&cinfo);

	cinfo.input_components = nChannels;
	cinfo.num_components   = nChannels;
	cinfo.image_width  = width;
	cinfo.image_height = height;
	cinfo.data_precision = 8;
	cinfo.input_gamma = 1.0;

	jpeg_set_quality(&cinfo, quality, FALSE);

	jpeg_stdio_dest(&cinfo, file);
	jpeg_start_compress(&cinfo, TRUE);

	ubyte *curr_scanline = &pixels[0];

	for (int y = 0; y < height; y++){
		jpeg_write_scanlines(&cinfo, &curr_scanline, 1);
		curr_scanline += nChannels * width;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(file);

	return true;
}
#endif // NO_JPEG


#ifndef NO_PNG
bool Image3::savePNG(const char *fileName){
	int type;

	switch (format){
		case FORMAT_I8:
		case FORMAT_I16:
			type = PNG_COLOR_TYPE_GRAY;
			break;
		case FORMAT_IA8:
		case FORMAT_IA16:
			type = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		case FORMAT_RGB8:
		case FORMAT_RGB16:
			type = PNG_COLOR_TYPE_RGB;
			break;
		case FORMAT_RGBA8:
		case FORMAT_RGBA16:
			type = PNG_COLOR_TYPE_RGBA;
			break;
		default:
			return false;
	}

	png_structp png_ptr;
	png_infop info_ptr;
	FILE *file;

    std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

    if ((png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL, NULL, malloc_fn, free_fn)) == NULL){
		fclose(file);
        return false;
    }

    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL){
        png_destroy_write_struct(&png_ptr, NULL);
		fclose(file);
        return false;
    }

	png_set_write_fn(png_ptr, file, user_write_data, user_flush_data);

	int bpp = (format >= FORMAT_I16)? 16 : 8;

	png_set_IHDR(png_ptr, info_ptr, width, height, bpp, type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	//png_set_compression_level(png_ptr, 9);
	png_write_info(png_ptr, info_ptr);

	int nElements = width * getChannelCount(format);
	if (format >= FORMAT_I16){
		ushort *line = new ushort[nElements];
		for (int y = 0; y < height; y++){
			ushort *src = ((ushort *) &pixels[0]) + y * nElements;
			// Swap endian
			for (int x = 0; x < nElements; x++){
				line[x] = (src[x] >> 8) | (src[x] << 8);
			}
			png_write_row(png_ptr, (ubyte *) line);
		}
		delete [] line;
	} else {
		for (int y = 0; y < height; y++){
			png_write_row(png_ptr, &pixels[0] + y * nElements);
		}
	}

	png_write_end(png_ptr, info_ptr);
	
    png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(file);

	return true;
}
#endif // NO_PNG


#ifndef NO_TGA
bool Image3::saveTGA(const char *fileName){
	if (format != FORMAT_I8 && format != FORMAT_RGB8 && format != FORMAT_RGBA8) return false;

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	int nChannels = getChannelCount(format);

	TGAHeader header = {
		0x00,
		static_cast<unsigned char>((format == FORMAT_I8)? 1 : 0),
		static_cast<unsigned char>((format == FORMAT_I8)? 1 : 2),
		0x0000,
		static_cast<unsigned short>((format == FORMAT_I8)? 256 : 0),
		static_cast<unsigned char>((format == FORMAT_I8)? 24  : 0),
		0x0000,
		0x0000,
		static_cast<unsigned short>(width),
		static_cast<unsigned short>(height),
		static_cast<unsigned char>(nChannels * 8),
		0x00
	};

	fwrite(&header, sizeof(header), 1, file);

	ubyte *dest, *src, *buffer;

	if (format == FORMAT_I8){
		ubyte pal[768];
		int p = 0;
		for (int i = 0; i < 256; i++){
			pal[p++] = i;
			pal[p++] = i;
			pal[p++] = i;
		}
		fwrite(pal, sizeof(pal), 1, file);

		src = &pixels[0] + width * height;
		for (int y = 0; y < height; y++){
			src -= width;
			fwrite(src, width, 1, file);
		}

	} else {
		bool useAlpha = (nChannels == 4);
		int lineLength = width * (useAlpha? 4 : 3);

		buffer = new ubyte[height * lineLength];
		int len;

		for (int y = 0; y < height; y++){
			dest = buffer + (height - y - 1) * lineLength;
			src  = &pixels[0] + y * width * nChannels;
			len = width;
			do {
				*dest++ = src[2];
				*dest++ = src[1];
				*dest++ = src[0];
				if (useAlpha) *dest++ = src[3];
				src += nChannels;
			} while (--len);
		}

		fwrite(buffer, height * lineLength, 1, file);
		delete [] buffer;
	}

	fclose(file);

	return true;
}
#endif // NO_TGA

#ifndef NO_BMP
bool Image3::saveBMP(const char *fileName){
	if (format != FORMAT_I8 && format != FORMAT_RGB8 && format != FORMAT_RGBA8) return false;

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	int nChannels = getChannelCount(format);
	BMPHeader header = {
		MCHAR2('B','M'), 
		{0x36, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00},
		static_cast<unsigned int>(width),
		static_cast<unsigned int>(height),
		0x0001,
		static_cast<unsigned short>(nChannels * 8),
		0,
		{0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	fwrite(&header, sizeof(header), 1, file);

	if (format == FORMAT_I8){
		ubyte pal[1024];
		for (int i = 0; i < 1024; i++){
			pal[i] = i >> 2;
		}
		fwrite(pal, sizeof(pal), 1, file);

		ubyte *src = &pixels[0] + width * height;
		for (int y = 0; y < height; y++){
			src -= width;
			fwrite(src, width, 1, file);
		}
	} else {
		bool useAlpha = (nChannels == 4);
		int paddBytes = (width * nChannels) & 3;
		if (paddBytes) paddBytes = 4 - paddBytes;

		int len, size = (width * nChannels + paddBytes) * height;
		ubyte *dest, *src, *buffer = new ubyte[size];

		for (int y = 0; y < height; y++){
			dest = buffer + (height - y - 1) * (width * nChannels + paddBytes);
			src  = &pixels[0] + y * width * nChannels;
			len = width;
			do {
				*dest++ = src[2];
				*dest++ = src[1];
				*dest++ = src[0];
				if (useAlpha) *dest++ = src[3];
				src += nChannels;
			} while (--len);
		}
		fwrite(buffer, size, 1, file);
		delete [] buffer;
	}

	fclose(file);

	return true;
}
#endif // NO_BMP

#ifndef NO_PCX
bool Image3::savePCX(const char *fileName){
	if (format != FORMAT_I8 && format != FORMAT_RGB8 && format != FORMAT_RGBA8) return false;

	int nChannels = getChannelCount(format);
	int destChannels = (format == FORMAT_RGBA8)? 3 : nChannels;


	PCXHeader header = {
		{0x0a, 0x05, 0x01},
		8,
		{0x00, 0x00, 0x00, 0x00}, 
		width  - 1,
		height - 1,
		{0x48, 0x00, 0x48, 0x00, 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
		 0x0d, 0x0d, 0x0d, 0x0c, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x0a,
		 0x0a, 0x0a, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x07, 0x07,
		 0x07, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04,
		 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00,
		 0x00, 0x00, 0x00},
		destChannels, // 3 channels
		width,
		{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	FILE *file;
	std::string fn=std::string(fileName);if ((file = fopen(fn.c_str(), "wb")) == NULL) return false;

	fwrite(&header, sizeof(header), 1, file);


	ubyte *src, *buffer = new ubyte[width * 2];
	int pos;

	for (int y = 0; y < height; y++){
		for (int i = 0; i < destChannels; i++){
			src = &pixels[0] + y * width * nChannels + i;
			pos = 0;

			unsigned int count;
			int prevx, x = 0;
			ubyte curr = *src;

			do {
				prevx = x;
				curr = src[x * nChannels];
				do {
					x++;
				} while (x < width && src[x * nChannels] == curr);

				count = x - prevx;
				if (count == 1){
					if (curr < 192){
						buffer[pos++] = curr;
					} else {
						buffer[pos++] = 193;
						buffer[pos++] = curr;
					}
				} else {
					do {
						ubyte len = (count > 63)? 63 : count;
						
						buffer[pos++] = 192 + len;
						buffer[pos++] = curr;
						
						count -= len;
					} while (count);
				}

			} while (x < width);

			fwrite(buffer, pos, 1, file);			
		}
	}

	delete [] buffer;

	if (format == FORMAT_I8){
		ubyte pal[768];
		int p = 0;
		for (int i = 0; i < 256; i++){
			pal[p++] = i;
			pal[p++] = i;
			pal[p++] = i;
		}
		fwrite(pal, sizeof(pal), 1, file);
	}

	fclose(file);

	return true;
}
#endif // NO_PCX

bool Image3::saveImage(const char *aFileName){
		STX_PRINT("Image3::saveImage:\n");
		STX_PRINT("fileName=%s\n", aFileName);
		std::string fn=aFileName;
		if(aFileName&&aFileName[0]=='/')
			fn=stx_convertpath(aFileName);
	const char *fileName=fn.c_str();


	const char *extension = strrchr(fileName, '.');

	if (extension != NULL){
		if (stricmp(extension, ".htex") == 0){
			return saveHTEX(fileName);
		} else if (stricmp(extension, ".dds") == 0){
			return saveDDS(fileName);
		}
#ifndef NO_JPEG
		else if (stricmp(extension, ".jpg") == 0 || 
                   stricmp(extension, ".jpeg") == 0){
			return saveJPEG(fileName, 75);
		}
#endif // NO_JPEG
#ifndef NO_PNG
		else if (stricmp(extension, ".png") == 0){
			return savePNG(fileName);
		}
#endif // NO_PNG
#ifndef NO_TGA
		else if (stricmp(extension, ".tga") == 0){
			return saveTGA(fileName);
		}
#endif // NO_TGA
#ifndef NO_BMP
		else if (stricmp(extension, ".bmp") == 0){
			return saveBMP(fileName);
		}
#endif // NO_BMP
#ifndef NO_PCX
		else if (stricmp(extension, ".pcx") == 0){
			return savePCX(fileName);
		}
#endif // NO_PCX
	}
	return false;
}

void Image3::loadFromMemory(void *mem, const FORMAT frmt, const int w, const int h, const int d, const int mipMapCount, bool ownsMemory){
	free();

	width  = w;
	height = h;
	depth  = d;
    format = frmt;
	nMipMaps = mipMapCount;
	arraySize = 1;

	if (0)//ownsMemory)
	{
	//	pixels = (ubyte *) mem;
	} else {
		int size = getMipMappedSize(0, nMipMaps);
		pixels.resize(size);
		stx_memcpy(&pixels[0], mem, size);
	}
}


template <typename DATA_TYPE>
void buildMipMap(DATA_TYPE *dst, const DATA_TYPE *src, const uint w, const uint h, const uint d, const uint c){
	uint xOff = (w < 2)? 0 : c;
	uint yOff = (h < 2)? 0 : c * w;
	uint zOff = (d < 2)? 0 : c * w * h;

	for (uint z = 0; z < d; z += 2){
		for (uint y = 0; y < h; y += 2){
			for (uint x = 0; x < w; x += 2){
				for (uint i = 0; i < c; i++){
					*dst++ = (src[0] + src[xOff] + src[yOff] + src[yOff + xOff] + src[zOff] + src[zOff + xOff] + src[zOff + yOff] + src[zOff + yOff + xOff]) / 8;
					src++;
				}
				src += xOff;
			}
			src += yOff;
		}
		src += zOff;
	}
}

bool Image3::createMipMaps(const int mipMaps){
	if (isCompressedFormat(format)) return false;
	if (!isPowerOf2(width) || !isPowerOf2(height) || !isPowerOf2(depth)) return false;

	int actualMipMaps = min(mipMaps, getMipMapCountFromDimesions());

	if (nMipMaps != actualMipMaps){
		int size = getMipMappedSize(0, actualMipMaps);
		if (arraySize > 1){
			std::vector<ubyte> newPixels(size * arraySize);

			// Copy top mipmap of all array slices to new location
			int firstMipSize = getMipMappedSize(0, 1);
			int oldSize = getMipMappedSize(0, nMipMaps);

			for (int i = 0; i < arraySize; i++){
				stx_memcpy(&newPixels[0] + i * size, &pixels[0] + i * oldSize, firstMipSize);
			}

			pixels.resize(0);
			pixels = newPixels;
		} else {
			pixels.resize(size);
		}
		nMipMaps = actualMipMaps;
	}

	int nChannels = getChannelCount(format);


	int n = isCube()? 6 : 1;

	for (int arraySlice = 0; arraySlice < arraySize; arraySlice++){
		ubyte *src = getPixels(0, arraySlice);
		ubyte *dst = getPixels(1, arraySlice);

		for (int level = 1; level < nMipMaps; level++){
			int w = getWidth (level - 1);
			int h = getHeight(level - 1);
			int d = getDepth (level - 1);

			int srcSize = getMipMappedSize(level - 1, 1) / n;
			int dstSize = getMipMappedSize(level,     1) / n;

			for (int i = 0; i < n; i++){
				if (isPlainFormat(format)){
					if (isFloatFormat(format)){
						buildMipMap((float *) dst, (float *) src, w, h, d, nChannels);
					} else if (format >= FORMAT_I16){
						buildMipMap((ushort *) dst, (ushort *) src, w, h, d, nChannels);
					} else {
						buildMipMap(dst, src, w, h, d, nChannels);
					}
				}
				src += srcSize;
				dst += dstSize;
			}
		}
	}

	return true;
}

void decodeColorBlock(ubyte *dest, int w, int h, int xOff, int yOff, FORMAT format, int red, int blue, ubyte *src){
	ubyte colors[4][3];

	uint16 c0 = *(uint16 *) src;
	uint16 c1 = *(uint16 *) (src + 2);
	
	colors[0][0] = ((c0 >> 11) & 0x1F) << 3;
	colors[0][1] = ((c0 >>  5) & 0x3F) << 2;
	colors[0][2] =  (c0        & 0x1F) << 3;
	
	colors[1][0] = ((c1 >> 11) & 0x1F) << 3;
	colors[1][1] = ((c1 >>  5) & 0x3F) << 2;
	colors[1][2] =  (c1        & 0x1F) << 3;

	if (c0 > c1 || format == FORMAT_DXT5){
		for (int i = 0; i < 3; i++){
			colors[2][i] = (2 * colors[0][i] +     colors[1][i] + 1) / 3;
			colors[3][i] = (    colors[0][i] + 2 * colors[1][i] + 1) / 3;
		}
	} else {
		for (int i = 0; i < 3; i++){
			colors[2][i] = (colors[0][i] + colors[1][i] + 1) >> 1;
			colors[3][i] = 0;
		}
	}

	src += 4;
	for (int y = 0; y < h; y++){
		ubyte *dst = dest + yOff * y;
		unsigned int indexes = src[y];
		for (int x = 0; x < w; x++){
			unsigned int index = indexes & 0x3;
			dst[red]  = colors[index][0];
			dst[1]    = colors[index][1];
			dst[blue] = colors[index][2];
			indexes >>= 2;

			dst += xOff;
		}
	}
}

void decodeDXT3AlphaBlock(ubyte *dest, int w, int h, int xOff, int yOff, ubyte *src){
	for (int y = 0; y < h; y++){
		ubyte *dst = dest + yOff * y;
		unsigned int alpha = ((unsigned short *) src)[y];
		for (int x = 0; x < w; x++){
			*dst = (alpha & 0xF) * 17;
			alpha >>= 4;
			dst += xOff;
		}
	}
}

void decodeDXT5AlphaBlock(ubyte *dest, int w, int h, int xOff, int yOff, ubyte *src){
	ubyte a0 = src[0];
	ubyte a1 = src[1];
	uint64 alpha = (*(uint64 *) src) >> 16;

	for (int y = 0; y < h; y++){
		ubyte *dst = dest + yOff * y;
		for (int x = 0; x < w; x++){
			int k = ((unsigned int) alpha) & 0x7;
			if (k == 0){
				*dst = a0;
			} else if (k == 1){
				*dst = a1;
			} else if (a0 > a1){
				*dst = ((8 - k) * a0 + (k - 1) * a1) / 7;
			} else if (k >= 6){
				*dst = (k == 6)? 0 : 255;
			} else {
				*dst = ((6 - k) * a0 + (k - 1) * a1) / 5;
			}
			alpha >>= 3;

			dst += xOff;
		}
		if (w < 4) alpha >>= (3 * (4 - w));
	}
}

void decodeCompressedImage(ubyte *dest, ubyte *src, const int width, const int height, const FORMAT format){
	int sx = (width  < 4)? width  : 4;
	int sy = (height < 4)? height : 4;

	int nChannels = getChannelCount(format);

	for (int y = 0; y < height; y += 4){
		for (int x = 0; x < width; x += 4){
			ubyte *dst = dest + (y * width + x) * nChannels;
			if (format == FORMAT_DXT3){
				decodeDXT3AlphaBlock(dst + 3, sx, sy, nChannels, width * nChannels, src);
				src += 8;
			} else if (format == FORMAT_DXT5){
				decodeDXT5AlphaBlock(dst + 3, sx, sy, nChannels, width * nChannels, src);
				src += 8;
			}
			if (format <= FORMAT_DXT5){
				decodeColorBlock(dst, sx, sy, nChannels, width * nChannels, format, 0, 2, src);
				src += 8;
			} else {
				if (format == FORMAT_ATI1N){
					decodeDXT5AlphaBlock(dst, sx, sy, 1, width, src);
					src += 8;
				} else {
					decodeDXT5AlphaBlock(dst,     sx, sy, 2, width * 2, src + 8);
					decodeDXT5AlphaBlock(dst + 1, sx, sy, 2, width * 2, src);
					src += 16;
				}
			}
		}
	}
}

bool Image3::uncompressImage(){
	if (isCompressedFormat(format)){
		FORMAT destFormat;
		if (format >= FORMAT_ATI1N){
			destFormat = (format == FORMAT_ATI1N)? FORMAT_I8 : FORMAT_IA8;
		} else {
			destFormat = (format == FORMAT_DXT1)? FORMAT_RGB8 : FORMAT_RGBA8;
		}

		std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps, destFormat));

		int level = 0;
		ubyte* src=&newPixels[0];
		ubyte* dst=&newPixels[0];
		while ((src = getPixels(level)) != NULL){
			int w = getWidth(level);
			int h = getHeight(level);
			int d = (depth == 0)? 6 : getDepth(level);

			int dstSliceSize = getSliceSize(level, destFormat);
			int srcSliceSize = getSliceSize(level, format);

			for (int slice = 0; slice < d; slice++){
				decodeCompressedImage(dst, src, w, h, format);

				dst += dstSliceSize;
				src += srcSliceSize;
			}
			level++;
		}

		format = destFormat;
		
		free();
		pixels = newPixels;
	}

	return true;
}

bool Image3::unpackImage(){
	int pixelCount = getPixelCount(0, nMipMaps);

	std::vector<ubyte> newPixels;
	if (format == FORMAT_RGBE8){
		format = FORMAT_RGB32F;
		newPixels.resize(getMipMappedSize(0, nMipMaps));

		for (int i = 0; i < pixelCount; i++){
			((vec3 *) &newPixels[0])[i] = rgbeToRGB(&pixels[0] + 4 * i);
		}

	} else if (format == FORMAT_RGB565){
		format = FORMAT_RGB8;
		newPixels.resize(getMipMappedSize(0, nMipMaps));

		for (int i = 0; i < pixelCount; i++){
			unsigned int rgb565 = ((unsigned short *) &pixels[0])[i];
			newPixels[3 * i    ] = ((rgb565 >> 11) * 2106) >> 8;
			newPixels[3 * i + 1] = ((rgb565 >> 5) & 0x3F) * 1037 >> 8;
			newPixels[3 * i + 2] = ((rgb565 & 0x1F) * 2106) >> 8;
		}
	} else if (format == FORMAT_RGBA4){
		format = FORMAT_RGBA8;
		newPixels.resize(getMipMappedSize(0, nMipMaps));

		for (int i = 0; i < pixelCount; i++){
			newPixels[4 * i    ] = (pixels[2 * i + 1] & 0xF) * 17;
			newPixels[4 * i + 1] = (pixels[2 * i]     >>  4) * 17;
			newPixels[4 * i + 2] = (pixels[2 * i]     & 0xF) * 17;
			newPixels[4 * i + 3] = (pixels[2 * i + 1] >>  4) * 17;
		}
	} else if (format == FORMAT_RGB10A2){
		format = FORMAT_RGBA16;
		newPixels.resize(getMipMappedSize(0, nMipMaps));

		for (int i = 0; i < pixelCount; i++){
			uint32 src = ((uint32 *) &pixels[0])[i];
			((ushort *) &newPixels[0])[4 * i    ] = (((src      ) & 0x3FF) * 4198340) >> 16;
			((ushort *) &newPixels[0])[4 * i + 1] = (((src >> 10) & 0x3FF) * 4198340) >> 16;
			((ushort *) &newPixels[0])[4 * i + 2] = (((src >> 20) & 0x3FF) * 4198340) >> 16;
			((ushort *) &newPixels[0])[4 * i + 3] = (((src >> 30) & 0x003) * 21845);
		}
	} else {
		return false;
	}

	pixels.clear();
	pixels = newPixels;

	return true;
}

bool Image3::convert(const FORMAT newFormat)
{
				#if 0
		LOG_PRINT("Image3::convert:\n");
	if(format==FORMAT_RGB8)
		LOG_PRINT("format==FORMAT_RGB8\n");
	else if(format==FORMAT_RGBA8)
		LOG_PRINT("format==FORMAT_RGBA8\n");
	if(newFormat==FORMAT_RGB8)
		LOG_PRINT("newFormat==FORMAT_RGB8\n");
	else if(newFormat==FORMAT_RGBA8)
		LOG_PRINT("newFormat==FORMAT_RGBA8\n");
				#endif

	if(format == newFormat)
		return true;

	std::vector<ubyte> newPixels;
	uint nPixels = getPixelCount(0, nMipMaps) * arraySize;

	if (format == FORMAT_RGBE8 && (newFormat == FORMAT_RGB32F || newFormat == FORMAT_RGBA32F)){
		newPixels.resize(getMipMappedSize(0, nMipMaps, newFormat) * arraySize);
		float *dest = (float *) &newPixels[0];

		bool writeAlpha = (newFormat == FORMAT_RGBA32F);
		ubyte *src = &pixels[0];
		do {
			*((vec3 *) dest) = rgbeToRGB(src);
			if (writeAlpha){
				dest[3] = 1.0f;
				dest += 4;
			} else {
				dest += 3;
			}
			src += 4;
		} while (--nPixels);

	} else {
		if (!isPlainFormat(format) || !(isPlainFormat(newFormat) || newFormat == FORMAT_RGB10A2 || newFormat == FORMAT_RGBE8 || newFormat == FORMAT_RGB9E5)) return false;
		if (format == newFormat) return true;

		ubyte *src = &pixels[0];
		newPixels.resize(getMipMappedSize(0, nMipMaps, newFormat) * arraySize);
		ubyte *dest = &newPixels[0];

		if (format == FORMAT_RGB8 && newFormat == FORMAT_RGBA8){
			// Fast path for RGB->RGBA8
			do {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				dest[3] = 255;
				dest += 4;
				src += 3;
			} while (--nPixels);

		} else {
			int srcSize = getBytesPerPixel(format);
			int nSrcChannels = getChannelCount(format);

			int destSize = getBytesPerPixel(newFormat);
			int nDestChannels = getChannelCount(newFormat);

			do {
				float rgba[4];

				if (isFloatFormat(format)){
					if (format <= FORMAT_RGBA16F){
						for (int i = 0; i < nSrcChannels; i++) rgba[i] = ((half *) src)[i];
					} else {
						for (int i = 0; i < nSrcChannels; i++) rgba[i] = ((float *) src)[i];
					}
				} else if (format >= FORMAT_I16 && format <= FORMAT_RGBA16){
					for (int i = 0; i < nSrcChannels; i++) rgba[i] = ((ushort *) src)[i] * (1.0f / 65535.0f);
				} else {
					for (int i = 0; i < nSrcChannels; i++) rgba[i] = src[i] * (1.0f / 255.0f);
				}
				if (nSrcChannels  < 4) rgba[3] = 1.0f;
				if (nSrcChannels == 1) rgba[2] = rgba[1] = rgba[0];
				
				if (nDestChannels == 1)	rgba[0] = 0.30f * rgba[0] + 0.59f * rgba[1] + 0.11f * rgba[2];

				if (isFloatFormat(newFormat)){
					if (newFormat <= FORMAT_RGBA32F){
						if (newFormat <= FORMAT_RGBA16F){
							for (int i = 0; i < nDestChannels; i++)	((half *) dest)[i] = rgba[i];
						} else {
							for (int i = 0; i < nDestChannels; i++)	((float *) dest)[i] = rgba[i];
						}
					} else {
						if (newFormat == FORMAT_RGBE8){
							*(uint32 *) dest = rgbToRGBE8(vec3(rgba[0], rgba[1], rgba[2]));
						} else {
							*(uint32 *) dest = rgbToRGB9E5(vec3(rgba[0], rgba[1], rgba[2]));
						}
					}
				} else if (newFormat >= FORMAT_I16 && newFormat <= FORMAT_RGBA16){
					for (int i = 0; i < nDestChannels; i++)	((ushort *) dest)[i] = (ushort) (65535 * saturate(rgba[i]) + 0.5f);
				} else if (/*isPackedFormat(newFormat)*/newFormat == FORMAT_RGB10A2){
					*(uint *) dest =
						(uint(1023.0f * saturate(rgba[0]) + 0.5f) << 22) |
						(uint(1023.0f * saturate(rgba[1]) + 0.5f) << 12) |
						(uint(1023.0f * saturate(rgba[2]) + 0.5f) <<  2) |
						(uint(   3.0f * saturate(rgba[3]) + 0.5f));
				}
				#ifndef __APPLE__ // ???
				else {
					for (int i = 0; i < nDestChannels; i++)	dest[i] = (ubyte) (255 * saturate(rgba[i]) + 0.5f);
				}
				#else
				else {
					for (int i = 0; i < nDestChannels; i++)
					{
						float f=(255 * saturate(rgba[i]) + 0.5f);
						dest[i] = ubyte(f);
					}
				}
				#endif

				src  += srcSize;
				dest += destSize;
			} while (--nPixels);
		}
	}
	pixels.resize(0);
	pixels = newPixels;
	format = newFormat;

	return true;
}

bool Image3::swap(const int ch0, const int ch1){
	if (!isPlainFormat(format)) return false;

	unsigned int nPixels = getPixelCount(0, nMipMaps) * arraySize;
	unsigned int nChannels = getChannelCount(format);

	if (format <= FORMAT_RGBA8){
		swapChannels((ubyte *) &pixels[0], nPixels, nChannels, ch0, ch1);
	} else if (format <= FORMAT_RGBA16F){
		swapChannels((unsigned short *) &pixels[0], nPixels, nChannels, ch0, ch1);
	} else {
		swapChannels((float *) &pixels[0], nPixels, nChannels, ch0, ch1);
	}

	return true;
}

bool Image3::flipX(){
	// TODO: Implement ...
	return false;
}

bool Image3::flipY(){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (isCompressedFormat(format)) return false;

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps));
	ubyte *dst = &newPixels[0];

	int bytes = getBytesPerPixel(format);

	for (int i = 0; i < nMipMaps; i++){
		int w = getWidth(i);
		int h = getHeight(i);

		int lineWidth = w * bytes;

		ubyte *src = getPixels(i) + (h - 1) * lineWidth;

		for (int y = 0; y < h; y++){
			stx_memcpy(dst, src, lineWidth);

			dst += lineWidth;
			src -= lineWidth;
		}
	}


	pixels.clear();
	pixels = newPixels;

	return true;
}

bool Image3::dilate(){
	if (format != FORMAT_R8)
		return false;

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps));
	ubyte *dst = &newPixels[0];

	//int ch = getChannelCount(format);

	for (int i = 0; i < nMipMaps; i++){
		int w = getWidth(i);
		int h = getHeight(i);

		ubyte *src = getPixels(i);

		for (int y = 0; y < h; y++){
			for (int x = 0; x < w; x++){
				int startX = max(x - 1, 0);
				int endX = min(x + 2, w);
				int startY = max(y - 1, 0);
				int endY = min(y + 2, h);

				ubyte max = 0;
				for (int iy = startY; iy < endY; iy++)
				{
					for (int ix = startX; ix < endX; ix++)
					{
						ubyte c = src[iy * w + ix];
						if (c > max)
							max = c;
					}
				}

				*dst++ = max;
			}
		}
	}


	pixels.clear();
	pixels = newPixels;

	return true;
}

bool Image3::erode(){
	if (format != FORMAT_R8)
		return false;

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps));
	ubyte *dst = &newPixels[0];

	//int ch = getChannelCount(format);

	for (int i = 0; i < nMipMaps; i++){
		int w = getWidth(i);
		int h = getHeight(i);

		ubyte *src = getPixels(i);

		for (int y = 0; y < h; y++){
			for (int x = 0; x < w; x++){
				int startX = max(x - 1, 0);
				int endX = min(x + 2, w);
				int startY = max(y - 1, 0);
				int endY = min(y + 2, h);

				ubyte min = 0xFF;
				for (int iy = startY; iy < endY; iy++)
				{
					for (int ix = startX; ix < endX; ix++)
					{
						ubyte c = src[iy * w + ix];
						if (c < min)
							min = c;
					}
				}

				*dst++ = min;
			}
		}
	}


	pixels.clear();
	pixels = newPixels;

	return true;
}

bool Image3::toRGBD16(){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (format != FORMAT_RGB32F) return false;

	uint nPixels = getPixelCount(0, nMipMaps);
	float *src = (float *) &pixels[0];

	ushort *newPixels = new ushort[nPixels * 4];
	for (uint i = 0; i < nPixels; i++){
		float maxChannel = max(max(max(src[3 * i], src[3 * i + 1]), src[3 * i + 2]), 1.0f);

		newPixels[4 * i + 0] = (ushort) (65535 * (src[3 * i + 0] / maxChannel));
		newPixels[4 * i + 1] = (ushort) (65535 * (src[3 * i + 1] / maxChannel));
		newPixels[4 * i + 2] = (ushort) (65535 * (src[3 * i + 2] / maxChannel));
		newPixels[4 * i + 3] = (ushort) (65535 * (1.0f / maxChannel));
	}

	pixels.resize(sizeof(ushort) * nPixels * 4);
	stx_memcpy(&pixels[0] , (ubyte *) &newPixels[0], sizeof(ushort)*4 * nPixels);
	format = FORMAT_RGBA16;

	return true;
}


// TODO: Take care of black pixels ...
//       Best would be to set RGB to zero and use an exponent that's the average of the surrounding pixels
bool Image3::toRGBE16(float &scale, float &bias){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (format != FORMAT_RGB32F) return false;

	uint nPixels = getPixelCount(0, nMipMaps);

	float maxExp = -1e10f;
	float minExp =  1e10f;

	float *src = (float *) &pixels[0];
	for (uint i = 0; i < nPixels; i++){
        float maxChannel = max(max(src[3 * i], src[3 * i + 1]), src[3 * i + 2]);

		if (maxChannel > maxExp) maxExp = maxChannel;
		if (maxChannel < minExp) minExp = maxChannel;
	}

	const float invLog2 = 1.0f / logf(2.0f);

	maxExp = logf(maxExp) * invLog2;
	minExp = logf(minExp) * invLog2;

	scale = maxExp - minExp;
	bias  = minExp;

	ushort *newPixels = new ushort[nPixels * 4];
	for (uint i = 0; i < nPixels; i++){
		float maxChannel = max(max(src[3 * i], src[3 * i + 1]), src[3 * i + 2]);
		float chExp = logf(maxChannel) * invLog2;

		newPixels[4 * i + 0] = (ushort) (65535 * src[3 * i + 0] / maxChannel);
		newPixels[4 * i + 1] = (ushort) (65535 * src[3 * i + 1] / maxChannel);
		newPixels[4 * i + 2] = (ushort) (65535 * src[3 * i + 2] / maxChannel);
		newPixels[4 * i + 3] = (ushort) (65535 * saturate((chExp - bias) / scale));
	}


	pixels.resize(sizeof(ushort) * nPixels * 4);
	stx_memcpy(&pixels[0] , (ubyte *) &newPixels[0], sizeof(ushort)*4 * nPixels);
	format = FORMAT_RGBA16;

	return true;
}

bool Image3::toE16(float *scale, float *bias, const bool useAllSameRange, const float minValue, const float maxValue){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (format < FORMAT_I32F || format > FORMAT_RGBA32F) return false;

	uint nPixels = getPixelCount(0, nMipMaps);
	uint nChannels = getChannelCount(format);
	uint nElements = nPixels * nChannels;

	uint nRanges = useAllSameRange? 1 : nChannels;


	float maxExp[4] = { minValue, minValue, minValue, minValue };
	float minExp[4] = { maxValue, maxValue, maxValue, maxValue };

	float *src = (float *) &pixels[0];
	for (uint i = 0; i < nElements; i += nRanges){
		for (uint k = 0; k < nRanges; k++){
			float val = min(max(src[i + k], minValue), maxValue);
			if (val > maxExp[k]) maxExp[k] = val;
			if (val < minExp[k]) minExp[k] = val;
		}
	}

	const float invLog2 = 1.0f / logf(2.0f);

	for (uint k = 0; k < nRanges; k++){
		maxExp[k] = logf(maxExp[k]) * invLog2;
		minExp[k] = logf(minExp[k]) * invLog2;

		scale[k] = maxExp[k] - minExp[k];
		bias[k]  = minExp[k];
	}

	ushort *newPixels = new ushort[nElements];
	for (uint i = 0; i < nElements; i += nRanges){
		for (uint k = 0; k < nRanges; k++){
			if (src[i + k] > minValue){
				if (src[i + k] < maxValue){
					float chExp = logf(src[i + k]) * invLog2;
					newPixels[i + k] = (ushort) (65535 * (chExp - bias[k]) / scale[k]);
				} else {
					newPixels[i + k] = 65535;
				}
			} else {
				newPixels[i + k] = 0;
			}
		}
	}

	pixels.resize(sizeof(ushort) * nElements);
	stx_memcpy(&pixels[0] , (ubyte *) &newPixels[0], sizeof(ushort)* nElements);
	format = (FORMAT) ((FORMAT_I16 - 1) + nChannels);

	return true;
}


bool Image3::toFixedPointHDR(float *maxValue, const int finalRgbBits, const int finalRangeBits){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (format != FORMAT_RGB32F) return false;

	float rgbScale = float(1 << finalRgbBits);
	float rangeScale = float(1 << finalRangeBits);

	float clampVal = *maxValue;

	float *src = (float *) &pixels[0];
	uint nPixels = getPixelCount(0, nMipMaps);

	float maxVal = 0;
	for (uint i = 0; i < 3 * nPixels; i++){
        if (src[i] > maxVal) maxVal = src[i];
	}
	if (maxVal > clampVal) maxVal = clampVal;

	ushort *newPixels = new ushort[4 * nPixels];
	for (uint i = 0; i < nPixels; i++){
		float r = min(src[3 * i + 0], clampVal);
		float g = min(src[3 * i + 1], clampVal);
		float b = min(src[3 * i + 2], clampVal);

		float maxChannel = max(max(r, g), b);
		//float minChannel = min(min(r, g), b);

		r /= maxChannel;
		g /= maxChannel;
		b /= maxChannel;
		float range = maxChannel / maxVal;

/*
		r = sqrtf(r);
		g = sqrtf(g);
		b = sqrtf(b);
		range = sqrtf(range);
*/

		/*
			If range gets too small, scale it up and scale down rgb so that the lowest
			channel's value is equal to range. This gives us a much better distribution
			of bits for low values.
		*/
		//float rgbMin = minChannel / maxChannel;
		//if (range * rangeScale < rgbMin * rgbScale){
			//float f = sqrtf((rgbMin * rgbScale) / (range * rangeScale));
			float f = sqrtf(range * rangeScale / rgbScale);

			//float f = powf(range * rangeScale / rgbScale, 1.0f / 3.0f);

//			f = 1.0f;

			range /= f;
//			f *= f;
			r *= f;
			g *= f;
			b *= f;
		//}

		newPixels[4 * i + 0] = (ushort) (65535 * r);
		newPixels[4 * i + 1] = (ushort) (65535 * g);
		newPixels[4 * i + 2] = (ushort) (65535 * b);
		newPixels[4 * i + 3] = (ushort) (65535 * range);
	}


	format = FORMAT_RGBA16;

	pixels.resize(sizeof(ushort)*4 * nPixels);
	stx_memcpy(&pixels[0] , (ubyte *) &newPixels[0], sizeof(ushort)*4 * nPixels);

	*maxValue = maxVal;
	return true;
}

bool Image3::toNormalMap(FORMAT destFormat, float sZ, float mipMapScaleZ){
	if (arraySize > 1) return false; // TODO: Implement ...
	if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) toGrayScale();
	if (format != FORMAT_I8) return false;
	
	uint xMask = 0, yMask = 0, zMask = 0, hMask = 0;
	uint xShift = 0, yShift = 0, zShift = 0, hShift = 0, hFactor = 1;

	switch (destFormat){
		case FORMAT_RG8:
		case FORMAT_RG8S:
			xMask = yMask = 0xFF;
			xShift = 8;
			break;
		case FORMAT_RGB565:
			xMask = zMask = 0x1F;
			yMask = 0x3F;
			xShift = 11;
			yShift = 5;
			break;
		case FORMAT_RGBA4:
			xMask = yMask = zMask = hMask = 0xF;
			yShift = 4;
			zShift = 8;
			hShift = 12;
			break;
		case FORMAT_RGBA8:
		case FORMAT_RGBA8S:
			xMask = yMask = zMask = hMask = 0xFF;
			yShift = 8;
			zShift = 16;
			hShift = 24;
			break;
		case FORMAT_RGB10A2:
			xMask = yMask = zMask = 0x3FF;
			hMask = 0x03;
			yShift = 10;
			zShift = 20;
			hShift = 30;
			break;
		case FORMAT_RGBA16:
		case FORMAT_RGBA16S:
			xMask = yMask = zMask = hMask = 0xFFFF;
			yShift = 16;
			zShift = 32;
			hShift = 48;
			hFactor = 257;
			break;
		default:
			return false;
	}

	const float sobelX[5][5] = {
		{ 1,  2,  0,  -2, -1 },
		{ 4,  8,  0,  -8, -4 },
		{ 6, 12,  0, -12, -6 },
		{ 4,  8,  0,  -8, -4 },
		{ 1,  2,  0,  -2, -1 },
	};
	const float sobelY[5][5] = {
		{  1,  4,   6,  4,  1 },
		{  2,  8,  12,  8,  2 },
		{  0,  0,   0,  0,  0 },
		{ -2, -8, -12, -8, -2 },
		{ -1, -4,  -6, -4, -1 },
	};

	int bpp = getBytesPerPixel(destFormat);
	bool use16 = (bpp == 2);
	bool use32 = (bpp == 4);

	float xFactor = 0.5f * xMask;
	float yFactor = 0.5f * yMask;
	float zFactor = 0.5f * zMask;
	float bias = isSignedFormat(destFormat)? 0.0f : 1.0f;


	// Size of the z component
	sZ *= 128.0f / max(width, height);

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps, destFormat));

	union {
		ushort *dest16;
		uint32 *dest32;
		uint64 *dest64;
	};
	dest32 = (uint32 *) &newPixels[0];

	for (int mipMap = 0; mipMap < nMipMaps; mipMap++){
		ubyte *src = getPixels(mipMap);

		int w = getWidth(mipMap);
		int h = getHeight(mipMap);

		for (int y = 0; y < h; y++){
			for (int x = 0; x < w; x++){
				// Apply a 5x5 Sobel filter
				float sX = 0;
				float sY = 0;
				for (int dy = 0; dy < 5; dy++){
					int fy = (y + dy - 2 + h) % h;
					for (int dx = 0; dx < 5; dx++){
						int fx = (x + dx - 2 + w) % w;
						sX += sobelX[dy][dx] * src[fy * w + fx];
						sY += sobelY[dy][dx] * src[fy * w + fx];
					}
				}
				// Construct the components
				sX *= 1.0f / (48 * 255);
				sY *= 1.0f / (48 * 255);

				// Normalize and store
				float invLen = 1.0f / sqrtf(sX * sX + sY * sY + sZ * sZ);
				float rX = xFactor * (sX * invLen + bias);
				float rY = yFactor * (sY * invLen + bias);
				float rZ = zFactor * (sZ * invLen + bias);

				uint64 result = 0;
				result |= uint64(int(rX) & xMask) << xShift;
				result |= uint64(int(rY) & yMask) << yShift;
				result |= uint64(int(rZ) & zMask) << zShift;
				result |= uint64((src[y * w + x] * hFactor) & hMask) << hShift;

				if (use32){
                    *dest32++ = (uint32) result;
				} else if (use16){
                    *dest16++ = (uint16) result;
				} else {
					*dest64++ = result;
				}
			}
		}
		sZ *= mipMapScaleZ;
	}

	format = destFormat;
	uint size_=getMipMappedSize(0, nMipMaps, destFormat);
	pixels.resize(size_);
	stx_memcpy(&pixels[0] , (ubyte *) &newPixels[0], size_);

	return true;
}

bool Image3::toGrayScale(){
	int nChannels = getChannelCount(format);

	if (!isPlainFormat(format) || isFloatFormat(format) || nChannels < 3) return false;

	uint len = getPixelCount(0, nMipMaps) * arraySize;
	uint size = len;

	if (format <= FORMAT_RGBA8){
		ubyte *src = &pixels[0], *dest = &pixels[0];
		do {
			*dest++ = (77 * src[0] + 151 * src[1] + 28 * src[2] + 128) >> 8;
			src += nChannels;
		} while (--len);

		format = FORMAT_I8;
	} else {
		ushort *src = (ushort *) &pixels[0], *dest = (ushort *) &pixels[0];
		do {
			*dest++ = (77 * src[0] + 151 * src[1] + 28 * src[2] + 128) >> 8;
			src += nChannels;
		} while (--len);

		format = FORMAT_I16;
		size *= 2;
	}

	pixels.resize(size);

	return true;
}

bool Image3::getRange(float &min, float &max){
	// Only float supported at this point
	if (format < FORMAT_R32F || format > FORMAT_RGBA32F) return false;

	int nElements = getPixelCount(0, nMipMaps) * getChannelCount(format) * arraySize;
	if (nElements <= 0) return false;

	float minVal =  FLT_MAX;
	float maxVal = -FLT_MAX;
	for (int i = 0; i < nElements; i++){
		float d = ((float *) &pixels[0])[i];
		if (d < minVal) minVal = d;
		if (d > maxVal) maxVal = d;
	}
	min = minVal;
	max = maxVal;

	return true;
}

bool Image3::scaleBias(const float scale, const float bias){
	// Only float supported at this point
	if (format < FORMAT_R32F || format > FORMAT_RGBA32F) return false;

	int nElements = getPixelCount(0, nMipMaps) * getChannelCount(format) * arraySize;

	for (int i = 0; i < nElements; i++){
		float d = ((float *) &pixels[0])[i];
		((float *) &pixels[0])[i] = d * scale + bias;
	}

	return true;
}

bool Image3::normalize(){
	// Only float supported at this point
	if (format < FORMAT_R32F || format > FORMAT_RGBA32F) return false;

	float min, max;
	getRange(min, max);

	int nElements = getPixelCount(0, nMipMaps) * getChannelCount(format) * arraySize;

	float s = 1.0f / (max - min);
	float b = -min * s;
	for (int i = 0; i < nElements; i++){
		float d = ((float *) &pixels[0])[i];
		((float *) &pixels[0])[i] = d * s + b;
	}

	return true;
}

bool Image3::addAlphaChannel()
{
	//return true;// ???
	if (!isPlainFormat(format)) return false;
	uint srcChannels = getChannelCount(format);
	if (srcChannels!=3) return false;
		//removeChannels(true, true, true, false);
	std::vector<ubyte> newPixels(width*height*4);

	for(unsigned int y=0;y<height;y++)
	{
	for(unsigned int x=0;x<width;x++)
	{
		newPixels[(width*y+x)*4+0]=pixels[(width*y+x)*3+0];
		newPixels[(width*y+x)*4+1]=pixels[(width*y+x)*3+1];
		newPixels[(width*y+x)*4+2]=pixels[(width*y+x)*3+2];
		if(	(pixels[(width*y+x)*3+0]) ||
			(pixels[(width*y+x)*3+1]) ||
			(pixels[(width*y+x)*3+2]))
			newPixels[(width*y+x)*4+3]=255;
		else
			newPixels[(width*y+x)*4+3]=0;
	}
	}
	pixels.resize(0);
	pixels = newPixels;
	return true;
}

bool Image3::removeChannels(bool keepCh0, bool keepCh1, bool keepCh2, bool keepCh3){
	if (!isPlainFormat(format)) return false;

	uint srcChannels = getChannelCount(format);
	if (srcChannels < 4) keepCh3 = false;
	if (srcChannels < 3) keepCh2 = false;
	if (srcChannels < 2) keepCh1 = false;

	if (!(keepCh0 || keepCh1 || keepCh2 || keepCh3)) return false;
	uint destChannels = int(keepCh0) + int(keepCh1) + int(keepCh2) + int(keepCh3);
	if (srcChannels == destChannels) return true;


	uint nPixels = getPixelCount(0, nMipMaps) * arraySize;
	uint bpc = getBytesPerChannel(format);

	format = (FORMAT) (format + (destChannels - srcChannels));
	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps) * arraySize);

	if (bpc == 1){
		ubyte *src = &pixels[0];
		ubyte *dst = &newPixels[0];
		do {
			if (keepCh0) *dst++ = src[0];
			if (keepCh1) *dst++ = src[1];
			if (keepCh2) *dst++ = src[2];
			if (keepCh3) *dst++ = src[3];
			src += srcChannels;
		} while (--nPixels);
	} else if (bpc == 2){
		ushort *src = (ushort *) &pixels[0];
		ushort *dst = (ushort *) &newPixels[0];
		do {
			if (keepCh0) *dst++ = src[0];
			if (keepCh1) *dst++ = src[1];
			if (keepCh2) *dst++ = src[2];
			if (keepCh3) *dst++ = src[3];
			src += srcChannels;
		} while (--nPixels);
	} else {
		uint *src = (uint *) &pixels[0];
		uint *dst = (uint *) &newPixels[0];
		do {
			if (keepCh0) *dst++ = src[0];
			if (keepCh1) *dst++ = src[1];
			if (keepCh2) *dst++ = src[2];
			if (keepCh3) *dst++ = src[3];
			src += srcChannels;
		} while (--nPixels);
	}

	pixels.resize(0);
	pixels = newPixels;

	return true;
}

enum RESCALE_FILTER { RF_POINT, RF_LINEAR };

// Generic image resizing function. Works with 1D, 2D and 3D images.
template <typename DATA_TYPE>
void rescale(DATA_TYPE *src, DATA_TYPE *dest, const int srcW, const int srcH, const int srcD, const int destW, const int destH, const int destD, const unsigned int c, const RESCALE_FILTER filter){
	if (filter == RF_POINT){
		for (int z = 0; z < destD; z++){
			int dz = z * (srcD - 1) / destD;
			for (int y = 0; y < destH; y++){
				int dy = y * (srcH - 1) / destH;
				for (int x = 0; x < destW; x++){
					int dx = x * (srcW - 1) / destW;
					for (unsigned int i = 0; i < c; i++){
						*dest++ = src[((dz * srcH + dy) * srcW + dx) * c + i];
					}
				}
			}
		}

	} else {

		float sddx = float(srcW - 1) / max(destW - 1, 1);
		float sddy = float(srcH - 1) / max(destH - 1, 1);
		float sddz = float(srcD - 1) / max(destD - 1, 1);

		for (int z = 0; z < destD; z++){
			float sz = z * sddz;
			int dz = (int) sz;
			sz -= dz;
			int dzO = min(dz + 1, srcD - 1);
			for (int y = 0; y < destH; y++){
				float sy = y * sddy;
				int dy = (int) sy;
				sy -= dy;
				int dyO = min(dy + 1, srcH - 1);
				for (int x = 0; x < destW; x++){
					float sx = x * sddx;
					int dx = (int) sx;
					sx -= dx;
					int dxO = min(dx + 1, srcW - 1);
					for (unsigned int i = 0; i < c; i++){
						*dest++ = (DATA_TYPE) (
							((src[((dz  * srcH + dy ) * srcW + dx ) * c + i] * (1 - sx) +
							  src[((dz  * srcH + dy ) * srcW + dxO) * c + i] * (    sx)) * (1 - sy) +
							 (src[((dz  * srcH + dyO) * srcW + dx ) * c + i] * (1 - sx) +
							  src[((dz  * srcH + dyO) * srcW + dxO) * c + i] * (    sx)) * (    sy)) * (1 - sz) +
							((src[((dzO * srcH + dy ) * srcW + dx ) * c + i] * (1 - sx) +
							  src[((dzO * srcH + dy ) * srcW + dxO) * c + i] * (    sx)) * (1 - sy) +
							 (src[((dzO * srcH + dyO) * srcW + dx ) * c + i] * (1 - sx) +
							  src[((dzO * srcH + dyO) * srcW + dxO) * c + i] * (    sx)) * (    sy)) * (    sz));
					}
				}
			}
		}
	}
}

bool Image3::resize(const int w, const int h, const int d){
	if (!isPlainFormat(format) || nMipMaps > 1 || isCube()) return false;

	int c = getChannelCount(format);
	int cSize = getBytesPerChannel(format);
	std:vector<ubyte> newPixels(w * h * d * c * cSize);

	if (cSize == 1){
		rescale(&pixels[0], &newPixels[0], width, height, depth, w, h, d, c, RF_LINEAR);
	} else if (cSize == 2){
		rescale((unsigned short *) &pixels[0], (unsigned short *) &newPixels[0], width, height, depth, w, h, d, c, (format >= FORMAT_I16f)? RF_POINT : RF_LINEAR);
	} else {
		rescale((float *) &pixels[0], (float *) &newPixels[0], width, height, depth, w, h, d, c, RF_LINEAR);
	}

	pixels = newPixels;
	width  = w;
	height = h;
	depth  = d;

	return true;
}

// Encode a grayscale image as DXT
uint64 encodeGrayDXTColorBlock(const int w, const int h, const int xOff, const int yOff, ubyte *source){
	ubyte lo = 255;
	ubyte hi = 0;

	for (int y = 0; y < h; y++){
		ubyte *src = source + yOff * y;
		for (int x = 0; x < w; x++){
			if (*src < lo) lo = *src;
			if (*src > hi) hi = *src;

			src += xOff;
		}
	}

	ubyte hi5 = (hi + 4) >> 3;
	ubyte lo5 = (lo + 4) >> 3;

	if (hi5 == lo5){
		if (hi5 < 31){
            hi++;
			hi5++;
		} else {
            lo--;
			lo5--;
		}
	}
	int diff = hi - lo;

	uint c0 = hi5 | (hi5 << 6) | (hi5 << 11);
	uint c1 = lo5 | (lo5 << 6) | (lo5 << 11);

	uint64 block = c0 | (c1 << 16);

	static int kRemap[] = { 1, 3, 2, 0 };

	for (int y = 0; y < h; y++){
		ubyte *src = source + yOff * y;
		char shift = 8 * y + 32;
		for (int x = 0; x < w; x++){
			int k = (3 * (*src - lo) + (diff >> 1)) / diff;

			k = kRemap[k];

			block |= uint64(k) << shift;

			src += xOff;
			shift += 2;
		}
	}

	return block;
}

// Encode a DXT5 alpha block / 3Dc channel block
uint64 encodeDXT5AlphaBlock(const int w, const int h, const int xOff, const int yOff, ubyte *source){
	ubyte lo = 255;
	ubyte hi = 0;

	for (int y = 0; y < h; y++){
		ubyte *src = source + yOff * y;
		for (int x = 0; x < w; x++){
			if (*src < lo) lo = *src;
			if (*src > hi) hi = *src;

			src += xOff;
		}
	}

	if (hi == lo){
		if (hi < 255) hi++; else lo--;
	}
	int diff = hi - lo;

	uint64 block = hi | (lo << 8);

	static int kRemap[] = { 1, 7, 6, 5, 4, 3, 2, 0 };

	for (int y = 0; y < h; y++){
		ubyte *src = source + yOff * y;
		char shift = 12 * y + 16;
		for (int x = 0; x < w; x++){
			int k = (7 * (*src - lo) + (diff >> 1)) / diff;

			k = kRemap[k];

			block |= uint64(k) << shift;

			src += xOff;
			shift += 3;
		}
	}

	return block;
}

// Does a quick texture compression
void encodeCompressedImage(ubyte *dst, ubyte *source, const int width, const int height, const int nChannels, const FORMAT destFormat){
	int sx = (width  < 4)? width  : 4;
	int sy = (height < 4)? height : 4;

	for (int y = 0; y < height; y += 4){
		for (int x = 0; x < width; x += 4){
			ubyte *src = source + (y * width + x) * nChannels;

			if (destFormat == FORMAT_DXT1){
				*(uint64 *) dst = encodeGrayDXTColorBlock(sx, sy, 1, width, src);
				dst += 8;
			} else {
				if (destFormat == FORMAT_ATI2N){
					*(uint64 *) dst = encodeDXT5AlphaBlock(sx, sy, nChannels, width * nChannels, src + 1);
					dst += 8;
				}

				*(uint64 *) dst = encodeDXT5AlphaBlock(sx, sy, nChannels, width * nChannels, src);
				dst += 8;
			}
		}
	}
}

bool Image3::compress(const FORMAT destFormat){
	int nChannels = getChannelCount(format);

	if (destFormat == FORMAT_DXT1){
        if (nChannels != 1) return false;
	} else {
		if (destFormat != FORMAT_ATI1N && destFormat != FORMAT_ATI2N) return false;
	}

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps, destFormat));

	int level = 0;
	ubyte *src, *dst = &newPixels[0];
	while ((src = getPixels(level)) != NULL){
		int w = getWidth(level);
		int h = getHeight(level);
		int d = (depth == 0)? 6 : getDepth(level);

		int dstSliceSize = getImageSize(destFormat, w, h, 1, 1);
		int srcSliceSize = getImageSize(format,     w, h, 1, 1);

		for (int slice = 0; slice < d; slice++){
			encodeCompressedImage(dst, src, w, h, nChannels, destFormat);

			dst += dstSliceSize;
			src += srcSliceSize;
		}
		level++;
	}
	
	format = destFormat;
	pixels = newPixels;
	return true;
}

bool Image3::decompress(){
	if (!isCompressedFormat(format)) return false;

	FORMAT destFormat;
	if (format == FORMAT_ATI1N){
		destFormat = FORMAT_I8;
	} else if (format == FORMAT_ATI2N){
		destFormat = FORMAT_IA8;
	} else {
		destFormat = (format == FORMAT_DXT1)? FORMAT_RGB8 : FORMAT_RGBA8;
	}

	std::vector<ubyte> newPixels(getMipMappedSize(0, nMipMaps, destFormat));

	int level = 0;
	ubyte *src, *dst = &newPixels[0];
	while ((src = getPixels(level)) != NULL){
		int w = getWidth(level);
		int h = getHeight(level);
		int d = (depth == 0)? 6 : getDepth(level);

		int dstSliceSize = getImageSize(destFormat, w, h, 1, 1);
		int srcSliceSize = getImageSize(format,     w, h, 1, 1);

		for (int slice = 0; slice < d; slice++){
			decodeCompressedImage(dst, src, w, h, format);

			dst += dstSliceSize;
			src += srcSliceSize;
		}
		level++;
	}
	
	format = destFormat;
	pixels = newPixels;
	return true;
}

int getImageSize(const FORMAT format, int w, int h, int d, int nMipMapLevels){
	bool isCubemap = (d == 0);
	if (isCubemap) d = 1;

	int size = 0;
	while (nMipMapLevels){
		if (isCompressedFormat(format)){
			size += ((w + 3) >> 2) * ((h + 3) >> 2) * d;
		} else {
			size += w * h * d;
		}
		if (w == 1 && h == 1 && d == 1) break;
		if (w > 1) w >>= 1;
		if (h > 1) h >>= 1;
		if (d > 1) d >>= 1;
		nMipMapLevels--;
	}

	if (isCompressedFormat(format)){
		size *= getBytesPerBlock(format);
	} else {
		size *= getBytesPerPixel(format);
	}

	if (isCubemap) size *= 6;

	return size;
}/*
Supported   Not yet supported:
layout:
  []          []
[][][]      [][][][]
  []          []
  []
*/
bool Image3::crossToCube(){
	if (depth != 1 || isCompressedFormat(format)) return false;

	height /= 4;
	width = height;
	depth = 0;
	nMipMaps = 1;
	std::vector<ubyte> newPixels(getMipMappedSize(0, 1));

	int bytes = getBytesPerPixel(format);
	int destLineSize = width * bytes;

	int offX[6] = { 2 * width,  0,      width, width,      width,  width      };
	int offY[6] = { height,     height, 0,     height * 2, height, 3 * height };
	bool flipX[6] = { false, false, false, false, false, true };
	bool flipY[6] = { false, false, false, false, false, true };

	ubyte *dest = &newPixels[0];
	for (int i = 0; i < 6; i++){
		int srcLineSize = 3 * destLineSize;
		ubyte *src = &pixels[0] + offY[i] * srcLineSize + offX[i] * bytes;
		if (flipY[i]){
			src += srcLineSize * (height - 1);
			srcLineSize = -srcLineSize;
		}
		for (int y = 0; y < height; y++){
			if (flipX[i]){
				for (int x = width - 1; x >= 0; x--){
					stx_memcpy(dest, src + x * bytes, bytes);
					dest += bytes;
				}
			} else {
				stx_memcpy(dest, src, destLineSize);
				dest += destLineSize;
			}
			src += srcLineSize;
		}
	}

	pixels = newPixels;
	return true;
}



// Generic mipmap generation function. Works with 1D, 2D and 3D surfaces.
template <typename DATA_TYPE>
void generateMipMap(DATA_TYPE *src, DATA_TYPE *dest, const unsigned int w, const unsigned int h, const unsigned int d, const unsigned int c){
	unsigned int xOff = (w < 2)? 0 : c;
	unsigned int yOff = (h < 2)? 0 : c * w;
	unsigned int zOff = (d < 2)? 0 : c * w * h;

	for (unsigned int z = 0; z < d; z += 2){
		for (unsigned int y = 0; y < h; y += 2){
			for (unsigned int x = 0; x < w; x += 2){
				for (unsigned int i = 0; i < c; i++){
					*dest++ = (src[0] + src[xOff] + src[yOff] + src[yOff + xOff] + src[zOff] + src[zOff + xOff] + src[zOff + yOff] + src[zOff + yOff + xOff]) / 8;
					src++;
				}
				src += xOff;
			}
			src += yOff;
		}
		src += zOff;
	}
}

bool Image3::generateMipMaps(const int mipMapCount){
	if (!isPlainFormat(format)) return false;

	nMipMaps = min(getMipMapCountFromDim(max(max(width, height), depth)), mipMapCount);
	pixels.resize(getMipMappedSize());

	ubyte *dest, *src = &pixels[0];
	int c = getChannelCount(format);
	int cSize = getBytesPerChannel(format);
	if (isCube()){
		int dim = width;
		for (int i = 1; i < nMipMaps; i++){
			int sliceSize = dim * dim * c * cSize;
			dest = src + 6 * sliceSize;

			for (unsigned int s = 0; s < 6; s++){
				if (cSize == 1){
					generateMipMap(src + s * sliceSize, dest + s * sliceSize / 4, dim, dim, 1, c);
				} else if (cSize == 2){
					generateMipMap((unsigned short *) (src + s * sliceSize), (unsigned short *) (dest + s * sliceSize / 4), dim, dim, 1, c);
				} else {
					generateMipMap((float *) (src + s * sliceSize), (float *) (dest + s * sliceSize / 4), dim, dim, 1, c);
				}
			}

			src = dest;
			dim >>= 1;
		}
	} else {
		int w = width;
		int h = height;
		int d = depth;
		for (int i = 1; i < nMipMaps; i++){
			dest = src + w * h * d * c * cSize;

			if (cSize == 1){
				generateMipMap(src, dest, w, h, d, c);
			} else if (cSize == 2){
				generateMipMap((unsigned short *) src, (unsigned short *) dest, w, h, d, c);
			} else {
				generateMipMap((float *) src, (float *) dest, w, h, d, c);
			}

			src = dest;
			if (w > 1) w >>= 1;
			if (h > 1) h >>= 1;
			if (d > 1) d >>= 1;
		}
	}

	return true;
}

bool Image3::removeMipMaps(const int firstMipMap, int mipMapsToKeep){
	if (firstMipMap >= nMipMaps) return false;

	int mipMapCount = min(firstMipMap + mipMapsToKeep, nMipMaps) - firstMipMap;

	int size = getMipMappedSize(firstMipMap, mipMapCount);
	std::vector<ubyte> newPixels(size);

	stx_memcpy(&newPixels[0], getPixels(firstMipMap), size);

	pixels = newPixels;
	width  = getWidth (firstMipMap);
	height = getHeight(firstMipMap);
	depth  = getDepth (firstMipMap);
	nMipMaps = mipMapCount;

	return true;
}

