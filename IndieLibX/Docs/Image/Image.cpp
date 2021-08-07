//-----------------------------------------------------------------------------
// File: Framework\Imaging\Image.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------


#define XMD_H 1


#include <stdio.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>

#if 1
// Use system provided headers on Linux
#ifndef NO_PNG
#include <png.h>
#endif
#ifndef NO_JPEG
extern "C" {
#include <jpeglib.h>
}
#endif
#ifndef NO_TIFF
#include <tiffio.h>
#endif

#endif

// Pack header tightly
#pragma pack (push, 1)

struct DDSHeader {
	uint dwMagic;
	uint dwSize;
	uint dwFlags;
	uint dwHeight;
	uint dwWidth;
	uint dwPitchOrLinearSize;
	uint dwDepth; 
	uint dwMipMapCount;
	uint dwReserved[11];

	struct {
		uint dwSize;
		uint dwFlags;
		uint dwFourCC;
		uint dwRGBBitCount;
		uint dwRBitMask;
		uint dwGBitMask;
		uint dwBBitMask;
		uint dwRGBAlphaBitMask; 
	} ddpfPixelFormat;

	struct {
		uint dwCaps1;
		uint dwCaps2;
		uint Reserved[2];
	} ddsCaps;

	uint dwReserved2;
};

// DDS file format flags
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

#define DDSCAPS2_VOLUME  0x00200000
#define DDSCAPS2_CUBEMAP 0x00000200

#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000

#define DDSCAPS2_CUBEMAP_ALL_FACES (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

struct TGAHeader {
	ubyte  descLen;
	ubyte  cmapType;
	ubyte  imageType;
	ushort cmapStart;
	ushort cmapEntries;
	ubyte  cmapBits;
	ushort xOffset;
	ushort yOffset;
	ushort width;
	ushort height;
	ubyte  bpp;
	ubyte  attrib;
};

struct BMPHeader {
	ushort bfType;
	uint   bfSize;
	ushort bfReserved1;
	ushort bfReserved2;
	uint   bfOffBits;
	uint   biSize;
	uint   biWidth;
	uint   biHeight;
	ushort biPlanes;
	ushort biBitCount;
	uint   biCompression;
	uint   biSizeImage;
	uint   biXPelsPerMeter;
	uint   biYPelsPerMeter;
	uint   biClrUsed;
	uint   biClrImportant;
};

