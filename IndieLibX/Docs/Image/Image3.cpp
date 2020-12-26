
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
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
#define XMD_H 1

#include <stdio.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>
#ifndef NO_PNG
#include <png.h>
#endif
#ifndef NO_JPEG
extern "C" {
 #include <jpeglib.h>
}
#endif
#ifndef NO_PNG
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
ILenum ilFormat[]={0}
ILenum ilType[]={0}
struct Image4 : public Image3
{
	Image4():Image3(){}
	virtual ~Image4(){}
    unsigned char *create(const FORMAT fmt, const int w, const int h,
                          const int d, const int mipMapCount, const int arraysize = 1)
    {
        ILuint handle;
        ilGenImages(1, & handle);
        ilBindImage(handle);
        ilTexImage(w, h, d, getChannelCount(fmt), ilFormat[fmt], ilType[fmt], 0);
        return ilGetData();
    }
    bool loadDDS(const char*, uint);
	bool loadJPEG(const char*);
	bool loadPNG(const char*);
	bool loadTGA(const char*);
	bool loadBMP(const char*);
    bool loadImage(const char*, uint);
    bool saveDDS(const char*);
    bool saveJPEG(const char*, int);
    bool savePNG(const char*);
    bool saveTGA(const char*);
    bool saveBMP(const char*);
    bool saveImage(const char*);
};

bool Image4::loadDDS(const char *fileName, uint flags){
	DDSHeader header;

	FILE *file;
	if ((file = fopen(fileName, "rb")) == 0) return false;

	fread(&header, sizeof(header), 1, file);
	if (header.dwMagic != MCHAR4('D','D','S',' ')){
		fclose(file);
		return false;
	}

	int width  = header.dwWidth;
	int height = header.dwHeight;
	int depth  = (header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP)? 0 : (header.dwDepth == 0)? 1 : header.dwDepth;
	int nMipMaps = ((flags & DONT_LOAD_MIPMAPS) || (header.dwMipMapCount == 0))? 1 : header.dwMipMapCount;
	int arraySize = 1;
 ubyte* ilGetData()reate(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

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

	int size = getMipMappedSize(0, nMipMaps);
    ilGetData() new unsigned char[size];
	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
                unsigned char *src = getPixels(mipMapLevel) + face * faceSize;

				fread(src, 1, faceSize, file);
			}
			if ((flags & DONT_LOAD_MIPMAPS) && header.dwMipMapCount > 1){
				fseek(file, getMipMappedSize(1, header.dwMipMapCount - 1) / 6, SEEK_CUR);
			}
		}
	} else {
        fread(ilGetData()1, size, file);
	}

	if ((format == FORMAT_RGB8 || format == FORMAT_RGBA8) && header.ddpfPixelFormat.dwBBitMask == 0xFF){
		int nChannels = getChannelCount(format);
        swapChannels(ilGetData()size / nChannels, nChannels, 0, 2);
	}

	fclose(file);
	return true;
}


