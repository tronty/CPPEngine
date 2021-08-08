//--------------------------------------------------------------------------------------
// File: JPEG_Decoder.h 
// JPEG decompression code based on a LGPL Small JPEG Decoder Library
// Copyright (C) 1994-2000 Rich Geldreich
// richgel@voicenet.com
//--------------------------------------------------------------------------------------

#include <Framework3/IRenderer.h>

typedef signed char     schar;
typedef unsigned char   uchar;
typedef signed short    int16;
typedef unsigned short  uint16;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;
typedef signed int      int32;

#define SUPPORT_X86ASM
#define DUMP_DCT
#define JPGD_INBUFSIZE       4096

#define JPGD_MAXBLOCKSPERMCU 10

#define JPGD_MAXHUFFTABLES   8
#define JPGD_MAXQUANTTABLES  4
#define JPGD_MAXCOMPONENTS   4
#define JPGD_MAXCOMPSINSCAN  4

#define JPGD_MAXBLOCKS    100

#define JPGD_MAX_HEIGHT 32768
#define JPGD_MAX_WIDTH  32768

// JPEG specific errors
#define JPGD_BAD_DHT_COUNTS              -200
#define JPGD_BAD_DHT_INDEX               -201
#define JPGD_BAD_DHT_MARKER              -202
#define JPGD_BAD_DQT_MARKER              -203
#define JPGD_BAD_DQT_TABLE               -204
#define JPGD_BAD_PRECISION               -205
#define JPGD_BAD_HEIGHT                  -206
#define JPGD_BAD_WIDTH                   -207
#define JPGD_TOO_MANY_COMPONENTS         -208
#define JPGD_BAD_SOF_LENGTH              -209
#define JPGD_BAD_VARIABLE_MARKER         -210
#define JPGD_BAD_DRI_LENGTH              -211
#define JPGD_BAD_SOS_LENGTH              -212
#define JPGD_BAD_SOS_COMP_ID             -213
#define JPGD_W_EXTRA_BYTES_BEFORE_MARKER -214
#define JPGD_NO_ARITHMITIC_SUPPORT       -215
#define JPGD_UNEXPECTED_MARKER           -216
#define JPGD_NOT_JPEG                    -217
#define JPGD_UNSUPPORTED_MARKER          -218
#define JPGD_BAD_DQT_LENGTH              -219
#define JPGD_TOO_MANY_BLOCKS             -221
#define JPGD_UNDEFINED_QUANT_TABLE       -222
#define JPGD_UNDEFINED_HUFF_TABLE        -223
#define JPGD_NOT_SINGLE_SCAN             -224gin
#define JPGD_UNSUPPORTED_COLORSPACE      -225
#define JPGD_UNSUPPORTED_SAMP_FACTORS    -226
#define JPGD_DECODE_ERROR                -227
#define JPGD_BAD_RESTART_MARKER          -228
#define JPGD_ASSERTION_ERROR             -229
#define JPGD_BAD_SOS_SPECTRAL            -230
#define JPGD_BAD_SOS_SUCCESSIVE          -231
#define JPGD_STREAM_READ                 -232
#define JPGD_NOTENOUGHMEM                -233

#define JPGD_GRAYSCALE 0
#define JPGD_YH1V1     1
#define JPGD_YH2V1     2
#define JPGD_YH1V2     3
#define JPGD_YH2V2     4

const int JPGD_FAILED = -1;
const int JPGD_DONE = 1;
const int JPGD_OKAY = 0;