#pragma pack (pop)
/*
inline bool isCompressedFormat(const FORMAT format){
    return (format >= FORMAT_DXT1) && (format <= FORMAT_ATI2N);
}

// Does not accept compressed formats
inline int getBytesPerPixel(const FORMAT format){
    static const int bytesPP[] = {
        0,
        1, 2, 3, 4,       //  8-bit unsigned
        2, 4, 6, 8,       // 16-bit unsigned
        1, 2, 3, 4,       //  8-bit signed
        2, 4, 6, 8,       // 16-bit signed
        2, 4, 6, 8,       // 16-bit float
        4, 8, 12, 16,     // 32-bit float
        2, 4, 6, 8,       // 16-bit unsigned integer
        4, 8, 12, 16,     // 32-bit unsigned integer
        2, 4, 6, 8,       // 16-bit signed integer
        4, 8, 12, 16,     // 32-bit signed integer
        4, 4, 4, 2, 2, 4, // Packed
        2, 4, 4, 4,       // Depth
    };
    return bytesPP[format];
}

// Accepts only compressed formats
inline int getBytesPerBlock(const FORMAT format){
    return (format == FORMAT_DXT1 || format == FORMAT_ATI1N)? 8 : 16;
}
*/
ILenum ilFormat[]={0}
ILenum ilType[]={0}
struct Image4 : public Image3
{
    /*
   int getMipMappedSize(const int firstMipMapLevel = 0, int nMipMapLevels = ALL_MIPMAPS, FORMAT srcFormat = FORMAT_NONE) const
   {
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
   }*/
   Image4():Image3(){}
	virtual ~Image4(){}
    void clear(){};
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
	bool loadJPG(const char*);
	bool loadPNG(const char*);
	bool loadTGA(const char*);
	bool loadBMP(const char*);
	bool loadTIF(const char*);
	bool loadFromFile(const char*);
	bool saveDDS(const char*, bool);
}
bool Image4::loadDDS(const char *fileName){
	clear();

	DDSHeader header;
	FILE *file;

	if ((file = fopen(fileName, "rb")) == 0) return false;

	fread(&header, sizeof(header), 1, file);
	if (header.dwMagic != FOURCC('D', 'D', 'S', ' ')){
		fclose(file);
		return false;
	}

	int width    = header.dwWidth;
	int height   = header.dwHeight;
	int depth    = header.dwDepth;
	int nMipMaps = header.dwMipMapCount;
	int arraySize = 1;
    ubyte* pixels.data()=create(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);


	if (header.ddsCaps.dwCaps2 & DDSCAPS2_CUBEMAP){
		depth = 0;
	} else {
		if (depth == 0) depth = 1;
	}
	if (nMipMaps <= 0) nMipMaps = 1;

	// Identify the image format
 	switch (header.ddpfPixelFormat.dwFourCC){
		case FOURCC('D', 'X', 'T', '1'):
			format = FORMAT_DXT1;
			break;
		case FOURCC('D', 'X', 'T', '3'):
			format = FORMAT_DXT3;
			break;
		case FOURCC('D', 'X', 'T', '5'):
			format = FORMAT_DXT5;
			break;
		case FOURCC('A', 'T', 'I', '1'):
			format = FORMAT_ATI1N;
			break;
		case FOURCC('A', 'T', 'I', '2'):
			format = FORMAT_ATI2N;
			break;
		case 34:
			format = FORMAT_RG16;
			break;
		case 36:
			format = FORMAT_RGBA16;
			break;
		case 111:
			format = FORMAT_R16f;
			break;
		case 112:
			format = FORMAT_RG16f;
			break;
		case 113:
			format = FORMAT_RGBA16f;
			break;
		case 114:
			format = FORMAT_R32f;
			break;
		case 115:
			format = FORMAT_RG32f;
			break;
		case 116:
			format = FORMAT_RGBA32f;
			break;
		default:
			switch (header.ddpfPixelFormat.dwRGBBitCount){
				case 8:
					if (header.ddpfPixelFormat.dwRBitMask == 0xE0){
						format = FORMAT_RGB332;
					} else {
						format = FORMAT_I8;
					}
					break;
				case 16:
					if (header.ddpfPixelFormat.dwRGBAlphaBitMask){
						format = FORMAT_IA8;
					} else {
						format = FORMAT_I16;
					}
					break;
				case 24:
					format = FORMAT_RGB8;
					break;
				case 32:
					if (header.ddpfPixelFormat.dwRBitMask == 0x3FF00000){
						format = FORMAT_RGB10A2;
					} else {
						format = FORMAT_RGBA8;
					}
					break;
				default:
					fclose(file);
					return false;
			}
	}

	// Load the image
	int size = getMipMappedSize(0, nMipMaps);
    pixels.data() = new ubyte[size];
	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
				ubyte *src = getPixels(mipMapLevel) + face * faceSize;

				fread(src, 1, faceSize, file);
			}
		}
	} else {
        fread(pixels.data(), 1, size, file);
	}

	// Swap channels for formats stored in BGR order
	if ((format == FORMAT_RGB8 || format == FORMAT_RGBA8) && header.ddpfPixelFormat.dwBBitMask == 0xFF){
		int nChannels = getChannelCount(format);
        swap(pixels.data(), size / nChannels, nChannels, 0, 2);
	}

	fclose(file);
	return true;
}