#ifndef NO_JPEG
bool Image4::loadJPEG(const char *fileName){
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE *file;
	if ((file = fopen(fileName, "rb")) == 0) return false;
	
	jpeg_stdio_src(&cinfo, file);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

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


	int width  = cinfo.output_width;
	int height = cinfo.output_height;
	int depth  = 1;
	int nMipMaps = 1 ;
	int arraySize = 1;
 ubyte* ilGetData()reate(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

    ilGetData() new unsigned char[width * height * cinfo.num_components];
    unsigned char *curr_scanline = ilGetData()
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
bool Image4::loadPNG(const char *fileName){
	png_structp png_ptr = 0;
	png_infop info_ptr = 0;
    FILE *file;

    // open the PNG input file
    if ((file = fopen(fileName, "rb")) == 0) return false;

    // first check the eight byte PNG signature
    png_byte pbSig[8];
    fread(pbSig, 1, 8, file);
    if (!png_check_sig(pbSig, 8)){
		fclose(file);
		return false;
	}

    // create the two png(-info) structures
    if ((png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, 0, 0, 0, 0, malloc_fn, free_fn)) == 0){
		fclose(file);
        return false;
    }

    if ((info_ptr = png_create_info_struct(png_ptr)) == 0){
        png_destroy_read_struct(&png_ptr, 0, 0);
		fclose(file);
        return false;
    }

	// initialize the png structure
	png_set_read_fn(png_ptr, file, user_read_data);
	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// get width, height, bit-depth and color-type
	png_uint_32 w, h;
    int bitDepth, colorType;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bitDepth, &colorType, 0, 0, 0);


	int width  = w;
	int height = h;
	int depth  = 1;
	int nMipMaps = 1 ;
	int arraySize = 1;
 ubyte* ilGetData()reate(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

	int nChannels = png_get_channels(png_ptr, info_ptr);
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
    ilGetData() new ubyte[rowSize * height];
	
	// set the individual row-pointers to point at the correct offsets
    png_byte **ppbRowPointers = new png_bytep[height];
	for (int i = 0; i < height; i++)
        ppbRowPointers[i] = ilGetData() i * rowSize;

	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, 0);
	
	delete [] ppbRowPointers;

	if (colorType == PNG_COLOR_TYPE_PALETTE){
		png_colorp palette;
		int num_palette;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

		ubyte *newPixels = new ubyte[width * height * 3];
		if (bitDepth == 4){
			for (int i = 0; i < rowSize * height; i++){
                uint i0 = ilGetData()] >> 4;
                uint i1 = ilGetData()] & 0xF;
				newPixels[6 * i    ] = palette[i0].red;
				newPixels[6 * i + 1] = palette[i0].green;
				newPixels[6 * i + 2] = palette[i0].blue;
				newPixels[6 * i + 3] = palette[i1].red;
				newPixels[6 * i + 4] = palette[i1].green;
				newPixels[6 * i + 5] = palette[i1].blue;
			}
		} else {
			for (int i = 0; i < rowSize * height; i++){
                newPixels[3 * i    ] = palette[ilGetData()]].red;
                newPixels[3 * i + 1] = palette[ilGetData()]].green;
                newPixels[3 * i + 2] = palette[ilGetData()]].blue;
			}
		}
		format = FORMAT_RGB8;

        delete [] ilGetData()        ilGetData() newPixels;
	}

	if (bitDepth == 16){
		// Fix endian
		int size = width * height * nChannels * sizeof(ushort);
		for (int i = 0; i < size; i += 2){
            ubyte tmp = ilGetData()];
            ilGetData()ata(ililGetData()eilGetData()];
            ilGetData() + 1] = tmp;
		}
	}

	// and we're done
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    fclose(file);

    return true;
}
#endif // NO_PNG