typedef enum
{
    M_SOF0  = 0xC0,
    M_SOF1  = 0xC1,
    M_SOF2  = 0xC2,
    M_SOF3  = 0xC3,

    M_SOF5  = 0xC5,
    M_SOF6  = 0xC6,
    M_SOF7  = 0xC7,

    M_JPG   = 0xC8,
    M_SOF9  = 0xC9,
    M_SOF10 = 0xCA,
    M_SOF11 = 0xCB,

    M_SOF13 = 0xCD,
    M_SOF14 = 0xCE,
    M_SOF15 = 0xCF,

    M_DHT   = 0xC4,

    M_DAC   = 0xCC,

    M_RST0  = 0xD0,
    M_RST1  = 0xD1,
    M_RST2  = 0xD2,
    M_RST3  = 0xD3,
    M_RST4  = 0xD4,
    M_RST5  = 0xD5,
    M_RST6  = 0xD6,
    M_RST7  = 0xD7,

    M_SOI   = 0xD8,
    M_EOI   = 0xD9,
    M_SOS   = 0xDA,
    M_DQT   = 0xDB,
    M_DNL   = 0xDC,
    M_DRI   = 0xDD,
    M_DHP   = 0xDE,
    M_EXP   = 0xDF,

    M_APP0  = 0xE0,
    M_APP15 = 0xEF,

    M_JPG0  = 0xF0,
    M_JPG13 = 0xFD,
    M_COM   = 0xFE,

    M_TEM   = 0x01,

    M_ERROR = 0x100
} JPEG_MARKER;

#define RST0 0xD0

typedef struct huff_tables_tag
{
    uint  look_up[256];
    uchar code_size[256];
    uint  tree[512];
} huff_tables_t, *Phuff_tables_t;

typedef struct coeff_buf_tag
{
    uchar *Pdata;

    int block_num_x, block_num_y;
    int block_len_x, block_len_y;
    int block_size;

} coeff_buf_t, *Pcoeff_buf_t;

class jpeg_decoder;
typedef void (*Pdecode_block_func)(jpeg_decoder *, int, int, int);

class jpeg_decoder_stream
{
public:

    jpeg_decoder_stream() {}
    virtual ~jpeg_decoder_stream() {}
    virtual int read(uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag) = 0;
    virtual void attach() {}
    virtual void detach() {}
};

typedef jpeg_decoder_stream *Pjpeg_decoder_stream;

class jpeg_decoder_file_stream : public jpeg_decoder_stream
{
    FILE *Pfile;
    bool eof_flag, error_flag;

public:

    jpeg_decoder_file_stream()
    {
        Pfile = 0;
        eof_flag = false;
        error_flag = false;
    }

    void close(void)
    {
        if(Pfile)
        {
            fclose(Pfile);
            Pfile = 0;
        }

        eof_flag = false;
        error_flag = false;
    }

    virtual ~jpeg_decoder_file_stream()
    {
        close();
    }

    bool open( const char * fileName )
    {
        close();

        eof_flag = false;
        error_flag = false;

        freopen( fileName, "rb", Pfile );
    
        if( !Pfile )
            return false;

        return true;
    }

    virtual int read( uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag )
    {
        if( !Pfile )
            return -1;

        if( eof_flag )
        {
            *Peof_flag = true;
            return 0;
        }

        if( error_flag )
            return -1;

        int bytes_read = (int)fread(Pbuf, 1, max_bytes_to_read, Pfile);

        if( bytes_read < (int)max_bytes_to_read )
        {
            if( ferror( Pfile ) )
            {
                error_flag = true;
                return -1;
            }

            eof_flag = true;
            *Peof_flag = true;
        }

        return bytes_read;
    }

    bool get_error_status()
    {
        return (error_flag);
    }

    bool reset()
    {
        if( error_flag )
            return true;

        fseek(Pfile, 0, SEEK_SET);

        eof_flag = false;

        return false;
    }

    int get_size()
    {
        if( !Pfile )
        return -1;

        int loc = ftell(Pfile);
        fseek(Pfile, 0, SEEK_END);
        int size = ftell(Pfile);
        fseek(Pfile, loc, SEEK_SET);

        return (size);
    }
};

typedef jpeg_decoder_file_stream *Pjpeg_decoder_file_stream;

#define QUANT_TYPE int16
#define BLOCK_TYPE int16

#pragma warning(push)
#pragma warning( disable : 4035 4799 )

class jpeg_decoder
{
private:

    void free_all_blocks();
    void terminate( int status );
    void *alloc( int n );
    void word_clear( void *p, ushort c, uint n );
    void prep_in_buffer();
    void read_dht_marker();
    void read_dqt_marker();
    void read_sof_marker();
    void skip_variable_marker();
    void read_dri_marker();
    void read_sos_marker();
    int next_marker();
    int process_markers();
    void locate_soi_marker();
    void locate_sof_marker();
    int locate_sos_marker();

    void init(Pjpeg_decoder_stream Pstream);

    void fix_in_buffer();

    void make_huff_table( int index, Phuff_tables_t hs );

    void check_quant_tables();
    void check_huff_tables();
    void calc_mcu_block_order();
    int init_scan();

    void init_frame();
    void process_restart();
    void decode_scan( Pdecode_block_func decode_block_func );
    void decode_start();
    void decode_init( Pjpeg_decoder_stream Pstream );

    void find_eoi();

    inline uint jpeg_decoder::rol( uint i, uchar j );
    inline uint jpeg_decoder::get_char();
    inline uint jpeg_decoder::get_char( bool *Ppadding_flag );
    inline void jpeg_decoder::stuff_char( uchar q );
    inline uchar jpeg_decoder::get_octet();
    inline uint jpeg_decoder::get_bits_1( int num_bits );
    inline uint jpeg_decoder::get_bits_2( int numbits );
    inline int jpeg_decoder::huff_decode( Phuff_tables_t Ph );

    inline uint jpeg_decoder::huff_extend( uint i, int c );

    inline uchar jpeg_decoder::clamp(int i);

    int   image_x_size;
    int   image_y_size;

    Pjpeg_decoder_stream Pstream;

    uchar *huff_num[JPGD_MAXHUFFTABLES];  // Pointer to number of Huffman codes per bit size
    uchar *huff_val[JPGD_MAXHUFFTABLES];  // Pointer to Huffman codes per bit size

    QUANT_TYPE *quant[JPGD_MAXQUANTTABLES];    // Pointer to quantization tables

    int   scan_type;

    int   comps_in_frame;
    int   comp_h_samp[JPGD_MAXCOMPONENTS];     // Component's horizontal sampling factor
    int   comp_v_samp[JPGD_MAXCOMPONENTS];     // Component's vertical sampling factor
    int   comp_quant[JPGD_MAXCOMPONENTS];      // Component's quantization table selector
    int   comp_ident[JPGD_MAXCOMPONENTS];      // Component's ID

    int   comp_h_blocks[JPGD_MAXCOMPONENTS];
    int   comp_v_blocks[JPGD_MAXCOMPONENTS];

    int   comps_in_scan;                      // # of components in scan
    int   comp_list[JPGD_MAXCOMPSINSCAN];     // Components in this scan
    int   comp_dc_tab[JPGD_MAXCOMPONENTS];    // Component's DC Huffman coding table selector
    int   comp_ac_tab[JPGD_MAXCOMPONENTS];    // Component's AC Huffman coding table selector

    int   spectral_start;                 // Spectral selection start
    int   spectral_end;                   // Spectral selection end
    int   successive_low;                 // Successive approximation low
    int   successive_high;                // Successive approximation high

    int   max_mcu_x_size;                 // MCU's max. X size in pixels
    int   max_mcu_y_size;                 // MCU's max. Y size in pixels

    int   blocks_per_mcu;
    int   max_blocks_per_row;
    int   mcus_per_row, mcus_per_col;

    int   mcu_org[JPGD_MAXBLOCKSPERMCU];

    int   total_lines_left;               // Total # lines left in image
    int   mcu_lines_left;                 // Total # lines left in this MCU

    int   real_dest_bytes_per_scan_line;
    int   dest_bytes_per_scan_line;
    int   dest_bytes_per_pixel;

    Phuff_tables_t h[JPGD_MAXHUFFTABLES];