bool Image4::loadTGA(const char *fileName){
	clear();

	TGAHeader header;
	FILE *file;

	if ((file = fopen(fileName, "rb")) == 0) return false;
	fread(&header, sizeof(header), 1, file);
	// Skip past the description if there's any
	if (header.descLen) fseek(file, header.descLen, SEEK_CUR);


    int width    = header.width;
    int height   = header.height;
	int depth    = 1;
	int nMipMaps = 1;
	int arraySize = 1;
    ubyte* pixels.data()=create(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);



	int pixelSize = header.bpp / 8;
	int size = width * height * pixelSize;

	ubyte *readPixels = new ubyte[size];
	fread(readPixels, size, 1, file);
	fclose(file);

	ubyte *dest, *src = readPixels + width * (height - 1) * pixelSize;

	int x, y;
	switch (header.bpp) {
		case 8:
			format = FORMAT_I8;
            dest = pixels.data() = new ubyte[width * height];
			for (y = 0; y < height; y++){
				memcpy(dest, src, width);
				dest += width;
				src -= width;
			}
			break;
		case 16:
			format = FORMAT_RGBA8;
            dest = pixels.data() = new ubyte[width * height * 4];
			for (y = 0; y < height; y++){
				for (x = 0; x < width; x++){
					unsigned short tempPixel = *((unsigned short *) src);

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
            dest = pixels.data() = new ubyte[width * height * 3];
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
            dest = pixels.data() = new ubyte[width * height * 4];
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

	if (header.attrib & 0x20) flip();

	delete readPixels;
	return true;
}

bool Image4::loadBMP(const char *fileName){
	clear();

	FILE *file = fopen(fileName, "rb");
	if (file == 0) return false;
	
	// Read the header
	BMPHeader header;
	fread(&header, sizeof(header), 1, file);
	if (header.bfType != 0x4D42){
		fclose(file);
		return false;
	}


	int width    = header.biWidth;
	int height   = header.biHeight;
	int depth    = 1;
	int nMipMaps = 1;
	int arraySize = 1;
    ubyte* pixels.data()=create(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);


	switch (header.biBitCount){
		case 8:
			// No support for RLE compressed bitmaps
			if (header.biCompression){
				fclose(file);
				return false;
			}
			format = FORMAT_RGB8;
            pixels.data() = new ubyte[width * height * 3];

			ubyte palette[1024];
			fread(palette, sizeof(palette), 1, file);

			// 8-bit paletted bitmap
			ubyte *tmp;
			tmp = new ubyte[width];
			for (int i = height - 1; i >= 0; i--){
                ubyte *dest = pixels.data() + i * width * 3;
				fread(tmp, width, 1, file);
				int len = width;
				do {
					ubyte *src = palette + ((*tmp++) << 2);
					*dest++ = src[2];
					*dest++ = src[1];
					*dest++ = src[0];
				} while (--len);
				tmp -= width;
			}
			delete tmp;
			break;
		case 24:
		case 32:
			int nChannels;
			if (header.biBitCount == 24){
				nChannels = 3;
				format = FORMAT_RGB8;
			} else {
				nChannels = 4;
				format = FORMAT_RGBA8;
			}
            pixels.data() = new ubyte[width * height * nChannels];
			for (int i = height - 1; i >= 0; i--){
                ubyte *dest = pixels.data() + i * width * nChannels;
				fread(dest, width * nChannels, 1, file);
				swap(dest, width, nChannels, 0, 2);
			}
			break;
		default:
			fclose(file);
			return false;
	}

	fclose(file);

	return true;
}

#ifndef NO_PNG
bool Image4::loadPNG(const char *fileName){
	clear();

    int width    = 1;
    int height   = 1;
    int depth    = 1;
    int nMipMaps = 1;
    int arraySize = 1;
    ubyte* pixels.data()=create(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

    png_structp png_ptr = 0;
	png_infop info_ptr = 0;
	FILE *file;

	png_byte pbSig[8];
	int iBitDepth, iColorType;
	double dGamma;
	png_color_16 *pBackground;
	png_byte **ppbRowPointers;

	if ((file = fopen(fileName, "rb")) == 0) return false;

	// Identify as PNG file
	fread(pbSig, 1, 8, file);
	if (!png_check_sig(pbSig, 8)){
		fclose(file);
		return false;
	}

	// Create the two png-info structures
	if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, (png_error_ptr) 0, (png_error_ptr) 0)) == 0){
		fclose(file);
		return false;
	}

	if ((info_ptr = png_create_info_struct(png_ptr)) == 0){
		png_destroy_read_struct(&png_ptr, 0, 0);
		fclose(file);
		return false;
	}

	// Initialize the png structure
	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);

	// Read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// Get width, height, bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) &width, (png_uint_32 *) &height, &iBitDepth, &iColorType, 0, 0, 0);
	depth = 1;
	nMipMaps = 1;

	// Expand all images to 8 bits / channel
	if (iBitDepth == 16) png_set_strip_16(png_ptr);
	if (iColorType == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (iBitDepth < 8) png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);

	// Set the background color to draw transparent and alpha images over.
	if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
		png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);

	// Set gamma conversion if required
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, 2.2, dGamma);

	// After the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);

	// Get all info again
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) &width, (png_uint_32 *) &height, &iBitDepth, &iColorType, 0, 0, 0);
	int nChannels = png_get_channels(png_ptr, info_ptr);
	switch (nChannels){
		case 1:
			format = FORMAT_I8;
			break;
		case 2:
			format = FORMAT_IA8;
			break;
		case 3:
			format = FORMAT_RGB8;
			break;
		case 4:
			format = FORMAT_RGBA8;
			break;
	}

	// Allocate memory
    pixels.data() = new ubyte[width * height * nChannels];

	// Pointers for scanlines
	ppbRowPointers = new png_bytep[height];
	for (int i = 0; i < height; i++)
        ppbRowPointers[i] = pixels.data() + i * width * nChannels;

	png_read_image(png_ptr, ppbRowPointers);
	png_read_end(png_ptr, 0);

	delete ppbRowPointers;

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	fclose(file);
	return true;
}
#endif