#ifndef NO_TGA
bool Image4::loadTGA(const char *fileName){
	TGAHeader header;

	int size, x, y, pixelSize, palLength;
	unsigned char *tempBuffer, *fBuffer, *dest, *src;
	unsigned int tempPixel;
	unsigned char palette[768];
	FILE *file;


	if ((file = fopen(fileName, "rb")) == 0) return false;
	
	// Find file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the header
	fread(&header, sizeof(header), 1, file);
	

	int width  = header.width;
	int height = header.height;
	int depth  = 1;
	int nMipMaps = 1 ;
	int arraySize = 1;
 ubyte* ilGetData()reate(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

	pixelSize = header.bpp / 8;

	if ((palLength = header.descriptionlen + header.cmapentries * header.cmapbits / 8) > 0) fread(palette, sizeof(palette), 1, file);
	
	// Read the file data
	fBuffer = new unsigned char[size - sizeof(header) - palLength];
	fread(fBuffer, size - sizeof(header) - palLength, 1, file);
	fclose(file);

	size = width * height * pixelSize;

	tempBuffer = new unsigned char[size];

	// Decode if rle compressed. Bit 3 of .imagetype tells if the file is compressed
	if (header.imagetype & 0x08){
		unsigned int c,count;

		dest = tempBuffer;
		src = fBuffer;

		while (size > 0) {
			// Get packet header
			c = *src++;

			count = (c & 0x7f) + 1;
			size -= count * pixelSize;
			
			if (c & 0x80) {
				// Rle packet
				do {
					memcpy(dest,src,pixelSize);
					dest += pixelSize;
				} while (--count);
				src += pixelSize;
			} else {
				// Raw packet
				count *= pixelSize;
				memcpy(dest,src,count);
				src += count;
				dest += count;
			}
		}
		
		src = tempBuffer;
	} else {
		src = fBuffer;
	}

	src += (header.bpp / 8) * width * (height - 1);	

	switch(header.bpp) {
	case 8:
		if (palLength > 0){
			format = FORMAT_RGB8;
            dest = ilGetData() new unsigned char[width * height * 3];
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
            dest = ilGetData() new unsigned char[width * height];
			for (y = 0; y < height; y++){
				memcpy(dest, src, width);
				dest += width;
				src -= width;
			}
		}
		break;
	case 16:
		format = FORMAT_RGBA8;
        dest = ilGetData() new unsigned char[width * height * 4];
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
        dest = ilGetData() new unsigned char[width * height * 3];
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
        dest = ilGetData() new unsigned char[width * height * 4];
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

	delete [] tempBuffer;
	delete [] fBuffer;
	return true;
}
#endif

#ifndef NO_BMP
bool Image4::loadBMP(const char *fileName){
	BMPHeader header;

	FILE *file;
	unsigned char *dest, *src, *tmp;
	int i, len;
	unsigned char palette[1024];

	if ((file = fopen(fileName, "rb")) == 0) return false;
	
	// Read the header
	fread(&header, sizeof(header), 1, file);
	if (header.bmpIdentifier != MCHAR2('B', 'M')){
		fclose(file);
		return false;
	}

	int width  = header.width;
	int height = header.height;
	int depth  = 1;
	int nMipMaps = 1 ;
	int arraySize = 1;
 ubyte* ilGetData()reate(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);


	switch (header.bpp){
	case 8:
		if (header.compression){
			// No support for RLE compressed bitmaps
			fclose(file);
			return false;
		}
		format = FORMAT_RGB8;
        ilGetData() new unsigned char[width * height * 3];

		fread(palette, sizeof(palette), 1, file);
		swapChannels(palette, 256, 4, 0, 2);

		// Normal unencoded 8 bit paletted bitmap
		tmp = new unsigned char[width];
		for (i = height - 1; i >= 0; i--){
            dest = ilGetData() i * width * 3;
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
		delete [] tmp;
		break;
	case 24:
	case 32:
		int nChannels;
		nChannels = (header.bpp == 24)? 3 : 4;
		format    = (header.bpp == 24)? FORMAT_RGB8 : FORMAT_RGBA8;
        ilGetData() new unsigned char[width * height * nChannels];
		for (i = height - 1; i >= 0; i--){
            dest = ilGetData() i * width * nChannels;
			fread(dest, width * nChannels, 1, file);
			swapChannels(dest, width, nChannels, 0, 2);
		}
		break;
	default:
		fclose(file);
		return false;
	}

	fclose(file);

	return true;
}
#endif // NO_BMP


bool Image4::loadImage(const char *fileName, uint flags){
	const char *extension = strrchr(fileName, '.');

	clear();

	if (extension == 0) return false;

	if (stricmp(extension, ".dds") == 0){
		if (!loadDDS(fileName, flags)) return false;
	} else
#ifndef NO_JPEG
	if (stricmp(extension, ".jpg") == 0 || 
			 stricmp(extension, ".jpeg") == 0){
		if (!loadJPEG(fileName)) return false;
	}
#endif // NO_JPEG
#ifndef NO_PNG
	else if (stricmp(extension, ".png") == 0){
		if (!loadPNG(fileName)) return false;
	}
#endif // NO_PNG
#ifndef NO_TGA
	else if (stricmp(extension, ".tga") == 0){
		if (!loadTGA(fileName)) return false;
	}
#endif // NO_TGA
#ifndef NO_BMP
	else if (stricmp(extension, ".bmp") == 0){
		if (!loadBMP(fileName)) return false;
	}
#endif // NO_BMP
	else {
		return false;
	}
	return true;
}


bool Image4::saveDDS(const char *fileName){
	// Set up the header
	DDSHeader header;
	memset(&header, 0, sizeof(header));
	DDSHeaderDX10 headerDX10;
	memset(&headerDX10, 0, sizeof(headerDX10));

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
	if ((file = fopen(fileName, "wb")) == 0) return false;

	fwrite(&header, sizeof(header), 1, file);
	if (headerDX10.dxgiFormat) fwrite(&headerDX10, sizeof(headerDX10), 1, file);


	int size = getMipMappedSize(0, nMipMaps);

	// RGB to BGR
    if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swapChannels(ilGetData()size / nChannels, nChannels, 0, 2);

	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
                ubyte *src = getPixels(mipMapLevel) + face * faceSize;
				fwrite(src, 1, faceSize, file);
			}
		}
	} else {
        fwrite(ilGetData()size, 1, file);
	}
	fclose(file);

	// Restore to RGB
    if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swapChannels(ilGetData()size / nChannels, nChannels, 0, 2);

	return true;
}


