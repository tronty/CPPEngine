//--------------------------------------------------------------------------------------
// File: JPEG_Decoder.cpp 
// JPEG decompression code based on a LGPL Small JPEG Decoder Library
// Copyright (C) 1994-2000 Rich Geldreich
// richgel@voicenet.com
//--------------------------------------------------------------------------------------

#include <Framework3/IRenderer.h>
#include "JPEG_Decoder.h"

static int zzOrder[64] =
{
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

void jpeg_decoder::free_all_blocks()
{
    if( Pstream )
    {
        Pstream->detach();
        Pstream = 0;
    }
}

void jpeg_decoder::terminate( int status )
{
    error_code = status;

    free_all_blocks();
}

void jpeg_decoder::word_clear( void *p, ushort c, uint n )
{
    ushort *ps = (ushort *)p;
    while( n )
    {
        *ps++ = c;
        n--;
    }
}

void jpeg_decoder::prep_in_buffer()
{
    in_buf_left = 0;
    Pin_buf_ofs = in_buf;

    if( eof_flag )
        return;

    do
    {
        int bytes_read = Pstream->read(in_buf + in_buf_left,
            JPGD_INBUFSIZE - in_buf_left,
            &eof_flag);

        if (bytes_read == -1)
            terminate(JPGD_STREAM_READ);

        in_buf_left += bytes_read;

    } while( (in_buf_left < JPGD_INBUFSIZE) && (!eof_flag) );

    total_bytes_read += in_buf_left;
    word_clear( Pin_buf_ofs + in_buf_left, 0xD9FF, 64) ;
}

//------------------------------------------------------------------------------
// Read a Huffman code table.
//------------------------------------------------------------------------------
void jpeg_decoder::read_dht_marker()
{
    int i, index, count;
    uint left;
    uchar huff_num[17];
    uchar huff_val[256];

    left = get_bits_1(16);

    if (left < 2)
        terminate(JPGD_BAD_DHT_MARKER);

    left -= 2;

    while( left )
    {
        index = get_bits_1(8);

        huff_num[0] = 0;

        count = 0;

        for (i = 1; i <= 16; i++)
        {
            huff_num[i] = (uchar)get_bits_1(8);
            count += huff_num[i];
        }

        if( count > 255 )
            terminate( JPGD_BAD_DHT_COUNTS );

        for (i = 0; i < count; i++)
            huff_val[i] = (uchar)get_bits_1(8);

        i = 1 + 16 + count;

        if( left < (uint)i )
            terminate( JPGD_BAD_DHT_MARKER );

        left -= i;

        if( (index & 0x10) > 0x10 )
            terminate( JPGD_BAD_DHT_INDEX );

        index = (index & 0x0F) + ( ( index & 0x10 ) >> 4 ) * ( JPGD_MAXHUFFTABLES >> 1 );

        if( index >= JPGD_MAXHUFFTABLES )
            terminate( JPGD_BAD_DHT_INDEX );

        if( !this->huff_num[index] )
            this->huff_num[index] = new uchar[17];

        if( !this->huff_val[index] )
            this->huff_val[index] = new uchar[256];

        stx_memcpy( this->huff_num[index], huff_num, 17 );
        stx_memcpy( this->huff_val[index], huff_val, 256 );
    }
}

//------------------------------------------------------------------------------
// Read a quantization table.
//------------------------------------------------------------------------------
void jpeg_decoder::read_dqt_marker()
{
    int n, i, prec;
    uint left;
    uint temp;

    left = get_bits_1(16);

    if( left < 2 )
        terminate( JPGD_BAD_DQT_MARKER );

    left -= 2;

    while( left )
    {
        n = get_bits_1(8);
        prec = n >> 4;
        n &= 0x0F;

        if( n >= JPGD_MAXQUANTTABLES )
            terminate( JPGD_BAD_DQT_TABLE );

        if( !quant[n] )
            quant[n] = new QUANT_TYPE[64 * sizeof(QUANT_TYPE)];

        for (i = 0; i < 64; i++)
        {
            temp = get_bits_1(8);

            if (prec)
                temp = (temp << 8) + get_bits_1(8);

            quant[n][zzOrder[i]] = (QUANT_TYPE)temp;
        }

        i = 64 + 1;

        if (prec)
            i += 64;

        if (left < (uint)i)
            terminate(JPGD_BAD_DQT_LENGTH);

        left -= i;
    }
}

//------------------------------------------------------------------------------
// Read the start of frame (SOF) marker.
//------------------------------------------------------------------------------
void jpeg_decoder::read_sof_marker()
{
    int i;
    uint left;

    left = get_bits_1(16);

    if( get_bits_1(8) != 8 )
        terminate( JPGD_BAD_PRECISION );

    image_y_size = get_bits_1(16);

    if( (image_y_size < 1) || (image_y_size > JPGD_MAX_HEIGHT) )
        terminate( JPGD_BAD_HEIGHT );

    image_x_size = get_bits_1(16);

    if( (image_x_size < 1) || (image_x_size > JPGD_MAX_WIDTH) )
        terminate( JPGD_BAD_WIDTH );

    comps_in_frame = get_bits_1(8);

    if( comps_in_frame > JPGD_MAXCOMPONENTS )
        terminate( JPGD_TOO_MANY_COMPONENTS );

    if( left != (uint)(comps_in_frame * 3 + 8) )
        terminate( JPGD_BAD_SOF_LENGTH );

    for (i = 0; i < comps_in_frame; i++)
    {
        comp_ident[i]  = get_bits_1(8);
        comp_h_samp[i] = get_bits_1(4);
        comp_v_samp[i] = get_bits_1(4);
        comp_quant[i]  = get_bits_1(8);
    }
}

//------------------------------------------------------------------------------
// Used to skip unrecognized markers.
//------------------------------------------------------------------------------
void jpeg_decoder::skip_variable_marker()
{
    uint left;

    left = get_bits_1(16);

    if( left < 2 )
        terminate( JPGD_BAD_VARIABLE_MARKER );

    left -= 2;

    while( left )
    {
        get_bits_1(8);
        left--;
    }
}

void jpeg_decoder::read_dri_marker()
{
    if( get_bits_1(16) != 4 )
        terminate(JPGD_BAD_DRI_LENGTH);

    restart_interval = get_bits_1(16);
}

void jpeg_decoder::read_sos_marker()
{
    uint left;
    int i, ci, n, c, cc;

    left = get_bits_1(16);

    n = get_bits_1(8);

    comps_in_scan = n;

    left -= 3;

    if( (left != (uint)(n * 2 + 3)) || (n < 1) || (n > JPGD_MAXCOMPSINSCAN) )
        terminate( JPGD_BAD_SOS_LENGTH );

    for( i = 0; i < n; i++ )
    {
        cc = get_bits_1(8);
        c = get_bits_1(8);
        left -= 2;

        for( ci = 0; ci < comps_in_frame; ci++ )
            if( cc == comp_ident[ci] )
                break;

        if( ci >= comps_in_frame )
            terminate(JPGD_BAD_SOS_COMP_ID);

        comp_list[i]    = ci;
        comp_dc_tab[ci] = (c >> 4) & 15;
        comp_ac_tab[ci] = (c & 15) + (JPGD_MAXHUFFTABLES >> 1);
    }

    spectral_start  = get_bits_1(8);
    spectral_end    = get_bits_1(8);
    successive_high = get_bits_1(4);
    successive_low  = get_bits_1(4);

    spectral_start = 0;
    spectral_end = 63;

    left -= 3;

    while( left )
    {
        get_bits_1(8);
        left--;
    }
}

int jpeg_decoder::next_marker()
{
    uint c, bytes;

    bytes = 0;

    do
    {
        do
        {
            bytes++;
            c = get_bits_1( 8 );

        } while( c != 0xFF );

        do
        {
            c = get_bits_1( 8 );

        } while( c == 0xFF );

    } while( c == 0 );

    return c;
}

int jpeg_decoder::process_markers()
{
    int c;

    for ( ; ; )
    {
        c = next_marker();

        switch (c)
        {
            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF3:
            case M_SOF5:
            case M_SOF6:
            case M_SOF7:
            case M_SOF9:
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
            case M_SOI:
            case M_EOI:
            case M_SOS:
                return c;
            case M_DHT:
                read_dht_marker();
                break;
            case M_DAC: // Arithmetic encoding
                break;
            case M_DQT:
                read_dqt_marker();
                break;
            case M_DRI:
                read_dri_marker();
                break;
            case M_JPG:
            case M_RST0:
            case M_RST1:
            case M_RST2:
            case M_RST3:
            case M_RST4:
            case M_RST5:
            case M_RST6:
            case M_RST7:
            case M_TEM:
                break;
            default:
                skip_variable_marker();
                break;
        }
    }
}

void jpeg_decoder::locate_soi_marker()
{
    uint lastchar, thischar;
    ulong bytesleft;

    lastchar = get_bits_1( 8 );
    thischar = get_bits_1( 8 );

    if( ( lastchar == 0xFF ) && ( thischar == M_SOI ) )
        return;

    bytesleft = 512;

    for ( ; ; )
    {
        if( --bytesleft == 0 )
            terminate( JPGD_NOT_JPEG );

        lastchar = thischar;
        thischar = get_bits_1( 8 );

        if( ( lastchar == 0xFF ) && ( thischar == M_SOI ) )
            break;
    }

    thischar = ( bit_buf >> 8 ) & 0xFF;

    if( thischar != 0xFF )
        terminate( JPGD_NOT_JPEG );
}

void jpeg_decoder::locate_sof_marker()
{
    int c;

    locate_soi_marker();
    c = process_markers();

    switch (c)
    {
        case M_SOF0:  // Baseline DCT
        case M_SOF1:  // Extended sequential DCT
            read_sof_marker();
            break;
        case M_SOF9:  // Arithmetic coding
            terminate( JPGD_NO_ARITHMITIC_SUPPORT );
            break;
        default:
            terminate(JPGD_UNSUPPORTED_MARKER);
            break;
    }
}

int jpeg_decoder::locate_sos_marker()
{
    int c;

    c = process_markers();

    if( c == M_EOI )
        return 0;
    else if( c != M_SOS )
        terminate(JPGD_UNEXPECTED_MARKER);

    read_sos_marker();

    return 1;
}

void jpeg_decoder::init( Pjpeg_decoder_stream Pstream )
{
    error_code = 0;

    ready_flag = false;

    image_x_size = image_y_size = 0;

    this->Pstream = Pstream;

    stx_memset(huff_num, 0, sizeof(huff_num));
    stx_memset(huff_val, 0, sizeof(huff_val));
    stx_memset(quant, 0, sizeof(quant));

    scan_type = 0;

    comps_in_frame = 0;

    stx_memset(comp_h_samp, 0, sizeof(comp_h_samp));
    stx_memset(comp_v_samp, 0, sizeof(comp_v_samp));
    stx_memset(comp_quant, 0, sizeof(comp_quant));
    stx_memset(comp_ident, 0, sizeof(comp_ident));
    stx_memset(comp_h_blocks, 0, sizeof(comp_h_blocks));
    stx_memset(comp_v_blocks, 0, sizeof(comp_v_blocks));

    comps_in_scan = 0;
    stx_memset(comp_list, 0, sizeof(comp_list));
    stx_memset(comp_dc_tab, 0, sizeof(comp_dc_tab));
    stx_memset(comp_ac_tab, 0, sizeof(comp_ac_tab));

    spectral_start = 0;
    spectral_end = 0;
    successive_low = 0;
    successive_high = 0;

    max_mcu_x_size = 0;
    max_mcu_y_size = 0;

    blocks_per_mcu = 0;
    max_blocks_per_row = 0;
    mcus_per_row = 0;
    mcus_per_col = 0;

    stx_memset(mcu_org, 0, sizeof(mcu_org));

    total_lines_left = 0;
    mcu_lines_left = 0;

    real_dest_bytes_per_scan_line = 0;
    dest_bytes_per_scan_line = 0;
    dest_bytes_per_pixel = 0;

    //stx_memset(blocks, 0, sizeof(blocks));

    stx_memset(h, 0, sizeof(h));

    stx_memset(dc_coeffs, 0, sizeof(dc_coeffs));
    stx_memset(ac_coeffs, 0, sizeof(ac_coeffs));
    stx_memset(block_y_mcu, 0, sizeof(block_y_mcu));

    eob_run = 0;

    stx_memset(block_y_mcu, 0, sizeof(block_y_mcu));

    Pin_buf_ofs = in_buf;
    in_buf_left = 0;
    eof_flag = false;
    tem_flag = 0;

    stx_memset(padd_1, 0, sizeof(padd_1));
    stx_memset(in_buf, 0, sizeof(in_buf));
    stx_memset(padd_2, 0, sizeof(padd_2));

    restart_interval = 0;
    restarts_left    = 0;
    next_restart_num = 0;

    max_mcus_per_row = 0;
    max_blocks_per_mcu = 0;
    max_mcus_per_col = 0;

    stx_memset(component, 0, sizeof(component));
    stx_memset(last_dc_val, 0, sizeof(last_dc_val));
    stx_memset(dc_huff_seg, 0, sizeof(dc_huff_seg));
    stx_memset(ac_huff_seg, 0, sizeof(ac_huff_seg));

    total_bytes_read = 0;

    Pstream->attach();
    prep_in_buffer();

    bits_left = 16;
    bit_buf_64[0] = 0;
    bit_buf_64[1] = 0;

    get_bits_1(16);
    get_bits_1(16);
}


#define SCALEBITS 16
#define ONE_HALF ((long) 1 << (SCALEBITS-1))
#define FIX(x) ((long) ((x) * (1L<<SCALEBITS) + 0.5))

//------------------------------------------------------------------------------
// This method throws back into the stream any bytes that where read
// into the bit buffer during initial marker scanning.
//------------------------------------------------------------------------------
void jpeg_decoder::fix_in_buffer()
{
    if( bits_left == 16 )
        stuff_char( (uchar)( ( bit_buf >> 16 ) & 0xFF ) );

    if( bits_left >= 8 )
        stuff_char( (uchar)( (bit_buf >> 24 ) & 0xFF) );

    stuff_char( (uchar)( bit_buf & 0xFF ) );

    stuff_char( (uchar)( ( bit_buf >> 8 ) & 0xFF ) );

    bits_left = 16;
    get_bits_2( 16 );
    get_bits_2( 16 );
}

void jpeg_decoder::process_restart()
{
    int i = 0;
    int c = 0;

    for( i = 1536; i > 0; i-- )
        if( get_char() == 0xFF )
            break;

    if( i == 0 )
        terminate(JPGD_BAD_RESTART_MARKER);

    for( ; i > 0; i-- )
        if( ( c = get_char() ) != 0xFF )
            break;

    if( i == 0 )
        terminate(JPGD_BAD_RESTART_MARKER);

    if( c != ( next_restart_num + M_RST0 ) )
        terminate( JPGD_BAD_RESTART_MARKER );

    // Reset each component's DC prediction values.
    stx_memset( &last_dc_val, 0, comps_in_frame * sizeof(uint) );

    eob_run = 0;
    restarts_left = restart_interval;
    next_restart_num = (next_restart_num + 1) & 7;

    bits_left = 16;
    get_bits_2( 16 );
    get_bits_2( 16 );
}

void jpeg_decoder::find_eoi()
{
    bits_left = 16;
    get_bits_1( 16 );
    get_bits_1( 16 );

    process_markers();

    total_bytes_read -= in_buf_left;
}

//------------------------------------------------------------------------------
// Dumps DCT coefficients for single MCU row
//------------------------------------------------------------------------------
int jpeg_decoder::dump_DCT_data( HANDLE outFile )
{
    if( error_code || !ready_flag )
        return JPGD_FAILED;

    if( total_lines_left == 0 )
        return JPGD_DONE;

    int r, s, component_id;
    static __DWORD__ bytesWritten;
    
    static char BLOCK_DATA[4096];
    int curBlock = 0;
    
    stx_memset( BLOCK_DATA, 0, 4096 );

    for( int mcu_row = 0; mcu_row < mcus_per_col; mcu_row++ )
    {
        int row_block = 0;

        for( int mcu_row = 0; mcu_row < mcus_per_row; mcu_row++ )
        {
            for( int mcu_block = 0; mcu_block < blocks_per_mcu; mcu_block++ )
            {
                component_id = mcu_org[mcu_block];
                char *p = BLOCK_DATA + curBlock * 64;

                if( (s = huff_decode(h[comp_dc_tab[component_id]])) != 0 )
                {
                    r = get_bits_2(s);
                    s = HUFF_EXTEND(r, s);
                }

                last_dc_val[component_id] = (s += last_dc_val[component_id]);

                p[0] = (char)s;

                Phuff_tables_t Ph = h[comp_ac_tab[component_id]];

                int k;

                for( k = 1; k < 64; k++ )
                {
                    s = huff_decode(Ph);

                    r = s >> 4;
                    s &= 15;

                    if( s )
                    {
                        if( r )
                        {
                            k += r;
                        }

                        r = get_bits_2(s);
                        s = HUFF_EXTEND(r, s);

                        p[zzOrder[k]] = (char)s;
                    }
                    else
                    {
                        if( r == 15 )
                        {
                            k += 15;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                row_block++;
                curBlock++;
                
                if( curBlock == 64 )
                {
                    WriteFile( outFile, BLOCK_DATA, 4096, &bytesWritten, 0 );
                    stx_memset( BLOCK_DATA, 0, 4096 );
                    
                    curBlock = 0;
                }
            }
            
            restarts_left--;
        }
    }

    return JPGD_OKAY;
}

//------------------------------------------------------------------------------
// Creates the tables needed for efficient Huffman decoding.
//------------------------------------------------------------------------------
void jpeg_decoder::make_huff_table( int index, Phuff_tables_t hs)
{
    int p, i, l, si;
    uchar huffsize[257];
    uint huffcode[257];
    uint code;
    uint subtree;
    int code_size;
    int lastp;
    int nextfreeentry;
    int currententry;

    p = 0;

    for( l = 1; l <= 16; l++ )
    {
        for( i = 1; i <= huff_num[index][l]; i++ )
            huffsize[p++] = (uchar)l;
    }

    huffsize[p] = 0;

    lastp = p;

    code = 0;
    si = huffsize[0];
    p = 0;

    while( huffsize[p] )
    {
        while( huffsize[p] == si )
        {
            huffcode[p++] = code;
            code++;
        }

        code <<= 1;
        si++;
    }

    stx_memset( hs->look_up, 0, sizeof(hs->look_up) );
    stx_memset( hs->tree, 0, sizeof(hs->tree) );
    stx_memset( hs->code_size, 0, sizeof(hs->code_size) );

    nextfreeentry = -1;

    p = 0;

    while( p < lastp )
    {
        i = huff_val[index][p];
        code = huffcode[p];
        code_size = huffsize[p];

        hs->code_size[i] = (uchar)code_size;

        if(code_size <= 8 )
        {
            code <<= (8 - code_size);

            for( l = 1 << (8 - code_size); l > 0; l-- )
            {
                hs->look_up[code] = i;
                code++;
            }
        }
        else
        {
            subtree = ( code >> (code_size - 8) ) & 0xFF;

            currententry = hs->look_up[subtree];

            if( currententry == 0 )
            {
                hs->look_up[subtree] = currententry = nextfreeentry;
                nextfreeentry -= 2;
            }

            code <<= (16 - (code_size - 8));

            for( l = code_size; l > 9; l-- )
            {
                if( (code & 0x8000) == 0 )
                    currententry--;

                if( hs->tree[-currententry - 1] == 0 )
                {
                    hs->tree[-currententry - 1] = nextfreeentry;
                    currententry = nextfreeentry;
                    nextfreeentry -= 2;
                }
                else
                    currententry = hs->tree[-currententry - 1];

                code <<= 1;
            }

            if( (code & 0x8000) == 0 )
                currententry--;

            hs->tree[-currententry - 1] = i;
        }

        p++;
    }
}

//------------------------------------------------------------------------------
// Verifies the quantization tables needed for this scan are available.
//------------------------------------------------------------------------------
void jpeg_decoder::check_quant_tables(void)
{
    int i;

    for( i = 0; i < comps_in_scan; i++ )
        if( quant[comp_quant[comp_list[i]]] == 0 )
            terminate( JPGD_UNDEFINED_QUANT_TABLE );
}


//------------------------------------------------------------------------------
// Verifies that all the Huffman tables needed for this scan are available.
//------------------------------------------------------------------------------
void jpeg_decoder::check_huff_tables(void)
{
    int i;

    for (i = 0; i < comps_in_scan; i++)
    {
        if ((spectral_start == 0) && (huff_num[comp_dc_tab[comp_list[i]]] == 0))
            terminate(JPGD_UNDEFINED_HUFF_TABLE);

        if ((spectral_end > 0) && (huff_num[comp_ac_tab[comp_list[i]]] == 0))
            terminate(JPGD_UNDEFINED_HUFF_TABLE);
    }

    for (i = 0; i < JPGD_MAXHUFFTABLES; i++)
        if (huff_num[i])
        {
            if (!h[i])
                h[i] = new huff_tables_t[1];

            make_huff_table(i, h[i]);
        }

        for (i = 0; i < blocks_per_mcu; i++)
        {
            dc_huff_seg[i] = h[comp_dc_tab[mcu_org[i]]];
            ac_huff_seg[i] = h[comp_ac_tab[mcu_org[i]]];
            component[i]   = &last_dc_val[mcu_org[i]];
        }
}


//------------------------------------------------------------------------------
// Determines the component order inside each MCU.
//------------------------------------------------------------------------------
void jpeg_decoder::calc_mcu_block_order(void)
{
    int component_num, component_id;
    int max_h_samp = 0, max_v_samp = 0;

    for (component_id = 0; component_id < comps_in_frame; component_id++)
    {
        if (comp_h_samp[component_id] > max_h_samp)
            max_h_samp = comp_h_samp[component_id];

        if (comp_v_samp[component_id] > max_v_samp)
            max_v_samp = comp_v_samp[component_id];
    }

    for (component_id = 0; component_id < comps_in_frame; component_id++)
    {
        comp_h_blocks[component_id] = ((((image_x_size * comp_h_samp[component_id]) + (max_h_samp - 1)) / max_h_samp) + 7) / 8;
        comp_v_blocks[component_id] = ((((image_y_size * comp_v_samp[component_id]) + (max_v_samp - 1)) / max_v_samp) + 7) / 8;
    }

    if (comps_in_scan == 1)
    {
        mcus_per_row = comp_h_blocks[comp_list[0]];
        mcus_per_col = comp_v_blocks[comp_list[0]];
    }
    else
    {
        mcus_per_row = (((image_x_size + 7) / 8) + (max_h_samp - 1)) / max_h_samp;
        mcus_per_col = (((image_y_size + 7) / 8) + (max_v_samp - 1)) / max_v_samp;
    }

    if (comps_in_scan == 1)
    {
        mcu_org[0] = comp_list[0];

        blocks_per_mcu = 1;
    }
    else
    {
        blocks_per_mcu = 0;

        for (component_num = 0; component_num < comps_in_scan; component_num++)
        {
            int num_blocks;

            component_id = comp_list[component_num];

            num_blocks = comp_h_samp[component_id] * comp_v_samp[component_id];

            while (num_blocks--)
                mcu_org[blocks_per_mcu++] = component_id;
        }
    }
}

int jpeg_decoder::init_scan(void)
{
    if (!locate_sos_marker())
        return FALSE;

    calc_mcu_block_order();

    check_huff_tables();

    check_quant_tables();

    stx_memset(last_dc_val, 0, comps_in_frame * sizeof(uint));

    eob_run = 0;

    if (restart_interval)
    {
        restarts_left = restart_interval;
        next_restart_num = 0;
    }

    fix_in_buffer();

    return TRUE;
}

void jpeg_decoder::init_frame()
{
    if( comps_in_frame == 1 )
    {
        scan_type          = JPGD_GRAYSCALE;

        max_blocks_per_mcu = 1;

        max_mcu_x_size     = 8;
        max_mcu_y_size     = 8;
    }
    else if (comps_in_frame == 3)
    {
        if (    ((comp_h_samp[1] != 1) || (comp_v_samp[1] != 1)) ||
                ((comp_h_samp[2] != 1) || (comp_v_samp[2] != 1)) )
                    terminate( JPGD_UNSUPPORTED_SAMP_FACTORS );

        if( (comp_h_samp[0] == 1) && (comp_v_samp[0] == 1) )
        {
            scan_type          = JPGD_YH1V1;

            max_blocks_per_mcu = 3;

            max_mcu_x_size     = 8;
            max_mcu_y_size     = 8;
        }
        else if( (comp_h_samp[0] == 2) && (comp_v_samp[0] == 1) )
        {
            scan_type          = JPGD_YH2V1;

            max_blocks_per_mcu = 4;

            max_mcu_x_size     = 16;
            max_mcu_y_size     = 8;
        }
        else if( (comp_h_samp[0] == 1) && (comp_v_samp[0] == 2) )
        {
            scan_type          = JPGD_YH1V2;

            max_blocks_per_mcu = 4;

            max_mcu_x_size     = 8;
            max_mcu_y_size     = 16;
        }
        else if( (comp_h_samp[0] == 2) && (comp_v_samp[0] == 2) )
        {
            scan_type          = JPGD_YH2V2;

            max_blocks_per_mcu = 6;

            max_mcu_x_size     = 16;
            max_mcu_y_size     = 16;
        }
        else
            terminate( JPGD_UNSUPPORTED_SAMP_FACTORS );
    }
    else
        terminate( JPGD_UNSUPPORTED_COLORSPACE );

    max_mcus_per_row = ( image_x_size + (max_mcu_x_size - 1) ) / max_mcu_x_size;
    max_mcus_per_col = ( image_y_size + (max_mcu_y_size - 1) ) / max_mcu_y_size;

    if( scan_type == JPGD_GRAYSCALE )
        dest_bytes_per_pixel = 1;
    else
        dest_bytes_per_pixel = 4;

    dest_bytes_per_scan_line = ((image_x_size + 15) & 0xFFF0) * dest_bytes_per_pixel;

    real_dest_bytes_per_scan_line = (image_x_size * dest_bytes_per_pixel);

    max_blocks_per_row = max_mcus_per_row * max_blocks_per_mcu;

    total_lines_left = image_y_size;
    mcu_lines_left = 0;
}


void jpeg_decoder::decode_start()
{
    init_frame();
    init_scan();
}

//------------------------------------------------------------------------------
// Find the start of the JPEG file and reads enough data to determine
// its size, number of components, etc.
//------------------------------------------------------------------------------
void jpeg_decoder::decode_init(Pjpeg_decoder_stream Pstream)
{
    init(Pstream);

    locate_sof_marker();
}

jpeg_decoder::jpeg_decoder(Pjpeg_decoder_stream Pstream)
{
    decode_init( Pstream );
}

int jpeg_decoder::begin()
{
    if( ready_flag )
        return JPGD_OKAY;

    if( error_code )
        return JPGD_FAILED;

    decode_start();

    ready_flag = true;

    return JPGD_OKAY;
}

//------------------------------------------------------------------------------
// Completely destroys the decoder object. May be called at any time.
//------------------------------------------------------------------------------
jpeg_decoder::~jpeg_decoder()
{
    free_all_blocks();
}