#ifndef NO_JPEG
bool Image4::loadJPG(const char *fileName){
	clear();

	FILE *file;
	if ((file = fopen(fileName, "rb")) == 0) return false;

	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, file);
	jpeg_read_header(&cinfo, TRUE);

	format = (FORMAT) cinfo.num_components;

	jpeg_start_decompress(&cinfo);


	int width  = cinfo.output_width;
	int height = cinfo.output_height;
	int depth    = 1;
	int nMipMaps = 1;
	int arraySize = 1;
    ubyte* pixels.data()=create(FORMAT_RGB8, width, height, depth, nMipMaps, arraySize);

    pixels.data() = new ubyte[width * height * cinfo.num_components];

    // Read all pixels.data()
    ubyte *dest = pixels.data();
	while (cinfo.output_scanline < cinfo.output_height){
		jpeg_read_scanlines(&cinfo, &dest, 1);
		dest += width * cinfo.num_components;
	}

	// Clean up
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(file);
	return true;
}
#endif

#ifndef NO_TIFF
bool Image4::loadTIF(const char *fileName){
	clear();

	TIFF *tif = TIFFOpen(fileName, "r");
	if (tif == 0) return false;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	if (!TIFFGetField(tif, TIFFTAG_IMAGEDEPTH, &depth)) depth = 1;

	unsigned short config, photo;
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	if (config != PLANARCONFIG_CONTIG){
		TIFFClose(tif);
		return false;
	}

	// Support standard grayscale and RGB formats
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
	if (config != PHOTOMETRIC_MINISBLACK && photo != PHOTOMETRIC_RGB){
		TIFFClose(tif);
		return false;
	}

	unsigned short bitdepth, components, datatype;
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,   &bitdepth);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &components);
	if (!TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &datatype)){
		datatype = SAMPLEFORMAT_UINT;
	}

	if (datatype >= SAMPLEFORMAT_VOID){
		TIFFClose(tif);
		return false;
	}

	unsigned int bytes = components * bitdepth / 8;

	nMipMaps = 1;
	format = (components == 1)? FORMAT_I8 : (components == 3)? FORMAT_RGB8 : FORMAT_RGBA8;
	if (bitdepth == 16) (int &) format += (FORMAT_I16 - FORMAT_I8);
    pixels.data() = new ubyte[width * height * depth * bytes];

	if (TIFFIsTiled(tif)){
		unsigned int tWidth, tHeight, tDepth;
		TIFFGetField(tif, TIFFTAG_TILEWIDTH,  &tWidth);
		TIFFGetField(tif, TIFFTAG_TILELENGTH, &tHeight);
		if (!TIFFGetField(tif, TIFFTAG_TILEDEPTH,  &tDepth)){
			tDepth = 1;
		}

		ubyte *buf = new ubyte[TIFFTileSize(tif)];

		// For each tile ...
		for (int z = 0; z < depth; z += tDepth){
			for (int y = 0; y < height; y += tHeight){
				for (int x = 0; x < width; x += tWidth){
					// ... decode into buf ...
					TIFFReadTile(tif, buf, x, y, z, 0);
					// ... and place the results at the right place in the pixel buffer
					for (unsigned int j = 0; j < tDepth; j++){
						for (unsigned int i = 0; i < tHeight; i++){
                            memcpy(pixels.data() + (((z + j) * height + y + i) * width + x) * bytes, buf + (j * tHeight + i) * tWidth * bytes, tWidth * bytes);
						}
					}
				}
			}
		}

		delete buf;
	} else {
		unsigned int rowCount;
		TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowCount);
		unsigned int stripSize = TIFFStripSize(tif);

		// For each strip ...
		for (int i = 0; i < height * depth; i += rowCount){
			// ... decode directly into our pixel buffer
			tstrip_t strip = TIFFComputeStrip(tif, i, 0);
            TIFFReadEncodedStrip(tif, strip, pixels.data() + i * width * bytes, stripSize);
		}
	}

	TIFFClose(tif);
	return true;
}
#endif