#ifndef NO_JPEG
bool Image4::saveJPEG(const char *fileName, const int quality){
	if (format != FORMAT_I8 && format != FORMAT_RGB8) return false;

	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE *file;
	if ((file = fopen(fileName, "wb")) == 0) return false;

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

    unsigned char *curr_scanline = ilGetData()
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
bool Image4::savePNG(const char *fileName){
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

    if ((file = fopen(fileName, "wb")) == 0) return false;

    if ((png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, 0, 0, 0, 0, malloc_fn, free_fn)) == 0){
		fclose(file);
        return false;
    }

    if ((info_ptr = png_create_info_struct(png_ptr)) == 0){
        png_destroy_write_struct(&png_ptr, 0);
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
            ushort *src = ((ushort *) ilGetData()+ y * nElements;
			// Swap endian
			for (int x = 0; x < nElements; x++){
				line[x] = (src[x] >> 8) | (src[x] << 8);
			}
			png_write_row(png_ptr, (ubyte *) line);
		}
		delete [] line;
	} else {
		for (int y = 0; y < height; y++){
            png_write_row(png_ptr, ilGetData() y * nElements);
		}
	}

	png_write_end(png_ptr, info_ptr);
	
    png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(file);

	return true;
}
#endif // NO_PNG


#ifndef NO_TGA
bool Image4::saveTGA(const char *fileName){
	if (format != FORMAT_I8 && format != FORMAT_RGB8 && format != FORMAT_RGBA8) return false;

	FILE *file;
	if ((file = fopen(fileName, "wb")) == 0) return false;

	int nChannels = getChannelCount(format);

	TGAHeader header = {
		0x00,
		(format == FORMAT_I8)? 1 : 0,
		(format == FORMAT_I8)? 1 : 2,
		0x0000,
		(format == FORMAT_I8)? 256 : 0,
		(format == FORMAT_I8)? 24  : 0,
		0x0000,
		0x0000,
		width,
		height,
		nChannels * 8,
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

        src = ilGetData() width * height;
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
            src  = ilGetData() y * width * nChannels;
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
bool Image4::saveBMP(const char *fileName){
	if (format != FORMAT_I8 && format != FORMAT_RGB8 && format != FORMAT_RGBA8) return false;

	FILE *file;
	if ((file = fopen(fileName, "wb")) == 0) return false;

	int nChannels = getChannelCount(format);
	BMPHeader header = {
		MCHAR2('B','M'), 
		{0x36, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00},
		width,
		height,
		0x0001,
		nChannels * 8,
		0,
		{0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	fwrite(&header, sizeof(header), 1, file);

	if (format == FORMAT_I8){
		unsigned char pal[1024];
		for (int i = 0; i < 1024; i++){
			pal[i] = i >> 2;
		}
		fwrite(pal, sizeof(pal), 1, file);

        unsigned char *src = ilGetData() width * height;
		for (int y = 0; y < height; y++){
			src -= width;
			fwrite(src, width, 1, file);
		}
	} else {
		bool useAlpha = (nChannels == 4);
		int paddBytes = (width * nChannels) & 3;
		if (paddBytes) paddBytes = 4 - paddBytes;

		int len, size = (width * nChannels + paddBytes) * height;
		unsigned char *dest, *src, *buffer = new unsigned char[size];

		for (int y = 0; y < height; y++){
			dest = buffer + (height - y - 1) * (width * nChannels + paddBytes);
            src  = ilGetData() y * width * nChannels;
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


bool Image4::saveImage(const char *fileName){
	const char *extension = strrchr(fileName, '.');

	if (extension != 0){
		if (stricmp(extension, ".dds") == 0){
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
	}
	return false;
}