    Pcoeff_buf_t dc_coeffs[JPGD_MAXCOMPONENTS];
    Pcoeff_buf_t ac_coeffs[JPGD_MAXCOMPONENTS];

    int eob_run;

    int block_y_mcu[JPGD_MAXCOMPONENTS];

    uchar *Pin_buf_ofs;
    int in_buf_left;
    int tem_flag;
    bool eof_flag;

    uchar padd_1[128];
    uchar in_buf[JPGD_INBUFSIZE + 128];
    uchar padd_2[128];

    int   bits_left;
    union
    {
        uint bit_buf;
        uint bit_buf_64[2];
    };
  
    uint  saved_mm1[2];

    int   restart_interval;
    int   restarts_left;
    int   next_restart_num;

    int   max_mcus_per_row;
    int   max_blocks_per_mcu;

    int   max_mcus_per_col;

    uint *component[JPGD_MAXBLOCKSPERMCU];   // Points into the lastdcvals table
    uint  last_dc_val[JPGD_MAXCOMPONENTS];

    Phuff_tables_t dc_huff_seg[JPGD_MAXBLOCKSPERMCU];
    Phuff_tables_t ac_huff_seg[JPGD_MAXBLOCKSPERMCU];

    int   crr[256];
    int   cbb[256];
    int   padd;
    long  crg[256];
    long  cbg[256];

    BLOCK_TYPE temp_block[64];

    int error_code;
    bool ready_flag;

    int total_bytes_read;

public:

    jpeg_decoder(Pjpeg_decoder_stream Pstream);

    int begin();
    int decode( void** Pscan_line_ofs, uint *Pscan_line_len );
    int dump_DCT_data( HANDLE outFile );

    ~jpeg_decoder();

    int get_error_code() { return (error_code); }

    int get_width() { return image_x_size; }
    int get_height() { return (image_y_size); }
    int get_num_components() { return (comps_in_frame); }
    int get_bytes_per_pixel() { return (dest_bytes_per_pixel); }
    int get_bytes_per_scan_line() { return (image_x_size * get_bytes_per_pixel()); }
    int get_total_bytes_read() { return (total_bytes_read); }
    int get_blocks_per_mcu() { return blocks_per_mcu; }
    int get_mcus_per_row() { return mcus_per_row; }
    int getScanComponentsNum() { return comps_in_scan; }
    QUANT_TYPE* getQuantizationTable( int componentIndex ) { return quant[comp_quant[componentIndex]]; }
};

inline uint jpeg_decoder::rol( uint i, uchar j )
{
#if 0 // ???
    _asm
    {
        Mov eax, i
        Mov cl, j
        rol eax, cl
    }
#endif
}

inline uint jpeg_decoder::get_char()
{
    if( !in_buf_left )
    {
        prep_in_buffer();
    
        if( !in_buf_left )
        {
            int t = tem_flag;
            tem_flag ^= 1;
        
            if( t )
                return (0xD9);
            else
                return (0xFF);
        }
    }

    uint c = *Pin_buf_ofs++;
    in_buf_left--;

    return c;
}

inline uint jpeg_decoder::get_char( bool *Ppadding_flag )
{
    if( !in_buf_left )
    {
        prep_in_buffer();
        if( !in_buf_left )
        {
            *Ppadding_flag = true;
            int t = tem_flag;
            tem_flag ^= 1;
            if( t )
                return (0xD9);
            else
                return (0xFF);
        }
    }

    *Ppadding_flag = false;

    uint c = *Pin_buf_ofs++;
    in_buf_left--;

    return c;
}

inline void jpeg_decoder::stuff_char( uchar q )
{
    *(--Pin_buf_ofs) = q;
    in_buf_left++;
}

inline uchar jpeg_decoder::get_octet(void)
{
    bool padding_flag;
    int c = get_char(&padding_flag);

    if( c == 0xFF )
    {
        if( padding_flag )
            return (0xFF);

        c = get_char(&padding_flag);
        if( padding_flag )
        {
            stuff_char(0xFF);
            return (0xFF);
        }

        if( c == 0x00 )
            return (0xFF);
        else
        {
            stuff_char( (uchar)c );
            stuff_char(0xFF);
            return (0xFF);
        }
    }

    return (uchar)c;
}