bool Image4::loadFromFile(const char *fileName){
	const char *ext = strrchr(fileName, '.');
	if (ext == 0) return false;

	ext++;
	if (stricmp(ext, "dds") == 0){
		return loadDDS(fileName);
	} else if (stricmp(ext, "tga") == 0){
		return loadTGA(fileName);
	} else if (stricmp(ext, "bmp") == 0){
		return loadBMP(fileName);
#ifndef NO_PNG
	} else if (stricmp(ext, "png") == 0){
		return loadPNG(fileName);
#endif
#ifndef NO_TIFF
	} else if (stricmp(ext, "tif") == 0){
		return loadTIF(fileName);
#endif
#ifndef NO_JPEG
	} else if (stricmp(ext, "jpg") == 0){
		return loadJPG(fileName);
#endif
	}

	return false;
}


bool Image4::saveDDS(const char *fileName, bool allowOverwrite){
	int nChannels = getChannelCount(format);

	if (isPlainFormat(format)){
		if (nChannels == 3 && format != FORMAT_RGB8) return false;
	} else {
		if (!isCompressedFormat(format)) return false;
	}

	// FOURCC values for RG16 to RGBA32f formats
	static const uint wideFourCC[FORMAT_RGBA32f - FORMAT_RG16 + 1] = {
		34, 0, 36,
		111, 112, 0, 113,
		114, 115, 0, 116,
	};
	static const uint compressedFourCC[FORMAT_ATI2N - FORMAT_DXT1 + 1] = {
		FOURCC('D','X','T','1'), FOURCC('D','X','T','3'), FOURCC('D','X','T','5'),
		FOURCC('A','T','I','1'), FOURCC('A','T','I','2'),
	};

	// Set up the header
	DDSHeader header;
	header.dwMagic = FOURCC('D', 'D', 'S', ' ');
	header.dwSize = 124;
	header.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | (nMipMaps > 1? DDSD_MIPMAPCOUNT : 0) | (depth > 1? DDSD_DEPTH : 0);
	header.dwHeight = height;
	header.dwWidth  = width;
	header.dwPitchOrLinearSize = 0;
	header.dwDepth = (depth > 1)? depth : 0;
	header.dwMipMapCount = (nMipMaps > 1)? nMipMaps : 0;
	memset(header.dwReserved, 0, sizeof(header.dwReserved));

	header.ddpfPixelFormat.dwSize = 32;
	if (format <= FORMAT_I16){
		header.ddpfPixelFormat.dwFlags = ((nChannels < 3)? 0x00020000 : DDPF_RGB) | ((nChannels & 1)? 0 : DDPF_ALPHAPIXELS);
		header.ddpfPixelFormat.dwFourCC = 0;
		header.ddpfPixelFormat.dwRGBBitCount = 8 * getBytesPerChannel(format) * nChannels;
		if (format <= FORMAT_RGBA8){
			header.ddpfPixelFormat.dwRBitMask = (nChannels > 2)? 0x00FF0000 : 0xFF;
		} else {
			header.ddpfPixelFormat.dwRBitMask = 0xFFFF;
		}
		header.ddpfPixelFormat.dwGBitMask = (nChannels > 1)? 0x0000FF00 : 0;
		header.ddpfPixelFormat.dwBBitMask = (nChannels > 1)? 0x000000FF : 0;
		header.ddpfPixelFormat.dwRGBAlphaBitMask = (nChannels == 4)? 0xFF000000 : (nChannels == 2)? 0xFF00 : 0;
	} else {
		header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		if (isCompressedFormat(format)){
			header.ddpfPixelFormat.dwFourCC = compressedFourCC[format - FORMAT_DXT1];
		} else {
			header.ddpfPixelFormat.dwFourCC = wideFourCC[format - FORMAT_RG16];
		}
		header.ddpfPixelFormat.dwRGBBitCount = 0;
		header.ddpfPixelFormat.dwRBitMask = 0;
		header.ddpfPixelFormat.dwGBitMask = 0;
		header.ddpfPixelFormat.dwBBitMask = 0;
		header.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}

	header.ddsCaps.dwCaps1 = DDSCAPS_TEXTURE | (nMipMaps > 1? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) | (depth != 1? DDSCAPS_COMPLEX : 0);
	header.ddsCaps.dwCaps2 = (depth > 1)? DDSCAPS2_VOLUME : (depth == 0)? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
	header.ddsCaps.Reserved[0] = 0;
	header.ddsCaps.Reserved[1] = 0;
	header.dwReserved2 = 0;

	FILE *file;
	if (!allowOverwrite){
		if ((file = fopen(fileName, "rb")) != 0){
			fclose(file);
			return false;
		}
	}
	if ((file = fopen(fileName, "wb")) == 0) return false;

	// Write to file
	fwrite(&header, sizeof(header), 1, file);
	int size = getMipMappedSize(0, nMipMaps);

	// Swap to BGR pixel format
    if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swap(pixels.data(), size / nChannels, nChannels, 0, 2);

	if (isCube()){
		for (int face = 0; face < 6; face++){
			for (int mipMapLevel = 0; mipMapLevel < nMipMaps; mipMapLevel++){
				int faceSize = getMipMappedSize(mipMapLevel, 1) / 6;
				ubyte *src = getPixels(mipMapLevel) + face * faceSize;
				fwrite(src, 1, faceSize, file);
			}
		}
	} else {
        fwrite(pixels.data(), size, 1, file);
	}
	fclose(file);

	// Swap to BGR pixel format
    if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) swap(pixels.data(), size / nChannels, nChannels, 0, 2);

	return true;
}

	