inline uint jpeg_decoder::get_bits_1(int num_bits)
{
    uint i;

    i = (bit_buf >> (16 - num_bits)) & ((1 << num_bits) - 1);

    if( (bits_left -= num_bits) <= 0 )
    {
        bit_buf = rol(bit_buf, uchar(num_bits += bits_left));

        uint c1 = get_char();
        uint c2 = get_char();

        bit_buf = (bit_buf & 0xFFFF) | (((ulong)c1) << 24) | (((ulong)c2) << 16);
        bit_buf = rol(bit_buf, uchar(-bits_left));

        bits_left += 16;
    }
    else
        bit_buf = rol( bit_buf, uchar(num_bits) );

    return i;
}

inline uint jpeg_decoder::get_bits_2(int numbits)
{
    uint i;

    i = (bit_buf >> (16 - numbits)) & ((1 << numbits) - 1);

    if( (bits_left -= numbits) <= 0 )
    {
        bit_buf = rol(bit_buf, uchar(numbits += bits_left) );

        uint c1 = get_octet();
        uint c2 = get_octet();

        bit_buf = (bit_buf & 0xFFFF) | (((ulong)c1) << 24) | (((ulong)c2) << 16);
        bit_buf = rol( bit_buf, uchar(-bits_left) );

        bits_left += 16;
    }
    else
        bit_buf = rol( bit_buf, uchar(numbits) );

    return i;
}

inline int jpeg_decoder::huff_decode(Phuff_tables_t Ph)
{
    int symbol;

    if( ( symbol = Ph->look_up[(bit_buf >> 8) & 0xFF] ) < 0 )
    {
        get_bits_2(8);

        do
        {
            symbol = Ph->tree[~symbol + (1 - get_bits_2(1))];
        } while( symbol < 0 );
    }
    else
        get_bits_2(Ph->code_size[symbol]);

    return symbol;
}

const int extend_mask[] =
{
  (0),
  (1<<0), (1<<1), (1<<2), (1<<3),
  (1<<4), (1<<5), (1<<6), (1<<7),
  (1<<8), (1<<9), (1<<10), (1<<11),
  (1<<12), (1<<13), (1<<14), (1<<15),
  (1<<16),
};

#define HUFF_EXTEND_TBL(x,s) ((x) < extend_test[s] ? (x) + extend_offset[s] : (x))

#define HUFF_EXTEND(x,s) huff_extend(x, s)
#define HUFF_EXTEND_P(x,s) Pd->huff_extend(x, s)

inline uint jpeg_decoder::huff_extend(uint i, int c)
{
#if 0 // ???
    _asm
    {
        mov ecx, c
        mov eax, i
        cmp eax, [ecx*4+extend_mask]
        sbb edx, edx
        shl edx, cl
        adc eax, edx
    }
#endif
}

#pragma warning(pop)

typedef jpeg_decoder *Pjpeg_decoder;

#if 1
#ifdef _MSC_VER
typedef unsigned __int3264 ULONG_PTR;
#else
typedef VOID* ULONG_PTR;
#endif
typedef struct _OVERLAPPED {
  ULONG_PTR Internal;
  ULONG_PTR InternalHigh;
  union {
    struct {
      __DWORD__ Offset;
      __DWORD__ OffsetHigh;
    } DUMMYSTRUCTNAME;
    VOID* Pointer;
  } DUMMYUNIONNAME;
  HANDLE    hEvent;
} OVERLAPPED, *LPOVERLAPPED;
inline BOOL WriteFile(
  HANDLE       hFile,
  LPCVOID      lpBuffer,
  __DWORD__        nNumberOfBytesToWrite,
  LPDWORD      lpNumberOfBytesWritten,
  LPOVERLAPPED lpOverlapped
){return TRUE;}
#endif

