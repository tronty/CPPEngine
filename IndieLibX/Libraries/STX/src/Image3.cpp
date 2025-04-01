/*
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifdef _MSC_VER
//pragma comment(lib, "winmm.lib")
//pragma comment(lib, "WSock32.lib")
#endif

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/Matrix.h>
#include <STX/LuaUtil.h>
using namespace LuaUtil;
#include <tinyxml/tinyxml.h>

#if 0
#define STX_FNLN  printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#define STX_PRINT printf
#define LOG_FNLN
#define LOG_PRINT
#else
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

#if 0//def _WIN32
#include <Winsock2.h>
#endif

#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
#ifdef _WIN32
//pragma comment(lib, "SDL2.lib")
//pragma comment(lib, "SDLmain.lib")
#endif
#endif

#if 0//def _WIN32
//pragma comment(lib, "fmodex_vc.lib")
//pragma comment(lib, "DevIL-1.7.8.lib")
//pragma comment(lib, "ILU.lib")
//pragma comment(lib, "ILUT.lib")
#endif

#if defined(QT_BUILD) && !defined(__APPLE__)
#include <qmath.h>
#endif
#include <math.h>

#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#define LOG_FNLN_X LOG_FNLN
#define LOG_PRINT_X LOG_PRINT
#else
#define LOG_FNLN_X
#define LOG_PRINT_X
#endif

#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/Xlib.h>
Display* g_X11_display=0;
Window g_X11_window;
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)

typedef enum
{
    MW_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    MW_SCANCODE_A = 4,
    MW_SCANCODE_B = 5,
    MW_SCANCODE_C = 6,
    MW_SCANCODE_D = 7,
    MW_SCANCODE_E = 8,
    MW_SCANCODE_F = 9,
    MW_SCANCODE_G = 10,
    MW_SCANCODE_H = 11,
    MW_SCANCODE_I = 12,
    MW_SCANCODE_J = 13,
    MW_SCANCODE_K = 14,
    MW_SCANCODE_L = 15,
    MW_SCANCODE_M = 16,
    MW_SCANCODE_N = 17,
    MW_SCANCODE_O = 18,
    MW_SCANCODE_P = 19,
    MW_SCANCODE_Q = 20,
    MW_SCANCODE_R = 21,
    MW_SCANCODE_S = 22,
    MW_SCANCODE_T = 23,
    MW_SCANCODE_U = 24,
    MW_SCANCODE_V = 25,
    MW_SCANCODE_W = 26,
    MW_SCANCODE_X = 27,
    MW_SCANCODE_Y = 28,
    MW_SCANCODE_Z = 29,

    MW_SCANCODE_1 = 30,
    MW_SCANCODE_2 = 31,
    MW_SCANCODE_3 = 32,
    MW_SCANCODE_4 = 33,
    MW_SCANCODE_5 = 34,
    MW_SCANCODE_6 = 35,
    MW_SCANCODE_7 = 36,
    MW_SCANCODE_8 = 37,
    MW_SCANCODE_9 = 38,
    MW_SCANCODE_0 = 39,

    MW_SCANCODE_RETURN = 40,
    MW_SCANCODE_ESCAPE = 41,
    MW_SCANCODE_BACKSPACE = 42,
    MW_SCANCODE_TAB = 43,
    MW_SCANCODE_SPACE = 44,

    MW_SCANCODE_MINUS = 45,
    MW_SCANCODE_EQUALS = 46,
    MW_SCANCODE_LEFTBRACKET = 47,
    MW_SCANCODE_RIGHTBRACKET = 48,
    MW_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    MW_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate MW_SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    MW_SCANCODE_SEMICOLON = 51,
    MW_SCANCODE_APOSTROPHE = 52,
    MW_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    MW_SCANCODE_COMMA = 54,
    MW_SCANCODE_PERIOD = 55,
    MW_SCANCODE_SLASH = 56,

    MW_SCANCODE_CAPSLOCK = 57,

    MW_SCANCODE_F1 = 58,
    MW_SCANCODE_F2 = 59,
    MW_SCANCODE_F3 = 60,
    MW_SCANCODE_F4 = 61,
    MW_SCANCODE_F5 = 62,
    MW_SCANCODE_F6 = 63,
    MW_SCANCODE_F7 = 64,
    MW_SCANCODE_F8 = 65,
    MW_SCANCODE_F9 = 66,
    MW_SCANCODE_F10 = 67,
    MW_SCANCODE_F11 = 68,
    MW_SCANCODE_F12 = 69,

    MW_SCANCODE_PRINTSCREEN = 70,
    MW_SCANCODE_SCROLLLOCK = 71,
    MW_SCANCODE_PAUSE = 72,
    MW_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    MW_SCANCODE_HOME = 74,
    MW_SCANCODE_PAGEUP = 75,
    MW_SCANCODE_DELETE = 76,
    MW_SCANCODE_END = 77,
    MW_SCANCODE_PAGEDOWN = 78,
    MW_SCANCODE_RIGHT = 79,
    MW_SCANCODE_LEFT = 80,
    MW_SCANCODE_DOWN = 81,
    MW_SCANCODE_UP = 82,

    MW_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    MW_SCANCODE_KP_DIVIDE = 84,
    MW_SCANCODE_KP_MULTIPLY = 85,
    MW_SCANCODE_KP_MINUS = 86,
    MW_SCANCODE_KP_PLUS = 87,
    MW_SCANCODE_KP_ENTER = 88,
    MW_SCANCODE_KP_1 = 89,
    MW_SCANCODE_KP_2 = 90,
    MW_SCANCODE_KP_3 = 91,
    MW_SCANCODE_KP_4 = 92,
    MW_SCANCODE_KP_5 = 93,
    MW_SCANCODE_KP_6 = 94,
    MW_SCANCODE_KP_7 = 95,
    MW_SCANCODE_KP_8 = 96,
    MW_SCANCODE_KP_9 = 97,
    MW_SCANCODE_KP_0 = 98,
    MW_SCANCODE_KP_PERIOD = 99,

    MW_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    MW_SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
    MW_SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    MW_SCANCODE_KP_EQUALS = 103,
    MW_SCANCODE_F13 = 104,
    MW_SCANCODE_F14 = 105,
    MW_SCANCODE_F15 = 106,
    MW_SCANCODE_F16 = 107,
    MW_SCANCODE_F17 = 108,
    MW_SCANCODE_F18 = 109,
    MW_SCANCODE_F19 = 110,
    MW_SCANCODE_F20 = 111,
    MW_SCANCODE_F21 = 112,
    MW_SCANCODE_F22 = 113,
    MW_SCANCODE_F23 = 114,
    MW_SCANCODE_F24 = 115,
    MW_SCANCODE_EXECUTE = 116,
    MW_SCANCODE_HELP = 117,
    MW_SCANCODE_MENU = 118,
    MW_SCANCODE_SELECT = 119,
    MW_SCANCODE_STOP = 120,
    MW_SCANCODE_AGAIN = 121,   /**< redo */
    MW_SCANCODE_UNDO = 122,
    MW_SCANCODE_CUT = 123,
    MW_SCANCODE_COPY = 124,
    MW_SCANCODE_PASTE = 125,
    MW_SCANCODE_FIND = 126,
    MW_SCANCODE_MUTE = 127,
    MW_SCANCODE_VOLUMEUP = 128,
    MW_SCANCODE_VOLUMEDOWN = 129,
/* not sure whether there's a reason to enable these */
/*     MW_SCANCODE_LOCKINGCAPSLOCK = 130,  */
/*     MW_SCANCODE_LOCKINGNUMLOCK = 131, */
/*     MW_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    MW_SCANCODE_KP_COMMA = 133,
    MW_SCANCODE_KP_EQUALSAS400 = 134,

    MW_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    MW_SCANCODE_INTERNATIONAL2 = 136,
    MW_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    MW_SCANCODE_INTERNATIONAL4 = 138,
    MW_SCANCODE_INTERNATIONAL5 = 139,
    MW_SCANCODE_INTERNATIONAL6 = 140,
    MW_SCANCODE_INTERNATIONAL7 = 141,
    MW_SCANCODE_INTERNATIONAL8 = 142,
    MW_SCANCODE_INTERNATIONAL9 = 143,
    MW_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    MW_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    MW_SCANCODE_LANG3 = 146, /**< Katakana */
    MW_SCANCODE_LANG4 = 147, /**< Hiragana */
    MW_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    MW_SCANCODE_LANG6 = 149, /**< reserved */
    MW_SCANCODE_LANG7 = 150, /**< reserved */
    MW_SCANCODE_LANG8 = 151, /**< reserved */
    MW_SCANCODE_LANG9 = 152, /**< reserved */

    MW_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
    MW_SCANCODE_SYSREQ = 154,
    MW_SCANCODE_CANCEL = 155,
    MW_SCANCODE_CLEAR = 156,
    MW_SCANCODE_PRIOR = 157,
    MW_SCANCODE_RETURN2 = 158,
    MW_SCANCODE_SEPARATOR = 159,
    MW_SCANCODE_OUT = 160,
    MW_SCANCODE_OPER = 161,
    MW_SCANCODE_CLEARAGAIN = 162,
    MW_SCANCODE_CRSEL = 163,
    MW_SCANCODE_EXSEL = 164,

    MW_SCANCODE_KP_00 = 176,
    MW_SCANCODE_KP_000 = 177,
    MW_SCANCODE_THOUSANDSSEPARATOR = 178,
    MW_SCANCODE_DECIMALSEPARATOR = 179,
    MW_SCANCODE_CURRENCYUNIT = 180,
    MW_SCANCODE_CURRENCYSUBUNIT = 181,
    MW_SCANCODE_KP_LEFTPAREN = 182,
    MW_SCANCODE_KP_RIGHTPAREN = 183,
    MW_SCANCODE_KP_LEFTBRACE = 184,
    MW_SCANCODE_KP_RIGHTBRACE = 185,
    MW_SCANCODE_KP_TAB = 186,
    MW_SCANCODE_KP_BACKSPACE = 187,
    MW_SCANCODE_KP_A = 188,
    MW_SCANCODE_KP_B = 189,
    MW_SCANCODE_KP_C = 190,
    MW_SCANCODE_KP_D = 191,
    MW_SCANCODE_KP_E = 192,
    MW_SCANCODE_KP_F = 193,
    MW_SCANCODE_KP_XOR = 194,
    MW_SCANCODE_KP_POWER = 195,
    MW_SCANCODE_KP_PERCENT = 196,
    MW_SCANCODE_KP_LESS = 197,
    MW_SCANCODE_KP_GREATER = 198,
    MW_SCANCODE_KP_AMPERSAND = 199,
    MW_SCANCODE_KP_DBLAMPERSAND = 200,
    MW_SCANCODE_KP_VERTICALBAR = 201,
    MW_SCANCODE_KP_DBLVERTICALBAR = 202,
    MW_SCANCODE_KP_COLON = 203,
    MW_SCANCODE_KP_HASH = 204,
    MW_SCANCODE_KP_SPACE = 205,
    MW_SCANCODE_KP_AT = 206,
    MW_SCANCODE_KP_EXCLAM = 207,
    MW_SCANCODE_KP_MEMSTORE = 208,
    MW_SCANCODE_KP_MEMRECALL = 209,
    MW_SCANCODE_KP_MEMCLEAR = 210,
    MW_SCANCODE_KP_MEMADD = 211,
    MW_SCANCODE_KP_MEMSUBTRACT = 212,
    MW_SCANCODE_KP_MEMMULTIPLY = 213,
    MW_SCANCODE_KP_MEMDIVIDE = 214,
    MW_SCANCODE_KP_PLUSMINUS = 215,
    MW_SCANCODE_KP_CLEAR = 216,
    MW_SCANCODE_KP_CLEARENTRY = 217,
    MW_SCANCODE_KP_BINARY = 218,
    MW_SCANCODE_KP_OCTAL = 219,
    MW_SCANCODE_KP_DECIMAL = 220,
    MW_SCANCODE_KP_HEXADECIMAL = 221,

    MW_SCANCODE_LCTRL = 224,
    MW_SCANCODE_LSHIFT = 225,
    MW_SCANCODE_LALT = 226, /**< alt, option */
    MW_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    MW_SCANCODE_RCTRL = 228,
    MW_SCANCODE_RSHIFT = 229,
    MW_SCANCODE_RALT = 230, /**< alt gr, option */
    MW_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    MW_SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    MW_SCANCODE_AUDIONEXT = 258,
    MW_SCANCODE_AUDIOPREV = 259,
    MW_SCANCODE_AUDIOSTOP = 260,
    MW_SCANCODE_AUDIOPLAY = 261,
    MW_SCANCODE_AUDIOMUTE = 262,
    MW_SCANCODE_MEDIASELECT = 263,
    MW_SCANCODE_WWW = 264,
    MW_SCANCODE_MAIL = 265,
    MW_SCANCODE_CALCULATOR = 266,
    MW_SCANCODE_COMPUTER = 267,
    MW_SCANCODE_AC_SEARCH = 268,
    MW_SCANCODE_AC_HOME = 269,
    MW_SCANCODE_AC_BACK = 270,
    MW_SCANCODE_AC_FORWARD = 271,
    MW_SCANCODE_AC_STOP = 272,
    MW_SCANCODE_AC_REFRESH = 273,
    MW_SCANCODE_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    MW_SCANCODE_BRIGHTNESSDOWN = 275,
    MW_SCANCODE_BRIGHTNESSUP = 276,
    MW_SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    MW_SCANCODE_KBDILLUMTOGGLE = 278,
    MW_SCANCODE_KBDILLUMDOWN = 279,
    MW_SCANCODE_KBDILLUMUP = 280,
    MW_SCANCODE_EJECT = 281,
    MW_SCANCODE_SLEEP = 282,

    MW_SCANCODE_APP1 = 283,
    MW_SCANCODE_APP2 = 284,

    /* @} *//* Walther keys */

    /* Add any other keys here. */

    SDL_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} SDL_Scancode;

static const SDL_Scancode windows_scancode_table[] = 
{ 
	/*	0						1							2							3							4						5							6							7 */
	/*	8						9							A							B							C						D							E							F */
	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_ESCAPE,		MW_SCANCODE_1,				MW_SCANCODE_2,				MW_SCANCODE_3,			MW_SCANCODE_4,				MW_SCANCODE_5,				MW_SCANCODE_6,			/* 0 */
	MW_SCANCODE_7,				MW_SCANCODE_8,				MW_SCANCODE_9,				MW_SCANCODE_0,				MW_SCANCODE_MINUS,		MW_SCANCODE_EQUALS,		MW_SCANCODE_BACKSPACE,		MW_SCANCODE_TAB,		/* 0 */

	MW_SCANCODE_Q,				MW_SCANCODE_W,				MW_SCANCODE_E,				MW_SCANCODE_R,				MW_SCANCODE_T,			MW_SCANCODE_Y,				MW_SCANCODE_U,				MW_SCANCODE_I,			/* 1 */
	MW_SCANCODE_O,				MW_SCANCODE_P,				MW_SCANCODE_LEFTBRACKET,	MW_SCANCODE_RIGHTBRACKET,	MW_SCANCODE_RETURN,	MW_SCANCODE_LCTRL,			MW_SCANCODE_A,				MW_SCANCODE_S,			/* 1 */

	MW_SCANCODE_D,				MW_SCANCODE_F,				MW_SCANCODE_G,				MW_SCANCODE_H,				MW_SCANCODE_J,			MW_SCANCODE_K,				MW_SCANCODE_L,				MW_SCANCODE_SEMICOLON,	/* 2 */
	MW_SCANCODE_APOSTROPHE,	MW_SCANCODE_GRAVE,			MW_SCANCODE_LSHIFT,		MW_SCANCODE_BACKSLASH,		MW_SCANCODE_Z,			MW_SCANCODE_X,				MW_SCANCODE_C,				MW_SCANCODE_V,			/* 2 */

	MW_SCANCODE_B,				MW_SCANCODE_N,				MW_SCANCODE_M,				MW_SCANCODE_COMMA,			MW_SCANCODE_PERIOD,	MW_SCANCODE_SLASH,			MW_SCANCODE_RSHIFT,		MW_SCANCODE_PRINTSCREEN,/* 3 */
	MW_SCANCODE_LALT,			MW_SCANCODE_SPACE,			MW_SCANCODE_CAPSLOCK,		MW_SCANCODE_F1,			MW_SCANCODE_F2,		MW_SCANCODE_F3,			MW_SCANCODE_F4,			MW_SCANCODE_F5,		/* 3 */

	MW_SCANCODE_F6,			MW_SCANCODE_F7,			MW_SCANCODE_F8,			MW_SCANCODE_F9,			MW_SCANCODE_F10,		MW_SCANCODE_NUMLOCKCLEAR,	MW_SCANCODE_SCROLLLOCK,	MW_SCANCODE_HOME,		/* 4 */
	MW_SCANCODE_UP,			MW_SCANCODE_PAGEUP,		MW_SCANCODE_KP_MINUS,		MW_SCANCODE_LEFT,			MW_SCANCODE_KP_5,		MW_SCANCODE_RIGHT,			MW_SCANCODE_KP_PLUS,		MW_SCANCODE_END,		/* 4 */

	MW_SCANCODE_DOWN,			MW_SCANCODE_PAGEDOWN,		MW_SCANCODE_INSERT,		MW_SCANCODE_DELETE,		MW_SCANCODE_UNKNOWN,	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_NONUSBACKSLASH,MW_SCANCODE_F11,		/* 5 */
	MW_SCANCODE_F12,			MW_SCANCODE_PAUSE,			MW_SCANCODE_UNKNOWN,		MW_SCANCODE_LGUI,			MW_SCANCODE_RGUI,		MW_SCANCODE_APPLICATION,	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,	/* 5 */

	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_F13,		MW_SCANCODE_F14,			MW_SCANCODE_F15,			MW_SCANCODE_F16,		/* 6 */
	MW_SCANCODE_F17,			MW_SCANCODE_F18,			MW_SCANCODE_F19,			MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,	/* 6 */
	
	MW_SCANCODE_INTERNATIONAL2,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_INTERNATIONAL1,		MW_SCANCODE_UNKNOWN,	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN,	/* 7 */
	MW_SCANCODE_UNKNOWN,		MW_SCANCODE_INTERNATIONAL4,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_INTERNATIONAL5,		MW_SCANCODE_UNKNOWN,	MW_SCANCODE_INTERNATIONAL3,		MW_SCANCODE_UNKNOWN,		MW_SCANCODE_UNKNOWN	/* 7 */
};

WNDCLASSEX winClass;

static unsigned int
MWScanCodeToSDLScanCode(LPARAM lParam, WPARAM wParam)
{
    unsigned int code;
    char bIsExtended;
    int nScanCode = (lParam >> 16) & 0xFF;

    /* 0x45 here to work around both pause and numlock sharing the same scancode, so use the VK key to tell them apart */
    if (nScanCode == 0 || nScanCode == 0x45) {
        switch(wParam) {
        case VK_CLEAR: return STX_KEY_CLEAR;
        //case VK_MODECHANGE: return STX_KEY_MODE;
        //case VK_SELECT: return STX_KEY_SELECT;
        //case VK_EXECUTE: return STX_KEY_EXECUTE;
        case VK_HELP: return STX_KEY_HELP;
        case VK_PAUSE: return STX_KEY_PAUSE;
        //case VK_NUMLOCK: return STX_KEY_NUMLOCKCLEAR;

        case VK_F13: return STX_KEY_F13;
        case VK_F14: return STX_KEY_F14;
        case VK_F15: return STX_KEY_F15;
        /* case VK_F16: return STX_KEY_F16;
        case VK_F17: return STX_KEY_F17;
        case VK_F18: return STX_KEY_F18;
        case VK_F19: return STX_KEY_F19;
        case VK_F20: return STX_KEY_F20;
        case VK_F21: return STX_KEY_F21;
        case VK_F22: return STX_KEY_F22;
        case VK_F23: return STX_KEY_F23;
        case VK_F24: return STX_KEY_F24;

        case VK_OEM_NEC_EQUAL: return STX_KEY_KP_EQUALS;
        case VK_BROWSER_BACK: return STX_KEY_AC_BACK;
        case VK_BROWSER_FORWARD: return STX_KEY_AC_FORWARD;
        case VK_BROWSER_REFRESH: return STX_KEY_AC_REFRESH;
        case VK_BROWSER_STOP: return STX_KEY_AC_STOP;
        case VK_BROWSER_SEARCH: return STX_KEY_AC_SEARCH;
        case VK_BROWSER_FAVORITES: return STX_KEY_AC_BOOKMARKS;
        case VK_BROWSER_HOME: return STX_KEY_AC_HOME;
        case VK_VOLUME_MUTE: return STX_KEY_AUDIOMUTE;
        case VK_VOLUME_DOWN: return STX_KEY_VOLUMEDOWN;
        case VK_VOLUME_UP: return STX_KEY_VOLUMEUP;

        case VK_MEDIA_NEXT_TRACK: return STX_KEY_AUDIONEXT;
        case VK_MEDIA_PREV_TRACK: return STX_KEY_AUDIOPREV;
        case VK_MEDIA_STOP: return STX_KEY_AUDIOSTOP;
        case VK_MEDIA_PLAY_PAUSE: return STX_KEY_AUDIOPLAY;
        case VK_LAUNCH_MAIL: return STX_KEY_MAIL;
        case VK_LAUNCH_MEDIA_SELECT: return STX_KEY_MEDIASELECT;

        case VK_OEM_102: return STX_KEY_NONUSBACKSLASH;

        case VK_ATTN: return STX_KEY_SYSREQ;
        case VK_CRSEL: return STX_KEY_CRSEL;
        case VK_EXSEL: return STX_KEY_EXSEL;
        case VK_OEM_CLEAR: return STX_KEY_CLEAR;

        case VK_LAUNCH_APP1: return STX_KEY_APP1;
        case VK_LAUNCH_APP2: return STX_KEY_APP2;
*/
        default: return STX_KEY_UNKNOWN;
        }
    }

    if (nScanCode > 127)
        return STX_KEY_UNKNOWN;

    code = windows_scancode_table[nScanCode];

	/*
    bIsExtended = (lParam & (1 << 24)) != 0;
    if (!bIsExtended) {
        switch (code) {
        case MW_SCANCODE_HOME:
            return STX_KEY_KP_7;
        case MW_SCANCODE_UP:
            return STX_KEY_KP_8;
        case MW_SCANCODE_PAGEUP:
            return STX_KEY_KP_9;
        case MW_SCANCODE_LEFT:
            return STX_KEY_KP_4;
        case MW_SCANCODE_RIGHT:
            return STX_KEY_KP_6;
        case MW_SCANCODE_END:
            return STX_KEY_KP_1;
        case MW_SCANCODE_DOWN:
            return STX_KEY_KP_2;
        case MW_SCANCODE_PAGEDOWN:
            return STX_KEY_KP_3;
        case MW_SCANCODE_INSERT:
            return STX_KEY_KP_0;
        case MW_SCANCODE_DELETE:
            return STX_KEY_KP_PERIOD;
        case MW_SCANCODE_PRINTSCREEN:
            return STX_KEY_KP_MULTIPLY;
        default:
            break;
        }
    } else {
        switch (code) {
        case MW_SCANCODE_RETURN:
            return STX_KEY_KP_ENTER;
        case MW_SCANCODE_LALT:
            return STX_KEY_RALT;
        case MW_SCANCODE_LCTRL:
            return STX_KEY_RCTRL;
        case MW_SCANCODE_SLASH:
            return STX_KEY_KP_DIVIDE;
        case MW_SCANCODE_CAPSLOCK:
            return STX_KEY_KP_PLUS;
        default:
            break;
        }
    } */

    return code;
}
LRESULT CALLBACK WindowProc( HWND   m_hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	//STX_Service::GetInputInstance()->clear();
    	if(msg==MK_LBUTTON)
		STX_Service::GetInputInstance()->SetMouseLB(1);
    	else if(msg==MK_MBUTTON)
		STX_Service::GetInputInstance()->SetMouseMB(1);
    	else if(msg==MK_RBUTTON)
		STX_Service::GetInputInstance()->SetMouseRB(1);
    	else if(msg==WM_MOUSEMOVE)
        {
		STX_Service::GetInputInstance()->SetMouseX(LOWORD(lParam));
		STX_Service::GetInputInstance()->SetMouseY(HIWORD(lParam));
	}
    	else if((msg==WM_KEYDOWN) || (msg==WM_SYSKEYDOWN))
        {
            unsigned int _keycode=MWScanCodeToSDLScanCode(lParam, wParam);
	    STX_Service::GetInputInstance()->SetKey(_keycode);
	}
    	else if((msg==WM_SYSKEYUP) || (msg==WM_KEYUP))
        {
            unsigned int _keycode=MWScanCodeToSDLScanCode(lParam, wParam);
	    STX_Service::GetInputInstance()->SetKey(_keycode);
        }
        else if((wParam==VK_ESCAPE) || (msg==WM_CLOSE) || (msg==WM_DESTROY))
            	PostQuitMessage(0);
        else
		return DefWindowProc( m_hWnd, msg, wParam, lParam );
	return 0;
}

struct MWWindow : public IWindow
{
int MW_HelperWindowCreate(const char* atitle, unsigned int awidth, unsigned int aheight)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HINSTANCE hPrevInstance=0;
	LPSTR     lpCmdLine=0;
	int       nCmdShow=0;
 
	MSG        uMsg;

    stx_memset(&uMsg,0,sizeof(uMsg));
    
	winClass.lpszClassName = "MW_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = 0;//LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = 0;//LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = 0;//(HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	m_hWnd = CreateWindowEx( NULL, "MW_WINDOWS_CLASS", 
                             atitle,
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, awidth, aheight, NULL, NULL, hInstance, NULL );
	if( m_hWnd == NULL )
		return E_FAIL;

    ShowWindow( m_hWnd, nCmdShow );
    UpdateWindow( m_hWnd );

    return 0;
}
	MWWindow():IWindow(){}
	virtual ~MWWindow()
	{
		UnregisterClass( "MW_WINDOWS_CLASS", winClass.hInstance );
	}
	virtual void ShowCursor (bool pEnable){}
	virtual void SetCaption(const char * buf){}
	const char* GetCaption(){return title.c_str();}
	virtual void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)
	{
		width=awidth;
		height=aheight;
		bpp=abpp;
		MW_HelperWindowCreate(atitle, awidth, aheight);
	}
	virtual void GetWindowPosition(int* x, int* y, unsigned int* z, unsigned int* w){}
	virtual void ReSize(int awidth, int aheight, int abpp){}
};
struct MWInput : public DummyInput
{
	unsigned int m_KeyPressed;
	unsigned int m_KeyReleased;
	unsigned int m_KeyID;
	char m_KeyChar;
	unsigned int m_KeyCode;
	MWInput():DummyInput(){}
	virtual ~MWInput(){}
	virtual bool Update()
	{
		//MW_keycode=0;
	}
	virtual bool OnKeyPress				(eKey pKey){return false;}
	virtual bool OnKeyRelease			(eKey pKey){return false;}
	virtual bool IsKeyPressed			(eKey pKey){return false;}
	virtual bool IsKeyPressed			(eKey pKey, unsigned int pTime){return false;}
	virtual bool IsKeyPressed(void){return false;}
	virtual bool IsKeyReleased(void){return false;}
	virtual int getKeyID(){return 0;}
	virtual char getKeyChar(){return '\0';}
	virtual Sint32 getKeyCode(){return 0;}
};
#endif

void IWindow::ReadConfigurationFile(std::string& atitle)
{
	unsigned int windowpositionx;
	unsigned int windowpositiony;
		width=800;
		height=600;
		windowpositionx=0;
		windowpositiony=0;

	#if 0
	char path[MAX_PATH];
	stx_getcwd(path);
	LOG_FNLN;
	LOG_PRINT("path=%s\n", path);

	if(//(	stx_strcmp(path,"~/IndieLibX/scripting")==0) &&
		stx_fileExists("./IndieLib.pyc") &&
		stx_fileExists("./_IndieLib.so") &&
		stx_fileExists("./IndieLib_wrap.o") &&
		stx_fileExists("./IndieLib_wrap.cxx") &&
		stx_fileExists("./IndieLib.py"))
			return;
	#endif
#define USEXML 1
#ifdef USEXML
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
	//LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* _Image3_BodyElement = BodyHandle.Element();
	std::string rendererstr = "GLGLSL_1_1";
	if(_Image3_BodyElement) {
		TiXmlElement* e = BodyHandle.FirstChild("Renderer").Element();
		if(e)
			rendererstr=e->Attribute("Value");
	}
#else
#if 1
	LuaScript script;
	bool r=script.Load("/Framework3/Framework3.lua");
	if(r)
		return;
#else
	LuaScript script("/Framework3/Framework3.lua");
#endif
	std::string rendererstr = script.get<std::string>("renderer");
#endif
	tRenderer renderer = eGLGLSL_1_1;
		
		if(rendererstr=="GLCG_1_1")
			renderer = eGLCG_1_1;
		else if(rendererstr=="GLGLSL_1_1")
			renderer = eGLGLSL_1_1;
		else if(rendererstr=="GLES2")
			renderer = eGLES2;
		else if(rendererstr=="D3D9")
			renderer = eD3D9;
		else if(rendererstr=="D3D10")
			renderer = eD3D10;
		else if(rendererstr=="D3D11")
			renderer = eD3D11;
		else if(rendererstr=="D3D12")
			renderer = eD3D12;
		else if(rendererstr=="VULKAN")
			renderer = eVULKAN;
		else if(rendererstr=="METAL")
			renderer = eMETAL;
#ifndef USEXML
		width = script.get<int>("window.size.x");
		height = script.get<int>("window.size.y");
		windowpositionx = script.get<int>("window.position.x");
		windowpositiony = script.get<int>("window.position.y");
	if(atitle=="Clock_Dial")
	{
		width = script.get<int>("Clock_Dial.window.size.x");
		height = script.get<int>("Clock_Dial.window.size.y");
	}
	else if(atitle=="Digital_Clock")
	{
		width = script.get<int>("Digital_Clock.window.size.x");
		height = script.get<int>("Digital_Clock.window.size.y");
	}
	else
	{
		char buf[256];
		stx_snprintf(buf, 256, "%s.window.size.x", atitle.c_str());
		if(script.lua_gettostack(buf))
			width = script.get<int>(buf);
		stx_snprintf(buf, 256, "%s.window.size.y", atitle.c_str());
		if(script.lua_gettostack(buf))
			height = script.get<int>(buf);
	}
#else	
	if(_Image3_BodyElement){
	TiXmlHandle windowsHandle = BodyHandle.FirstChild("windows");
	int i=1;
	TiXmlHandle windowHandle = windowsHandle.FirstChild("window");
	TiXmlElement* windowElement = windowHandle.Element();
	while ( windowElement )
	{
		std:string title_=windowHandle.FirstChild("title").Element()->GetText();
		if(atitle==title_)
		{
			width  = atoi(windowHandle.FirstChild("size").Element()->Attribute("x"));
			height = atoi(windowHandle.FirstChild("size").Element()->Attribute("y"));
		}
		else if(title_=="Default")
		{
			width  = atoi(windowHandle.FirstChild("size").Element()->Attribute("x"));
			height = atoi(windowHandle.FirstChild("size").Element()->Attribute("y"));
			windowpositionx = atoi(windowHandle.FirstChild("position").Element()->Attribute("x"));
			windowpositiony = atoi(windowHandle.FirstChild("position").Element()->Attribute("y"));
		}
		windowHandle = windowsHandle.Child( "window", i++ );
		windowElement = windowHandle.Element();
	}}
#endif
}

struct FormatString {
	FORMAT format;
    const char *string;
};
static const FormatString formatStrings[] = {
	{ (FORMAT)FORMAT_NONE,	 "NONE"  },
	{ FORMAT_R8,	 "R8"	 },
	{ FORMAT_RG8,	 "RG8"	 },
	{ FORMAT_RGB8,	 "RGB8"  },
	{ FORMAT_RGBA8,  "RGBA8" },
	{ FORMAT_R16,	 "R16"	 },
	{ FORMAT_RG16,	 "RG16"  },
	{ FORMAT_RGB16,  "RGB16" },
	{ FORMAT_RGBA16, "RGBA16"},
	{ FORMAT_R16f,	 "R16F"    },
	{ FORMAT_RG16f,  "RG16F"   },
	{ FORMAT_RGB16f, "RGB16F"  },
	{ FORMAT_RGBA16f,"RGBA16F" },
	{ FORMAT_R32f,	 "R32F"    },
	{ FORMAT_RG32f,  "RG32F"   },
	{ FORMAT_RGB32f, "RGB32F"  },
	{ FORMAT_RGBA32f,"RGBA32F" },
	{ FORMAT_RGBE8,  "RGBE8"   },
	{ FORMAT_RGB565, "RGB565"  },
	{ FORMAT_RGBA4,  "RGBA4"   },
	{ FORMAT_RGB10A2,"RGB10A2" },
	{ FORMAT_DXT1,	 "DXT1"  },
	{ FORMAT_DXT3,	 "DXT3"  },
	{ FORMAT_DXT5,	 "DXT5"  },
	{ FORMAT_ATI1N,  "ATI1N" },
	{ FORMAT_ATI2N,  "ATI2N" },
};

const char *stx_getFormatString(const FORMAT format){
	for (unsigned int i = 0; i < elementsOf(formatStrings); i++){
		if (format == formatStrings[i].format) return formatStrings[i].string;
	}
	return 0;
}

Uint32 stx_GetTicks(STX_TIMER* _Timer)
{

	return STX_Service::GetTimerInstance()->stx_GetTicks(0);
}
void stx_Delay(Uint32 ms,STX_TIMER* _Timer)
{

	return STX_Service::GetTimerInstance()->stx_Delay(ms,0);
}

//};


//namespace STX

#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
Uint32 STX_SDL_TIMER::stx_GetTicks(STX_TIMER* _Timer)
{
	return SDL_GetTicks();
}
void STX_SDL_TIMER::stx_Delay(Uint32 ms,STX_TIMER* _Timer)
{
	SDL_Delay(ms);
}
#endif
const char* stx_c_str(
					  std::vector<char>::const_iterator first,
    std::vector<char>::const_iterator last)
{
	if(!(last>first))return 0;
	int n=int(last-first);
	std::vector<char> buf(n+1);
	unsigned int i=0;
	for(i=0;i<n;i++)
		*(&buf[0]+i)=*(first+i);
	*(&buf[0]+i)='\0';
	return stx_strdup(&buf[0]);
}

#if defined(_WIN32) && defined(_DEBUG) && !defined(WIN_PHONE_APP) && !defined(WIN8_APP) && !defined(__MINGW32__) && !defined(__MINGW64__)
unsigned int Heap::GetCombinedSizesOfProcessHeaps(void)
    {
    return getCombinedSizesOfProcessHeaps();
    }
bool Heap::ValidateAllHeaps(void)
    {
       const HANDLE default_heap = ::GetProcessHeap ();
       const HANDLE crt_heap = (HANDLE) _get_heap_handle ();
	    if (!ValidateHeap(default_heap))
		{
			//LOG_PRINT_X("default_heap corrupted\n");
		return false;
		}
	    if (!ValidateHeap(crt_heap))
		{
			//LOG_PRINT_X("crt_heap corrupted\n");
		return false;
		}
    HANDLE hHeaps[250];
    __DWORD__ numHeaps = GetProcessHeaps(250,hHeaps);//(HANDLE)_get_heap_handle()
	       //LOG_PRINT_X ("The process has %d heaps.\n", numHeaps);
	unsigned int i;
    if (numHeaps <= 250)
	{
	for (i=0; i<numHeaps; i++)
	    {
	    if (!ValidateHeap(hHeaps[i]))
		{
		return false;
		}
	    }
	}
    else
	{
	// Too many heaps!
	return false;
	}
    return true;
    }
bool Heap::ValidateHeap(HANDLE heap)
    {
    if (HeapValidate(heap,0,0) == 0)//HeapValidate((HANDLE)_get_heap_handle(), 0, 0)
	{
	return false;
	}
    else
	{
	return true;
	}
    }
unsigned int Heap::getCombinedSizesOfProcessHeaps(void)
    {
    HANDLE hHeaps[250];
    unsigned int sz = 0;
    __DWORD__ numHeaps = GetProcessHeaps(250,hHeaps);
    unsigned int i;
    if (numHeaps <= 250)
	{
	for (i=0; i<numHeaps; i++)
	    {
	    sz += GetHeapSize(hHeaps[i]);
	    }
	}
    else
	{
	// Too many heaps!
	return 0;
	}
    return sz;
    }
unsigned int Heap::GetHeapSize(HANDLE heap)
    {
    unsigned int sz = 0;
    HeapLock(heap);
    PROCESS_HEAP_ENTRY entry;
    stx_memset(&entry,'\0',sizeof entry);
    while (HeapWalk(heap,&entry))
	{
	if (entry.wFlags == PROCESS_HEAP_ENTRY_BUSY)
	    {
	    sz += entry.cbData;
	    }
	}
    HeapUnlock(heap);
    return sz;
    }
Heap::Heap()
{
}
Heap::~Heap()
{
}
void heapdump( void )
{
_HEAPINFO hinfo;
int heapstatus;
hinfo._pentry = 0;
while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
{ LOG_PRINT_X( "%6s block at %Fp of size %4.4X\n",
( hinfo._useflag == _USEDENTRY ? "USED" : "FREE" ),
hinfo._pentry, hinfo._size );
}
switch( heapstatus )
{
case _HEAPEMPTY:
LOG_PRINT_X( "OK - empty heap\n" );
break;
case _HEAPEND:


LOG_PRINT_X( "OK - end of heap\n" );
break;
case _HEAPBADPTR:
LOG_PRINT_X( "ERROR - bad pointer to heap\n" );
break;
case _HEAPBADBEGIN:
//LOG_PRINT_X( "ERROR - bad start of heap\n" );
break;
case _HEAPBADNODE:
LOG_PRINT_X( "ERROR - bad node in heap\n" );
break;
}
}
#endif
std::string replaceAll( const std::string& s, const std::string& f, const std::string& r )
{
if (
	s.empty() ||
	f.empty() ||
	f == r ||
	s.find(f) == std::string::npos
	)
{
return s;
}
ostringstream build_it;
size_t i = 0;
for ( size_t pos; ( pos = s.find( f, i ) ) != string::npos; ) {
build_it.write( &s[i], pos - i );
build_it << r;
i = pos + f.size();
}
if ( i != s.size() ) {
build_it.write( &s[i], s.size() - i );
}
return build_it.str();
}

//namespace STX
//{
#if defined(SDL_1_2_15_BUILD)
class SDL1_2Window : public IWindow
{
protected:
	int nFlags;
	SDL_Surface* screen;
public:
	void ScreenShot();//SDL_Renderer *renderer);
	SDL1_2Window() : IWindow(){}//{LOG_PRINT_X("SDL1_2Window\n");}
	virtual ~SDL1_2Window(){}
	virtual void SetCaption(const char * buf);
	virtual void ShowCursor (bool pEnable);
	virtual void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle);
	virtual void ReSize(int awidth, int aheight, int abpp);

	virtual void GetWindowPosition(int* ax, int* ay, unsigned int* az, unsigned int* aw)
	{
		UpdateWindowArgs();
		*ax=x;
		*ay=y;
		*az=*ax+width;
		*aw=*ay+height;
	}

	virtual void HotKey_ToggleFullScreen(void)
{
	//SDL_Surface *screen;
	screen = SDL_GetVideoSurface();
	if ( SDL_WM_ToggleFullScreen(screen) ) {
		LOG_PRINT_X("Toggled fullscreen mode - now %s\n",
		    (screen->flags&SDL_FULLSCREEN) ? "fullscreen" : "windowed");
	} else {
		LOG_PRINT_X("Unable to toggle fullscreen mode\n");
		//???video_flags ^= SDL_FULLSCREEN;
		//???SetVideoMode(screen->w, screen->h);
	}
}

	virtual void HotKey_ToggleGrab(void)
{
	SDL_GrabMode mode;
	LOG_PRINT_X("Ctrl-G: toggling input grab!\n");
	mode = SDL_WM_GrabInput(SDL_GRAB_QUERY);
	if ( mode == SDL_GRAB_ON ) {
		LOG_PRINT_X("Grab was on\n");
	} else {
		LOG_PRINT_X("Grab was off\n");
	}
	mode = SDL_WM_GrabInput(mode ? SDL_GRAB_OFF : SDL_GRAB_ON);
	if ( mode == SDL_GRAB_ON ) {
		LOG_PRINT_X("Grab is now on\n");
	} else {
		LOG_PRINT_X("Grab is now off\n");
	}
}

	virtual void HotKey_Iconify(void)
{
	LOG_PRINT_X("Ctrl-Z: iconifying window!\n");
	SDL_WM_IconifyWindow();
}
};
#endif




#if defined(SDL_2_0_5_BUILD)
class SDL2_0Window : public IWindow
{
public:
	SDL2_0Window() : IWindow()
	{LOG_PRINT_X("SDL2_0Window\n");}
	virtual ~SDL2_0Window(){}
	virtual void SetCaption(const char * buf);

	virtual void GetWindowPosition(int* ax, int* ay, unsigned int* az, unsigned int* aw)
	{
		SDL_GetWindowPosition(STX_Service::GetSDLWindow(), ax, ay);
		*az=*ax+width;
		*aw=*ay+height;
	}
	virtual void ReSize(int awidth, int aheight, int abpp);
	virtual void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle);
	virtual void ShowCursor (bool pEnable);
	virtual void ScreenShot();
};
#endif


//};
// ImageLib lib (for loading images and applying filters)
unsigned int m_width=800;
unsigned int m_height=600;
#ifdef _MSCVER
unsigned int m_bpp=32;
#else
unsigned int m_bpp=24;
#endif

void IWindow::UpdateWindowArgs()
{/*
	//???:
	x=0;
	y=0;
	return;
*/
#if defined(__APPLE__)
	return;
#endif
#if defined(SDL_VIDEO_DRIVER_WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
	RECT r;
	GetWindowRect(m_hWnd, &r); 
	x=r.left;
	y=r.top;
	//width=r.right-x;
	//height=r.bottom-y;
#elif defined(SDL_VIDEO_DRIVER_X11)
	XWindowAttributes a;
	XGetWindowAttributes(m_dpy, m_win ,&a);

	x=a.x;
	y=a.y;
	//width=a.width;//-a.x;
	//height=a.height;//-a.y;
	Window child;
	int ax, ay;
	XTranslateCoordinates(m_dpy, m_win, RootWindow(m_dpy, 0),
                          0, 0, &ax, &ay, &child);
	if (child != None)
	{
        	int left, top;
       		XTranslateCoordinates(m_dpy, m_win, child,
                              0, 0, &left, &top, &child);
        	ax -= left;
        	ay -= top;
	
    	}
    	x = ax;
    	y=ay;
#endif
}

#if defined(SDL_2_0_5_BUILD)

void
SDL2_0Window::ScreenShot()//SDL_Renderer *renderer)
{
    SDL_Rect viewport;
    SDL_Surface *surface;

//    SDL_RenderGetViewport(renderer, &viewport);
    surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                    0x00FF0000, 0x0000FF00, 0x000000FF,
#else
                    0x000000FF, 0x0000FF00, 0x00FF0000,
#endif
                    0x00000000);
    if (!surface) {
        LOG_PRINT_X("Couldn't create surface: %s\n", SDL_GetError());
        return;
    }
/*

    if (SDL_RenderReadPixels(renderer, 0, surface->format->format,
                             surface->pixels, surface->pitch) < 0) {
        LOG_PRINT_X("Couldn't read screen: %s\n", SDL_GetError());
        return;

    }
*/
    if (SDL_SaveBMP(surface, "screenshot.bmp") < 0) {
        LOG_PRINT_X("Couldn't save screenshot.bmp: %s\n", SDL_GetError());
        return;
    }
}

	void SDL2_0Window::SetCaption(const char * buf)
	{
		this->title=buf;
		SDL_SetWindowTitle(STX_Service::GetSDLWindow(), title.c_str());
	}
	void SDL2_0Window::ShowCursor (bool pEnable)
{
	if (pEnable)
		SDL_ShowCursor (
		//???
		SDL_ENABLE);
	else
		SDL_ShowCursor (
		//???
		SDL_DISABLE);
}
void SDL2_0Window::ReSize(int awidth, int aheight, int abpp)
{
	//LOG_PRINT_X("ST:SDL2_0Window::Init\n");
	width=awidth;
	height=aheight;
	bpp=abpp;
	}

void SDL2_0Window::Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)
{
	bpp=abpp;

	#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
GLint viewport[4];
glGetIntegerv(GL_VIEWPORT, viewport); 
x=viewport[0];
y=viewport[1];
width=viewport[2];
height=viewport[3];
#else
	//width=awidth;
	//height=aheight;
	x=0;
	y=0;
#endif
	title=atitle;
#if 1//!defined(ANDROID)

    int done;
    SDL_Surface *surface;
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    //LOG_PRINT("SDL_Init1\n");
    if ( SDL_Init(
		SDL_INIT_TIMER|
		 SDL_INIT_AUDIO|
		//SDL_INIT_EVENTTHREAD|
		 SDL_INIT_JOYSTICK|SDL_INIT_VIDEO) < 0 ) {
        LOG_PRINT_X("Unable to initialize SDL: %s\n", SDL_GetError());
        stx_exit(1);
    }
    //LOG_PRINT("SDL_Init2\n");
#if 0//defined(ANDROID)
    if ((mutex = SDL_CreateMutex()) == NULL) {
        LOG_PRINT_X("Couldn't create mutex: %s\n", SDL_GetError());
        stx_exit(1);
    }
#endif
    STX_FNLN;
  	STX_PRINT("windowwidth=%d\n", width);
  	STX_PRINT("windowheight=%d\n", height);
    SDL_Window* SDL_Window_ = SDL_CreateWindow( atitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
    if ( !SDL_Window_ ) {
        LOG_PRINT_X("Unable to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        stx_exit(2);
    }
    STX_Service::SetSDLWindow(SDL_Window_);

    SDL_GLContext SDLGLContext=SDL_GL_CreateContext(SDL_Window_);
    STX_Service::SetSDLContext(SDLGLContext);
	SDL_GL_MakeCurrent(SDL_Window_, SDLGLContext);
#endif

	//???SDL_GL_SetSwapInterval(0);
#if defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(SDL_Window_, &wmInfo);

#if defined(__MINGW32__) || defined(__MINGW64__)
	m_hWnd=::GetActiveWindow();
	m_hinstance=::GetModuleHandle(0);
	m_hdc=::GetDC(0);
	LOG_PRINT_X("m_hWnd=%x\n", m_hWnd);
	RECT r;
	GetWindowRect(m_hWnd, &r); 
	x=r.left;
	y=r.top;
	//width=r.right-x;
	//height=r.bottom-y;
	LOG_PRINT_X("m_hWnd=%x\n", m_hWnd);
#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
	m_hWnd=(HWND)wmInfo.info.win.window;
	//m_hinstance=(HINSTANCE)wmInfo.info.win.hinstance;
	m_hinstance=::GetModuleHandle(0);
	m_hdc=(HDC)wmInfo.info.win.hdc;
	LOG_PRINT_X("m_hWnd=%x\n", m_hWnd);
	RECT r;
	GetWindowRect(m_hWnd, &r); 
	x=r.left;
	y=r.top;
	//width=r.right-x;
	//height=r.bottom-y;
#elif defined(SDL_VIDEO_DRIVER_X11)
        m_dpy=wmInfo.info.x11.display;
	m_win=wmInfo.info.x11.window;

    	g_X11_display=wmInfo.info.x11.display;
    	g_X11_window=wmInfo.info.x11.window;

	STX_PRINT("m_dpy=%x\n", m_dpy);
	STX_PRINT("m_win=%x\n", m_win);

	STX_PRINT("g_X11_display=%x\n", g_X11_display);
	STX_PRINT("g_X11_window=%x\n", g_X11_window);

	STX_PRINT("SDL_window_=%x\n", SDL_Window_);

#endif
#endif
/*
#if defined(SDL_VIDEO_DRIVER_COCOA)
        struct
        {
            NSWindow *window;           // The Cocoa window 
        } cocoa;
#endif
#if defined(SDL_VIDEO_DRIVER_UIKIT)
        struct
        {
            UIWindow *window;           // The UIKit window 
        } uikit;
#endif
*/
	//UpdateWindowArgs();

	char buf[1024] = {0};
	stx_strlcpy(buf, "STX - SDL2_0Window - ", 1024);
	stx_strlcat(buf, atitle, 1024);
	SDL_SetWindowTitle(SDL_Window_, buf);

	STX_Service::GetAudioInstance();
	STX_Service::GetInputInstance();
	STX_Service::GetTimerInstance();
}
#endif

#if 0
// --------------------------------------------------------------------------------
//									  Public methods
// --------------------------------------------------------------------------------

/*!
\b Operation:

Update input function. This method has to be called everytime in the game loop.
*/
	  float SDLInput::m_fSpinX=0.0f;
      float SDLInput::m_fSpinY=0.0f;
      float SDLInput::m_fX=0.0f;
      float SDLInput::m_fY=0.0f;
      float SDLInput::m_fZ=0.0f;
      float  SDLInput::m_fSpinX_L=0.0f;
      float  SDLInput::m_fSpinY_L=0.0f;
      float  SDLInput::m_fSpinX_R=0.0f;
      float  SDLInput::m_fSpinY_R=0.0f;


#include <STX/CInput.h>
SDLInput::SDLInput (): IInput()
{
	LOG_PRINT_X("SDLInput\n");
	//SDL_Init(SDL_INIT_EVENTTHREAD);
	m_quit=false;
	for (int i = 0; i < 132; i++)
	{
		mKeys [i].mKey = i;
		mKeys [i].mPressed = 0;
		mKeys [i].mReleased = 0;
	}

	mMouse.mMouseButtons [STX_MBUTTON_LEFT].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_LEFT].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_RIGHT].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_RIGHT].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].mReleased = 0;
}

bool SDLInput::Update()
{
	static SDL_Event mEvent;

	// ----- Flags initialization -----
	CKey::key=0;
	for (int i = 0; i < 132; i++)
	{
		mKeys [i].mPressed = 0;
		mKeys [i].mReleased = 0;
	}

	mMouse.mMouseMotion = 0;
	mMouse.mMouseButtons [STX_MBUTTON_LEFT].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_LEFT].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_RIGHT].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_RIGHT].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].mReleased = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].mPressed = 0;
	mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].mReleased = 0;

	// ----- Update -----

	while (SDL_PollEvent (&mEvent))
	{
		// ----- When minimized or focus lost -----

		while (!(SDL_GetAppState () &
			//
			SDL_APPACTIVE) || !(SDL_GetAppState () &
			//
			SDL_APPINPUTFOCUS))
		{
			SDL_WaitEvent(&mEvent);
		}

	mMouse.mMouseMotion = 0;
	mMouse.mMouseButton=0;
	mMouse.mMouseWheel=0;
	mMouse.mbMouseButton=0;
	mJoy.mJoyAxisMotion = 0;
	mJoy.mJoyBallMotion = 0;
	mJoy.mJoyHatMotion = 0;
	mJoy.mJoyButton=0;

		// ----- Mouse ------

		SDL_GetMouseState (&mMouse.mMouseX, &mMouse.mMouseY);

		switch (mEvent.type)
		{
			// ----- Window closed ------

			case SDL_QUIT: mQuit = 1; break;

			// ----- Window focus changed -----

			/*
			case SDL_ACTIVEEVENT:
			if (mEvent.active.state & SDL_APPACTIVE )
			{
				// Application iconified
				if( mEvent.active.gain == 0 )
				{

				}
				// Application restored
				else
				{

				}
			}
			break;
			*/


			case SDL_JOYAXISMOTION: mJoy.mJoyAxisMotion = 1;
				mJoy.AxisJoystickDeviceIndex=mEvent.jaxis.which;
				mJoy.axisIndex=mEvent.jaxis.axis;
				mJoy.axisvalue=mEvent.jaxis.value;
				break;
			case SDL_JOYBALLMOTION: mJoy.mJoyBallMotion = 1;
				mJoy.BallJoystickDeviceIndex=mEvent.jball.which;
				mJoy.ballIndex=mEvent.jball.ball;
				mJoy.ballxrel=mEvent.jball.xrel;
				mJoy.ballyrel=mEvent.jball.yrel;
				break;
			case SDL_JOYHATMOTION: mJoy.mJoyHatMotion = 1;
				mJoy.HatJoystickDeviceIndex=mEvent.jhat.which;
				mJoy.hatIndex=mEvent.jhat.hat;
				mJoy.hatvalue=mEvent.jhat.value;
				break;
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
	//mEvent.jaxis;
	//mEvent.jball;
	//mEvent.jhat;
	mJoy.AxisJoystickDeviceIndex=mEvent.jbutton.which;	/**< The joystick device index */
	mJoy.buttonIndex=mEvent.jbutton.button;	/**< The joystick button index */



	switch (mEvent.jbutton.state)
	{
	case SDL_PRESSED:
		mJoy.buttonState=STX_JOYBUTTONDOWN;	/**< SDL_PRESSED or SDL_RELEASED */
		mJoy.mJoyButton=1;
				break;
	case SDL_RELEASED:
		mJoy.buttonState=STX_JOYBUTTONUP;	/**< SDL_PRESSED or SDL_RELEASED */
				break;
	};
			//case SDL_JOYEVENTMASK: mMouse.mMouseMotion = 1;	break;

			// ----- Mouse motion -----

			case SDL_MOUSEMOTION: mMouse.mMouseMotion = 1;	break;

			// ----- Mouse buttons -----

			case SDL_MOUSEBUTTONDOWN:
				mMouse.mbMouseButton=1;
			switch (mEvent.button.button)
			{
				case SDL_BUTTON_LEFT: mMouse.mMouseButtons [STX_MBUTTON_LEFT].SetState (MBUTTON_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_RIGHT: mMouse.mMouseButtons [STX_MBUTTON_RIGHT].SetState (MBUTTON_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_MIDDLE: mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].SetState (MBUTTON_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_WHEELUP: mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].SetState (MBUTTON_PRESSED); mMouse.mMouseWheel=1; break;
				case SDL_BUTTON_WHEELDOWN: mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].SetState (MBUTTON_PRESSED); mMouse.mMouseWheel=1; break;

			}
			break;

			case SDL_MOUSEBUTTONUP:
			switch (mEvent.button.button)
			{
				case SDL_BUTTON_LEFT: mMouse.mMouseButtons [STX_MBUTTON_LEFT].SetState (MBUTTON_NOT_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_RIGHT: mMouse.mMouseButtons [STX_MBUTTON_RIGHT].SetState (MBUTTON_NOT_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_MIDDLE: mMouse.mMouseButtons [STX_MBUTTON_MIDDLE].SetState (MBUTTON_NOT_PRESSED); mMouse.mMouseButton=1; break;
				case SDL_BUTTON_WHEELUP: mMouse.mMouseButtons [STX_MBUTTON_WHEELUP].SetState (MBUTTON_NOT_PRESSED); mMouse.mMouseWheel=1; break;
				case SDL_BUTTON_WHEELDOWN: mMouse.mMouseButtons [STX_MBUTTON_WHEELDOWN].SetState (MBUTTON_NOT_PRESSED); mMouse.mMouseWheel=1; break;
			}
			break;
		}

		// ----- Keyboard ------

		CKey::key=mEvent.key.keysym.sym;
		switch (mEvent.key.keysym.sym)
		{
		case SDLK_a:			mKeys [STX_KEY_A].SetState (mEvent.type);  break;
			case SDLK_b:			mKeys [STX_KEY_B].SetState (mEvent.type);  break;
			case SDLK_c:			mKeys [STX_KEY_C].SetState (mEvent.type);  break;
			case SDLK_d:			mKeys [STX_KEY_D].SetState (mEvent.type);  break;
			case SDLK_e:			mKeys [STX_KEY_E].SetState (mEvent.type);  break;
			case SDLK_f:			mKeys [STX_KEY_F].SetState (mEvent.type);  break;
			case SDLK_g:			mKeys [STX_KEY_G].SetState (mEvent.type);  break;
			case SDLK_h:			mKeys [STX_KEY_H].SetState (mEvent.type);  break;
			case SDLK_i:			mKeys [STX_KEY_I].SetState (mEvent.type);  break;
			case SDLK_j:			mKeys [STX_KEY_J].SetState (mEvent.type);  break;
			case SDLK_k:			mKeys [STX_KEY_K].SetState (mEvent.type);  break;
			case SDLK_l:			mKeys [STX_KEY_L].SetState (mEvent.type);  break;
			case SDLK_m:			mKeys [STX_KEY_M].SetState (mEvent.type);  break;
			case SDLK_n:			mKeys [STX_KEY_N].SetState (mEvent.type);  break;
			case SDLK_o:			mKeys [STX_KEY_O].SetState (mEvent.type);  break;
			case SDLK_p:			mKeys [STX_KEY_P].SetState (mEvent.type);  break;
			case SDLK_q:			mKeys [STX_KEY_Q].SetState (mEvent.type);  break;
			case SDLK_r:			mKeys [STX_KEY_R].SetState (mEvent.type);  break;
			case SDLK_s:			mKeys [STX_KEY_S].SetState (mEvent.type);  break;
			case SDLK_t:			mKeys [STX_KEY_T].SetState (mEvent.type);  break;
			case SDLK_u:			mKeys [STX_KEY_U].SetState (mEvent.type);  break;
			case SDLK_v:			mKeys [STX_KEY_V].SetState (mEvent.type);  break;
			case SDLK_w:			mKeys [STX_KEY_W].SetState (mEvent.type);  break;
			case SDLK_x:			mKeys [STX_KEY_X].SetState (mEvent.type);  break;
			case SDLK_y:			mKeys [STX_KEY_Y].SetState (mEvent.type);  break;
			case SDLK_z:			mKeys [STX_KEY_Z].SetState (mEvent.type);  break;

			case SDLK_BACKSPACE:	mKeys [STX_KEY_BACKSPACE].SetState (mEvent.type);  break;
			case SDLK_TAB:			mKeys [STX_KEY_TAB].SetState (mEvent.type);  break;
			case SDLK_CLEAR:		mKeys [STX_KEY_CLEAR].SetState (mEvent.type);  break;
			case SDLK_RETURN:		mKeys [STX_KEY_RETURN].SetState (mEvent.type);  break;
			case SDLK_PAUSE:		mKeys [STX_KEY_PAUSE].SetState (mEvent.type);  break;
			case SDLK_ESCAPE:		mKeys [STX_KEY_ESCAPE].SetState (mEvent.type);  break;
			case SDLK_SPACE:		mKeys [STX_KEY_SPACE].SetState (mEvent.type);  break;
			case SDLK_EXCLAIM:		mKeys [STX_KEY_EXCLAIM].SetState (mEvent.type);  break;
			case SDLK_QUOTEDBL:		mKeys [STX_KEY_QUOTEDBL].SetState (mEvent.type);  break;
			case SDLK_HASH:			mKeys [STX_KEY_HASH].SetState (mEvent.type);  break;
			case SDLK_DOLLAR:		mKeys [STX_KEY_DOLLAR].SetState (mEvent.type);  break;
			case SDLK_AMPERSAND:	mKeys [STX_KEY_AMPERSAND].SetState (mEvent.type);  break;
			case SDLK_QUOTE:		mKeys [STX_KEY_QUOTE].SetState (mEvent.type);  break;
			case SDLK_LEFTPAREN:	mKeys [STX_KEY_LEFTPAREN].SetState (mEvent.type);  break;
			case SDLK_RIGHTPAREN:	mKeys [STX_KEY_RIGHTPAREN].SetState (mEvent.type);  break;
			case SDLK_ASTERISK:		mKeys [STX_KEY_ASTERISK].SetState (mEvent.type);  break;
			case SDLK_PLUS:			mKeys [STX_KEY_PLUS].SetState (mEvent.type);  break;
			case SDLK_COMMA:		mKeys [STX_KEY_COMMA].SetState (mEvent.type);  break;
			case SDLK_MINUS:		mKeys [STX_KEY_MINUS].SetState (mEvent.type);  break;
			case SDLK_PERIOD:		mKeys [STX_KEY_PERIOD].SetState (mEvent.type);  break;
			case SDLK_SLASH:		mKeys [STX_KEY_SLASH].SetState (mEvent.type);  break;

			case SDLK_0:			mKeys [STX_KEY_0].SetState (mEvent.type);  break;
			case SDLK_1:			mKeys [STX_KEY_1].SetState (mEvent.type);  break;
			case SDLK_2:			mKeys [STX_KEY_2].SetState (mEvent.type);  break;
			case SDLK_3:			mKeys [STX_KEY_3].SetState (mEvent.type);  break;
			case SDLK_4:			mKeys [STX_KEY_4].SetState (mEvent.type);  break;
			case SDLK_5:			mKeys [STX_KEY_5].SetState (mEvent.type);  break;
			case SDLK_6:			mKeys [STX_KEY_6].SetState (mEvent.type);  break;
			case SDLK_7:			mKeys [STX_KEY_7].SetState (mEvent.type);  break;
			case SDLK_8:			mKeys [STX_KEY_8].SetState (mEvent.type);  break;
			case SDLK_9:			mKeys [STX_KEY_9].SetState (mEvent.type);  break;

			case SDLK_COLON:		mKeys [STX_KEY_COLON].SetState (mEvent.type);  break;
			case SDLK_SEMICOLON:	mKeys [STX_KEY_SEMICOLON].SetState (mEvent.type);  break;
			case SDLK_LESS:			mKeys [STX_KEY_LESS].SetState (mEvent.type);  break;
			case SDLK_EQUALS:		mKeys [STX_KEY_EQUALS].SetState (mEvent.type);  break;
			case SDLK_GREATER:		mKeys [STX_KEY_GREATER].SetState (mEvent.type);  break;
			case SDLK_QUESTION:		mKeys [STX_KEY_QUESTION].SetState (mEvent.type);  break;
			case SDLK_AT:			mKeys [STX_KEY_AT].SetState (mEvent.type);  break;
			case SDLK_LEFTBRACKET:	mKeys [STX_KEY_LEFTBRACKET].SetState (mEvent.type);  break;
			case SDLK_BACKSLASH:	mKeys [STX_KEY_BACKSLASH].SetState (mEvent.type);  break;
			case SDLK_RIGHTBRACKET:	mKeys [STX_KEY_RIGHTBRACKET].SetState (mEvent.type);  break;
			case SDLK_CARET:		mKeys [STX_KEY_CARET].SetState (mEvent.type);  break;
			case SDLK_UNDERSCORE:	mKeys [STX_KEY_UNDERSCORE].SetState (mEvent.type);  break;
			case SDLK_BACKQUOTE:	mKeys [STX_KEY_BACKQUOTE].SetState (mEvent.type);  break;

			case SDLK_DELETE:		mKeys [STX_KEY_DELETE].SetState (mEvent.type);  break;
			case SDLK_KP0:			mKeys [STX_KEY_K0].SetState (mEvent.type);  break;
			case SDLK_KP1:			mKeys [STX_KEY_K1].SetState (mEvent.type);  break;
			case SDLK_KP2:			mKeys [STX_KEY_K2].SetState (mEvent.type);  break;
			case SDLK_KP3:			mKeys [STX_KEY_K3].SetState (mEvent.type);  break;
			case SDLK_KP4:			mKeys [STX_KEY_K4].SetState (mEvent.type);  break;
			case SDLK_KP5:			mKeys [STX_KEY_K5].SetState (mEvent.type);  break;
			case SDLK_KP6:			mKeys [STX_KEY_K6].SetState (mEvent.type);  break;
			case SDLK_KP7:			mKeys [STX_KEY_K7].SetState (mEvent.type);  break;
			case SDLK_KP8:			mKeys [STX_KEY_K8].SetState (mEvent.type);  break;
			case SDLK_KP9:			mKeys [STX_KEY_K9].SetState (mEvent.type);  break;

			case SDLK_KP_PERIOD:	mKeys [STX_KEY_KPERIOD].SetState (mEvent.type);  break;
			case SDLK_KP_DIVIDE:	mKeys [STX_KEY_KDIVIDE].SetState (mEvent.type);  break;
			case SDLK_KP_MULTIPLY:	mKeys [STX_KEY_KMULTIPLY].SetState (mEvent.type);  break;
			case SDLK_KP_MINUS:		mKeys [STX_KEY_KMINUS].SetState (mEvent.type);  break;
			case SDLK_KP_PLUS:		mKeys [STX_KEY_KPLUS].SetState (mEvent.type);  break;
			case SDLK_KP_ENTER:		mKeys [STX_KEY_KENTER].SetState (mEvent.type);  break;
			case SDLK_KP_EQUALS:	mKeys [STX_KEY_KEQUALS].SetState (mEvent.type);  break;

			case SDLK_UP:			mKeys [STX_KEY_KEYUP].SetState (mEvent.type);  break;
			case SDLK_DOWN:			mKeys [STX_KEY_KEYDOWN].SetState (mEvent.type);  break;
			case SDLK_RIGHT:		mKeys [STX_KEY_KEYRIGHT].SetState (mEvent.type);  break;
			case SDLK_LEFT:			mKeys [STX_KEY_KEYLEFT].SetState (mEvent.type);  break;
			case SDLK_INSERT:		mKeys [STX_KEY_INSERT].SetState (mEvent.type);  break;
			case SDLK_HOME:			mKeys [STX_KEY_HOME].SetState (mEvent.type);  break;
			case SDLK_END:			mKeys [STX_KEY_END].SetState (mEvent.type);  break;
			case SDLK_PAGEUP:		mKeys [STX_KEY_PAGEUP].SetState (mEvent.type);  break;
			case SDLK_PAGEDOWN:		mKeys [STX_KEY_PAGEDOWN].SetState (mEvent.type);  break;

			case SDLK_F1:			mKeys [STX_KEY_F1].SetState (mEvent.type);  break;
			case SDLK_F2:			mKeys [STX_KEY_F2].SetState (mEvent.type);  break;
			case SDLK_F3:			mKeys [STX_KEY_F3].SetState (mEvent.type);  break;
			case SDLK_F4:			mKeys [STX_KEY_F4].SetState (mEvent.type);  break;
			case SDLK_F5:			mKeys [STX_KEY_F5].SetState (mEvent.type);  break;
			case SDLK_F6:			mKeys [STX_KEY_F6].SetState (mEvent.type);  break;
			case SDLK_F7:			mKeys [STX_KEY_F7].SetState (mEvent.type);  break;
			case SDLK_F8:			mKeys [STX_KEY_F8].SetState (mEvent.type);  break;
			case SDLK_F9:			mKeys [STX_KEY_F9].SetState (mEvent.type);  break;
			case SDLK_F10:			mKeys [STX_KEY_F10].SetState (mEvent.type);  break;
			case SDLK_F11:			mKeys [STX_KEY_F11].SetState (mEvent.type);  break;
			case SDLK_F12:			mKeys [STX_KEY_F12].SetState (mEvent.type);  break;
			case SDLK_F13:			mKeys [STX_KEY_F13].SetState (mEvent.type);  break;
			case SDLK_F14:			mKeys [STX_KEY_F14].SetState (mEvent.type);  break;
			case SDLK_F15:			mKeys [STX_KEY_F15].SetState (mEvent.type);  break;

			case SDLK_NUMLOCK:		mKeys [STX_KEY_NUMLOCK].SetState (mEvent.type);  break;
			case SDLK_CAPSLOCK:		mKeys [STX_KEY_CAPSLOCK].SetState (mEvent.type);  break;
			case SDLK_SCROLLOCK:	mKeys [STX_KEY_SCROLLOCK].SetState (mEvent.type);  break;
			case SDLK_RSHIFT:		mKeys [STX_KEY_RSHIFT].SetState (mEvent.type);  break;
			case SDLK_LSHIFT:		mKeys [STX_KEY_LSHIFT].SetState (mEvent.type);  break;
			case SDLK_RCTRL:		mKeys [STX_KEY_RCTRL].SetState (mEvent.type);  break;
			case SDLK_LCTRL:		mKeys [STX_KEY_LCTRL].SetState (mEvent.type);  break;
			case SDLK_RALT:			mKeys [STX_KEY_RALT].SetState (mEvent.type);  break;
			case SDLK_LALT:			mKeys [STX_KEY_LALT].SetState (mEvent.type);  break;
			case SDLK_RMETA:		mKeys [STX_KEY_RMETA].SetState (mEvent.type);  break;
			case SDLK_LMETA:		mKeys [STX_KEY_LMETA].SetState (mEvent.type);  break;
//			case SDLK_LSUPER:		mKeys [STX_KEY_LSUPER].SetState (mEvent.type);  break;
//			case SDLK_RSUPER:		mKeys [STX_KEY_RSUPER].SetState (mEvent.type);  break;
			case SDLK_MODE:			mKeys [STX_KEY_MODE].SetState (mEvent.type);  break;
			case SDLK_HELP:			mKeys [STX_KEY_HELP].SetState (mEvent.type);  break;
			case SDLK_PRINT:		mKeys [STX_KEY_PRINT].SetState (mEvent.type);  break;
			case SDLK_SYSREQ:		mKeys [STX_KEY_SYSREQ].SetState (mEvent.type);  break;
			case SDLK_BREAK:		mKeys [STX_KEY_BREAK].SetState (mEvent.type);  break;
			case SDLK_MENU:			mKeys [STX_KEY_MENU].SetState (mEvent.type);  break;
			case SDLK_POWER:		mKeys [STX_KEY_POWER].SetState (mEvent.type);  break;
//			case SDLK_EURO:			mKeys [STX_KEY_EURO].SetState (mEvent.type);  break;
		}
	}
float x=0.0f;//319.5f;//9.8875f;//*2.0f*PI/360.0f;
float y=0.0f;//300.96f;//=6.836f/2.0f;//*2.0f*PI/360.0f;

	const SDL_VideoInfo * vi=SDL_GetVideoInfo();
	UINT width = vi->current_w;
	UINT height = vi->current_h;

x+=GetMouseX()
//*2.0f*PI
*360.0f*1.1f
/ width;
y+=GetMouseY()
//*PI
*180.0f*1.1f
/ height;
/*
//! Left mouse button
#define STX_MBUTTON_LEFT 0
//! Right mouse button
#define STX_MBUTTON_RIGHT 1
//! Middle mouse button
#define STX_MBUTTON_MIDDLE 2
//! Wheelup mouse button
#define STX_MBUTTON_WHEELUP 3
//! Wheeldown mouse button
#define STX_MBUTTON_WHEELDOWN 4
*/


	  m_fX=x;
      m_fY=y;
if(IsMouseButtonPressed(STX_MBUTTON_WHEELDOWN))
	  m_fZ=y;
if(IsMouseButtonPressed(STX_MBUTTON_LEFT))
{
	  m_fSpinX=x;
      m_fSpinY=y;
	  m_fSpinX_L=x;
      m_fSpinY_L=y;




}
if(IsMouseButtonPressed(STX_MBUTTON_RIGHT))
{
      m_fSpinX_R=x;
      m_fSpinY_R=y;
}
	return true;
}





/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::eKey.

\b Operation:

This function returns true if the key passed as \b pKey parameter is pressed.
*/
bool SDLInput::OnKeyPress	(eKey pKey)
{
	return mKeys [pKey].mPressed;
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::eKey.

\b Operation:

This function returns true if the key passed as \b pKey parameter is released.
*/
bool SDLInput::OnKeyRelease	(eKey pKey)

{
	return mKeys [pKey].mReleased;
}



/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::eKey.

\b Operation:

This function returns true if the key passed as \b pKey parameter is being pressed.
*/
bool SDLInput::IsKeyPressed	(eKey pKey)
{
	if (mKeys [pKey].mKeyState == STX_KEY_BUTTON_PRESSED)
	{
		return 1;
	}

	return 0;
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::eKey.
\arg \b pTime				Time that has to pass in milliseconds.

\b Operation:

This function returns true if the key passed as \b pKey parameter has been pressed \b pTime milliseconds.
*/
bool SDLInput::IsKeyPressed	(eKey pKey, unsigned int pTime)
{
	return IsKeyPressed	(pKey);
}


/*!
\b Operation:

This function returns the true if the mouse is moving
*/
bool SDLInput::IsMouseMotion ()
{
	return mMouse.mMouseMotion;
}


/*!
\b Operation:

This function returns the horizontal position of the mouse in screen pixels
*/
int SDLInput::GetMouseX ()
{
	return mMouse.mMouseX;
}


/*!
\b Operation:

This function returns the vertical position of the mouse in screen pixels
*/
int SDLInput::GetMouseY ()
{
	return mMouse.mMouseY;
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::eMouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is pressed. It will return true again only if the button
is released and pressed again.
*/
bool SDLInput::OnMouseButtonPress (eMouseButton pMouseButton)
{
	return mMouse.mMouseButtons [pMouseButton].mPressed;
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::eMouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is released. It will return true again only if the button
is pressed and released again.
*/
bool SDLInput::OnMouseButtonRelease	(eMouseButton pMouseButton)
{
	return mMouse.mMouseButtons [pMouseButton].mReleased;
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::eMouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is being pressed.
*/
bool SDLInput::IsMouseButtonPressed (eMouseButton pMouseButton)
{
	if (mMouse.mMouseButtons [pMouseButton].mButtonState == MBUTTON_PRESSED)
	{
		return 1;
	}

	return 0;
}

/*!
\b Parameters:

\arg \b pMouseButton		Mouse button that we want to check. See ::eMouseButton.
\arg \b pTime				Time that has to pass in milliseconds.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton parameter has been pressed \b pTime milliseconds.
*/
bool SDLInput::IsMouseButtonPressed (eMouseButton pMouseButton, unsigned int pTime)
{
	return IsMouseButtonPressed (pMouseButton);
}
#endif

// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------


/*
*************************
Initialized the input flags
*************************
*/

/*
$HOME/SDL-1.2.14/test/testwm.c
Qt QGLWidget:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
*/
#if defined(SDL_1_2_15_BUILD)

	//SDL_Surface* screen;

void
SDL1_2Window::ScreenShot()//SDL_Renderer *renderer)
{
    SDL_Rect viewport;
    SDL_Surface *surface;

//    SDL_RenderGetViewport(renderer, &viewport);
    surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                    0x00FF0000, 0x0000FF00, 0x000000FF,
#else
                    0x000000FF, 0x0000FF00, 0x00FF0000,
#endif
                    0x00000000);
    if (!surface) {
        LOG_PRINT_X("Couldn't create surface: %s\n", SDL_GetError());
        return;
    }
/*
    if (SDL_RenderReadPixels(renderer, 0, surface->format->format,
                             surface->pixels, surface->pitch) < 0) {
        LOG_PRINT_X("Couldn't read screen: %s\n", SDL_GetError());
        return;
    }
*/
    if (SDL_SaveBMP(surface, "screenshot.bmp") < 0) {
        LOG_PRINT_X("Couldn't save screenshot.bmp: %s\n", SDL_GetError());
        return;
    }
}

	void SDL1_2Window::SetCaption(const char * buf)
	{
		SDL_WM_SetCaption(buf,0);
		this->title=buf;
	}
	void SDL1_2Window::ShowCursor (bool pEnable)
{
	if (pEnable)
		SDL_ShowCursor (
		//???
		SDL_ENABLE);
	else
		SDL_ShowCursor (
		//???
		SDL_DISABLE);
}
void SDL1_2Window::ReSize(int awidth, int aheight, int abpp)
{
	//LOG_PRINT_X("ST:SDL1_2Window::Init\n");
	width=awidth;
	height=aheight;
	bpp=abpp;
screen = SDL_SetVideoMode(width, height, bpp, nFlags);
	}

void SDL1_2Window::Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)
{
	bpp=abpp;

	#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
GLint viewport[4];
glGetIntegerv(GL_VIEWPORT, viewport); 
x=viewport[0];
y=viewport[1];
width=viewport[2];
height=viewport[3];
#else
	//width=awidth;
	//height=aheight;
	x=0;
	y=0;
#endif



	title=atitle;
    //LOG_PRINT("SDL_Init1\n");

    	SDL_Init(	SDL_INIT_TIMER|
			SDL_INIT_AUDIO|
			//SDL_INIT_EVENTTHREAD|
			SDL_INIT_JOYSTICK|
			SDL_INIT_VIDEO);
    //LOG_PRINT("SDL_Init2\n");

    if ((mutex = SDL_CreateMutex()) == NULL) {
        LOG_PRINT_X("Couldn't create mutex: %s\n", SDL_GetError());
        stx_exit(1);
    }

	char buf[1024] = {0};
	stx_strlcpy(buf, "STX - SDL1_2Window - ", 1024);
	stx_strlcat(buf, atitle, 1024);

    SDL_WM_SetCaption( buf, 0 );

    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();

    if( !pSDLVideoInfo )
    {
        LOG_PRINT_X("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    int nFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
    
    if( pSDLVideoInfo->hw_available ) // Hardware surfaces enabled?
        nFlags |= SDL_HWSURFACE;
    else
        nFlags |= SDL_SWSURFACE;
  
    if( pSDLVideoInfo->blit_hw ) // Hardware supported blitting?
        nFlags |= SDL_HWACCEL;
  
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // Enable OpenGL Doublebuffering
  
    // Create our rendering surface

    width=windowwidth;
	height=windowheight;
    STX_FNLN;
  	STX_PRINT("windowwidth=%d\n", width);
  	STX_PRINT("windowheight=%d\n", height);
    SDL_Surface *pSDLSurface = SDL_SetVideoMode( width, height, abpp, nFlags );
  
    if( !pSDLSurface )
    {
        LOG_PRINT( "Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return;
    }
#if defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WINDOWS)
	SDL_SysWMinfo info;
    	SDL_VERSION(&info.version);
   	SDL_GetWMInfo(&info);
#endif
#if defined(__MINGW32__) || defined(__MINGW64__)
	m_hWnd=GetActiveWindow();
	m_hinstance=::GetModuleHandle(0);
	m_hdc=::GetDC(0);
	LOG_PRINT_X("m_hWnd=%x\n", m_hWnd);
#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
	m_hWnd=(HWND)info.window;
	m_hinstance=::GetModuleHandle(0);
	m_hdc=::GetDC(0);
	LOG_PRINT_X("m_hWnd=%x\n", m_hWnd);
#elif defined(SDL_VIDEO_DRIVER_X11)
        m_dpy=info.info.x11.display;
	m_win=info.info.x11.window;
	LOG_PRINT_X("m_dpy=%x\n", m_dpy);
#elif defined(LINUX)
	m_connection=XGetXCBConnection(glfwGetX11Display());
	m_window=glfwGetX11Window(window);
#endif

	LOG_PRINT("m_dpy=%x\n", m_dpy);
	LOG_PRINT("m_win=%x\n", m_win);

	LOG_PRINT("g_X11_display=%x\n", g_X11_display);
	LOG_PRINT("g_X11_window=%x\n", g_X11_window);

	LOG_PRINT("SDL_window_=%x\n", SDL_window_);

	//UpdateWindowArgs();
	STX_Service::GetAudioInstance();
	STX_Service::GetInputInstance();
	STX_Service::GetTimerInstance();
}
#endif

IWindow* STX_Service::s_pWindow=0;
IInput* STX_Service::s_pInput=0;
IAudio * STX_Service::s_pAudio=0;
STX_TIMER* STX_Service::s_pTimer=0;

#if 0
STX_TIMER* STX_Service::GetTimerInstance()
{
	if(STX_Service::s_pTimer==0)
		STX_Service::s_pTimer=new STX_SDL_TIMER();
	return STX_Service::s_pTimer;
}
#endif

STX_TIMER* STX_Service::GetTimerInstance()
{
	if(STX_Service::s_pTimer==0)
	{
#if defined(SDL_1_2_15_BUILD) ||  defined(SDL_2_0_5_BUILD)
		STX_Service::s_pTimer=new STX_SDL_TIMER();
#elif !defined(__APPLE__) && !defined(__MINGW32__) && !defined(__MINGW64__)
		STX_Service::s_pTimer=new STX_Dummy_TIMER();
#else
		STX_Service::s_pTimer=0;
#endif
	}
		#if defined(ANDROID)
		
		#endif
	return STX_Service::s_pTimer;
}
#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
class SDLInput;
#endif
IInput * STX_Service::GetInputInstance(tInput a )
{
	if (STX_Service::s_pInput == 0)
	{
#if defined(__MINGW32__) || defined(__MINGW64__)
		STX_Service::s_pInput = new MWInput();
#elif defined(SDL_2_0_5_BUILD) || defined(SDL_1_2_15_BUILD)
		STX_Service::s_pInput = new SDLInput();
#else
		STX_Service::s_pInput = new DummyInput();
#endif
	}	
		#if defined(ANDROID)
		
		#endif
	return STX_Service::s_pInput;
}

void STX_Service::CloseWindow(){
	STX_TRY;
	if(s_pWindow) delete s_pWindow;s_pWindow=0;
	STX_CATCH;}
void STX_Service::CloseInput(){
	STX_TRY;
	if(s_pInput) delete s_pInput;s_pInput=0;
	STX_CATCH;
}
void STX_Service::CloseAudio(){
	STX_TRY;
	if(s_pAudio) delete s_pAudio;s_pAudio=0;
	STX_CATCH;}
void STX_Service::CloseTimer(){
	STX_TRY;
	if(s_pTimer) delete s_pTimer;s_pTimer=0;
	STX_CATCH;}

void STX_Service::CloseAll()
{
	STX_TRY;
	STX_Service::CloseWindow();
	//IRenderer::CloseRenderer();
	STX_Service::CloseTimer();
	STX_Service::CloseAudio();
	STX_Service::CloseInput();
	STX_CATCH;
}
Uint32 STX_Service::GetDelay()
{
	static Uint32 time0=STX_Service::GetTimerInstance()->stx_GetTicks(0);
	Uint32 time1=STX_Service::GetTimerInstance()->stx_GetTicks(0);
	Uint32 delay=time1-time0;
	time0=time1;
	return delay;
}
void STX_Service::PrintDelay(const char* file, const char* function, int line)
{
	LOG_PRINT_X("Delay %d %s:%s:%d\n",STX_Service::GetDelay(),file,function,line);
}
/*static*/ IWindow * STX_Service::GetWindowInstance(tWindow a )
{
	if (STX_Service::s_pWindow == 0)
	{
#if defined(WIN_PHONE_APP) || defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
		STX_Service::s_pWindow = new DummyWindow();
#elif defined(__MINGW32__) || defined(__MINGW64__)
		STX_Service::s_pWindow = new MWWindow();
#elif defined(SDL_2_0_5_BUILD)
		STX_Service::s_pWindow = new SDL2_0Window();
#elif defined(SDL_1_2_15_BUILD)
		STX_Service::s_pWindow = new SDL1_2Window();
#else
		STX_Service::s_pWindow = new DummyWindow();
#endif
	}
	return STX_Service::s_pWindow;
}

#if defined(SDL_1_2_15_BUILD)
class SDLAudio : public DummyAudio
{
	public:

struct sample {
    Uint8 *data;
    Uint32 dlen;
	Uint8* data0;
    Uint32 dlen0;
    Uint32 dpos;
	std::string name;bool done;
	sample(){data=0;
    dlen=0;

    data0=0;
    dlen0=0;
    dpos=0;
	name="";done=false;}
	sample(const sample& rhs)
	{
		data=rhs.data;
    		dlen=rhs.dlen;
    data0=rhs.data0;
    		dlen0=rhs.dlen0;
    		dpos=rhs.dpos;
		name=rhs.name;
		done=rhs.done;
	}
	~sample()
	{
		name.clear();//frees();
	}
	void frees()
	{
		if(data0)
		{
/*

*** glibc detected *** ./IndieLibexe/vc2008/IndieLib-TankEnemiesParallax_2008_d.bin: double free or corruption (!prev): 0x08fcc008 ***
*/
			stx_free(data0);
			data0=0;
		}
	}
};
static std::vector<sample> sounds;
static void mixaudio(void *unused, Uint8 *stream, int len)
{
    if ( !sounds.size() )
        return;
    if (!stream)
        return;
    if (len <= 0)
        return;
	//SDL_memset(stream, 0, len);

    int i;
    Uint32 amount;

    for ( i=0; i<sounds.size(); ++i ) {
        amount = (sounds[i].dlen-sounds[i].dpos);
        if ( amount > len ) {
            amount = len;
        }
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
        sounds[i].dpos += amount;
	sounds[i].dlen -= amount;
    }
}

/*
RGB8.cpp:53: error: argument of type ‘void (SDLAudio::)(void*, Uint8*, int)’ does not match ‘void (*)(void*, Uint8*, int)’
*/

	SDLAudio():DummyAudio()
{
	//LOG_PRINT_X("SDLAudio()\n");
}
	virtual ~SDLAudio(){FreeSounds();SDL_CloseAudio();}
	virtual bool Play(const char* name, int loopCount = 1)
{PlaySound((char*)name);return true;}
	virtual bool Load(const char* filename, const char* name){LoadSound((char*)filename,(char*)name);return true;}





void LoadSound(const char* afile, const char* name)
{
std::string f=std::string(afile);
std::string file=stx_convertpath(f);
if(!stx_fileExists(file.c_str())) return;
static bool init=true;
if(init)
{
init=false;

    //extern void mixaudio(void *unused, Uint8 *stream, int len);
    SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = mixaudio;
/*

RGB8.cpp:53: error: argument of type ‘void (SDLAudio::)(void*, Uint8*, int)’ does not match ‘void (*)(void*, Uint8*, int)’
*/
    fmt.userdata = 0;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, 0) < 0 ) {
        LOG_PRINT_X("Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);

    //PlaySound("$HOME/IndieLib0-31.1.2012-16.30/IndieLibexe/resources/IndieLib-TankEnemies/sounds/bigexplode.wav");
}
    int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;
#if 0

    /* Look for an empty (or finished) sound slot */
    for ( index=0; index<sounds.size(); ++index ) {
        if ( sounds[index].dpos == sounds[index].dlen ) {
            break;

        }
    }



    if (
#endif
SDL_LockAudio();
{
	int iiii=sounds.size();
	
	//sounds.resize(sounds.size()+1);
	{
	sample sss;
	sounds.push_back(sss);
	}
	
int index2 = sounds.size()-1;
sounds[index2].name = name;
}
SDL_UnlockAudio();
	index = sounds.size()-1;
        //return;
//LOG_PRINT_X("SDLAudio:%s:%s\n", file.c_str(), name);

    /* Load the sound file and convert it to 16-bit stereo at 22kHz */
    if ( SDL_LoadWAV(file.c_str(), &wave, &data, &dlen) == 0 ) {
        LOG_PRINT_X("Couldn't load %s: %s\n", file.c_str(), SDL_GetError());
        return;
    }

	//cvt.buf=&sounds[index].data0[0];//(Uint8 *)stx_malloc
	SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
                            AUDIO_S16,   2,             22050);
    cvt.buf=(Uint8 *)stx_malloc(dlen*cvt.len_mult);
    stx_memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;

    SDL_ConvertAudio(&cvt);


    SDL_FreeWAV(data);// ??? Heap corruption revelation


#if 0
/* Put the sound data in the slot (it starts playing immediately) */
    if ( sounds[index].data0 ) {
        free(sounds[index].data0);

	sounds[index].data0=0;
    }*/
#endif

    SDL_LockAudio();

//    sounds[index].data = 0;
//    sounds[index].dlen = 0;
    sounds[index].data0 = cvt.buf;
    sounds[index].dlen0 = cvt.len_cvt;
//    sounds[index].dpos = 0;
//	sounds[index].name = stx_strdup(name);

    SDL_UnlockAudio();

//LOG_PRINT_X("%s\n",name);

}
void PlaySound(const char* name)
{

    int index;
    /* Look for an empty (or finished) sound slot */
    for ( index=0; index<sounds.size(); ++index ) {
        if ( stx_strcmp(sounds[index].name.c_str() , name)==0 ) {
            break;
        }
    }


    if ( index == sounds.size() )
        return;

//LOG_PRINT_X("%s\n",name);
#if 0



/* Put the sound data in the slot (it starts playing immediately) */
    if ( sounds[index].data ) {
        free(sounds[index].data);


	sounds[index].data=0;
	sounds[index].dlen = 0;
    }

#endif

    SDL_LockAudio();

    sounds[index].data = sounds[index].data0;

    sounds[index].dlen = sounds[index].dlen0;

    sounds[index].dpos = 0;

    SDL_UnlockAudio();

}
	static void FreeSounds()
	{
		for ( unsigned int i=0; i<sounds.size(); ++i )
        		sounds[i].frees();
	}
};
std::vector<SDLAudio::sample> SDLAudio::sounds;
#elif defined(SDL_2_0_5_BUILD)
class SDLAudio2 : public DummyAudio
{
	public:

struct sample {
    Uint8 *data;
    Uint32 dlen;
	Uint8* data0;
    Uint32 dlen0;
    Uint32 dpos;
	std::string name;
	bool done;
	sample(){data=0;
    dlen=0;

    data0=0;
    dlen0=0;
    dpos=0;
	name="";done=false;}
	sample(const sample& rhs)
	{
		data=rhs.data;
    		dlen=rhs.dlen;
    data0=rhs.data0;
    		dlen0=rhs.dlen0;
    		dpos=rhs.dpos;
		name=rhs.name;
		done=rhs.done;
	}
	~sample()
	{
		name.clear();
	}
	void frees()
	{
		if(data0)
		{
			stx_free(data0);
			data0=0;
		}
	}
};
static std::vector<sample> sounds;
static void mixaudio(void *unused, Uint8 *stream, int len)
{
    if ( !sounds.size() )
        return;
    if (!stream)
        return;
    if (len <= 0)
        return;

    SDL_memset(stream, 0, len);

    int i;
    Uint32 amount;

    for ( i=0; i<sounds.size(); ++i ) {

        if (sounds[i].dlen <= 0)
		continue;

        amount = (sounds[i].dlen-sounds[i].dpos);
        if ( amount > len ) {
            amount = len;
        }
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
        sounds[i].dpos += amount;
	sounds[i].dlen -= amount;
    }
}

	SDLAudio2():DummyAudio()
{
	LOG_PRINT_X("SDLAudio2()\n");
}
	virtual ~SDLAudio2(){FreeSounds();SDL_CloseAudio();}
	virtual bool Play(const char* name, int loopCount = 1)
{PlaySound((char*)name);return true;}
	virtual bool Load(const char* filename, const char* name){LoadSound((char*)filename,(char*)name);return true;}





void LoadSound(const char* afile, const char* name)
{
std::string f=std::string(afile);
std::string file=stx_convertpath(f);
if(!stx_fileExists(file.c_str())) return;
static bool init=true;
if(init)
{
init=false;

    SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = mixaudio;

    fmt.userdata = 0;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, 0) < 0 ) {
        LOG_PRINT_X("Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);

}
    int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

SDL_LockAudio();
{
	int iiii=sounds.size();
	
	{
	sample sss;
	sounds.push_back(sss);
	}
	
int index2 = sounds.size()-1;
sounds[index2].name = name;
}
SDL_UnlockAudio();
	index = sounds.size()-1;

    /* Load the sound file and convert it to 16-bit stereo at 22kHz */
    if ( SDL_LoadWAV(file.c_str(), &wave, &data, &dlen) == 0 ) {
        LOG_PRINT_X("Couldn't load %s: %s\n", file.c_str(), SDL_GetError());
        return;
    }

	SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
                            AUDIO_S16,   2,             22050);
    cvt.buf=(Uint8 *)stx_malloc(dlen*cvt.len_mult);
    stx_memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;

    SDL_ConvertAudio(&cvt);


    SDL_FreeWAV(data);

    SDL_LockAudio();
    sounds[index].data0 = cvt.buf;
    sounds[index].dlen0 = cvt.len_cvt;
    SDL_UnlockAudio();

}
void PlaySound(const char* name)
{
    int index;
    /* Look for an empty (or finished) sound slot */
    for ( index=0; index<sounds.size(); ++index ) {
        if ( stx_strcmp(sounds[index].name.c_str() , name)==0 ) {
            break;
        }
    }

    if ( index == sounds.size() )
        return;

    SDL_LockAudio();

    sounds[index].data = sounds[index].data0;

    sounds[index].dlen = sounds[index].dlen0;

    sounds[index].dpos = 0;

    SDL_UnlockAudio();

}
	static void FreeSounds()
	{
		for ( unsigned int i=0; i<sounds.size(); ++i )
        		sounds[i].frees();
	}
};
std::vector<SDLAudio2::sample> SDLAudio2::sounds;
#endif
#if defined(WIN_PHONE_APP) || defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
// || defined(__MINGW32__) || defined(__MINGW64__)
// || defined(SDL_2_0_5_BUILD)
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
class STXAPI OpenALAudio : public IAudio
{
protected:
struct sample {
	std::string name;
	sample(const char* aname){name=aname;}
	int channel,sampleRate,bps,size;
	char* data;

	ALuint source;
	unsigned int bufferid;
};
	std::vector<sample> sounds;
	ALCcontext* context;
	ALCdevice* device;
public:
	OpenALAudio():IAudio()
{
	LOG_PRINT_X("OpenALAudio()\n");
		//data=loadWAV(bigexplode,channel,sampleRate,bps,size);
            device=alcOpenDevice(0);
            if(device==0)
            {
                    LOG_PRINT_X("cannot open sound card\n");

            }
            context=alcCreateContext(device,0);
            if(context==0)
            {
                    LOG_PRINT_X("cannot open context\n");

            }
            alcMakeContextCurrent(context);
}
	virtual ~OpenALAudio();
public:
	//-------------------------------------------------
	// Public Functions for this Class
	//-------------------------------------------------
	virtual bool				Init			(int  maxchannels = 64, FMOD_INITFLAGS  flags = FMOD_INIT_NORMAL){return true;}
	virtual bool				Load			(const char* szFilename, const char* szName);
	virtual void				Update			( ){}
	virtual bool				Play			(const char* szName, int loopCount = 1);
	virtual void				Stop			(const char* szName){}
	virtual void				StopAll			( ){}
	virtual bool				IsPlaying		(const char* szName){return false;}
	virtual int					GetNumSounds	( ){return 0;}
};

    int convertToInt(char* buffer,int len)
    {
            int a=0;
            if(!stx_isBigEndian())
                    for(int i=0;i<len;i++)
                            ((char*)&a)[i]=buffer[i];
            else
                    for(int i=0;i<len;i++)
                            ((char*)&a)[3-i]=buffer[i];
            return a;
    }

//char* loadWAV(const char* fn,int& chan,int& samplerate,int& bps,int& size)
bool OpenALAudio::Load(const char* szFilename, const char* szName)
{
	sounds.push_back(sample(szName));
	int i=sounds.size()-1;
            char buffer[4];
            std::ifstream in(stx_convertpath(szFilename).c_str(),std::ios::binary);
            in.read(buffer,4);
            if(strncmp(buffer,"RIFF",4)!=0)
            {
                    LOG_PRINT_X("this is not a valid WAVE file %s\n",szFilename);
                    return 0;
            }
            in.read(buffer,4);
            in.read(buffer,4);      //WAVE
            in.read(buffer,4);      //fmt
            in.read(buffer,4);      //16
            in.read(buffer,2);      //1
            in.read(buffer,2);
            sounds[i].channel=convertToInt(buffer,2);
            in.read(buffer,4);
            sounds[i].sampleRate=convertToInt(buffer,4);
            in.read(buffer,4);
            in.read(buffer,2);
            in.read(buffer,2);
            sounds[i].bps=convertToInt(buffer,2);
            in.read(buffer,4);      //data
            in.read(buffer,4);
            sounds[i].size=convertToInt(buffer,4);
            sounds[i].data=new char[sounds[i].size];
            in.read(sounds[i].data,sounds[i].size);
		unsigned int format;
            alGenBuffers(1,&sounds[i ].bufferid);
            if(sounds[i ].channel==1)
            {
                    if(sounds[i ].bps==8)
                    {
                            format=AL_FORMAT_MONO8;
                    }else{
                            format=AL_FORMAT_MONO16;
                    }
            }else{
                    if(sounds[i ].bps==8)
                    {
                            format=AL_FORMAT_STEREO8;
                    }else{
                            format=AL_FORMAT_STEREO16;
                    }
            }
            alBufferData(sounds[i ].bufferid,format,sounds[i ].data,sounds[i ].size,sounds[i ].sampleRate);
	return true;
}
bool OpenALAudio::Play(const char* szName, int loopCount)
{


    int index;
	//LOG_PRINT_X("n0=%s\n",szName.c_str());
    /* Look for an empty (or finished) sound slot */
    for ( index=0; index<sounds.size(); ++index ) {
        //LOG_PRINT_X("%s\n",sounds[index].name.c_str());
	if ( stx_strcmp(sounds[index].name.c_str() , szName)==0 ) {
            break;
        }
    }


    if ( index == sounds.size() )
        return false;

	alGenSources(1, &sounds[index].source);
	alSourcei(sounds[index].source, AL_BUFFER, sounds[index].bufferid);
	alSourcePlay(sounds[index].source);
	//getchar();
	int error = alGetError();
	//if (error) LOG_PRINT_X("%s\n", alutGetErrorString(error));
	return false;
}



OpenALAudio::~OpenALAudio()
{

    int index;
	for ( index=0; index<sounds.size(); ++index )
	{
            alDeleteSources(1,&sounds[index].source);
            alDeleteBuffers(1,&sounds[index].bufferid);

            delete[] sounds[index].data;
	}
	alcDestroyContext(context);
        alcCloseDevice(device);    
}
#endif
/*static*/ IAudio * STX_Service::GetAudioInstance(tAudio a )
{
	if (STX_Service::s_pAudio == 0)
	{
#if defined(WIN_PHONE_APP) || defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
// || defined(__MINGW32__) || defined(__MINGW64__)
		STX_Service::s_pAudio = new OpenALAudio();
#elif defined(SDL_1_2_15_BUILD)
		STX_Service::s_pAudio = new SDLAudio();
#elif defined(SDL_2_0_5_BUILD)
		STX_Service::s_pAudio = new SDLAudio2();
#else
		STX_Service::s_pAudio = new DummyAudio();
#endif
	}
	return STX_Service::s_pAudio;
}

#if 0
void ililuInit(bool flip)
{
	static int i=0;
	if(i!=0) return;
	i=1;

#if 1
	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		ILint test = ilGetInteger(IL_VERSION_NUM);
		/// wrong ImageLib version ///
		LOG_PRINT_X("Wrong ImageLib version. Old ImageLib.dll in system32/SysWow64?\n");
		//char* cErr_msg = (char *) err_msg.c_str();
		//abortGLInit(cErr_msg);
		DBG_HALT;
//		return -1;
	}
#endif
	// ImageLib initialization
	ilInit ();
	iluInit();
	//???ilutInit();
#if 0
	// Flip the image
	//if(flip)
	ilEnable (IL_ORIGIN_SET);
	//ilSetInteger(IL_IMAGE_ORIGIN,IL_ORIGIN_LOWER_LEFT);
	ilSetInteger(
		//IL_ORIGIN_MODE,
		IL_IMAGE_ORIGIN,
		//IL_ORIGIN_UPPER_LEFT
		IL_ORIGIN_LOWER_LEFT
		);
#elif 0
	ilDisable (IL_ORIGIN_SET);
#elif 0
	ilEnable (IL_ORIGIN_SET);
#elif 0
ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
ilEnable(IL_ORIGIN_SET);
#endif
}
#include <STX/d3dx9mathFromWine.h>

struct ilFormatString {
    const char *string;
	ILint e;
};
static const ilFormatString ilFormatStr[]={
	{"IL_COLOUR_INDEX",	0x1900},
	{"IL_COLOR_INDEX",	0x1900},
	{"IL_ALPHA", 		0x1906},
	{"IL_RGB", 		0x1907},
	{"IL_RGBA", 		0x1908},
	{"IL_BGR", 		0x80E0},
	{"IL_BGRA", 		0x80E1},
	{"IL_LUMINANCE", 	0x1909},
	{"IL_LUMINANCE_ALPHA",	0x190A},
	{0,0}
};
static const ilFormatString ilTypeStr[]={
	{"IL_BYTE", 		0x1400},
	{"IL_UNSIGNED_BYTE",	0x1401},
	{"IL_SHORT", 		0x1402},
	{"IL_UNSIGNED_SHORT",	0x1403},
	{"IL_INT",		0x1404},
	{"IL_UNSIGNED_INT",	0x1405},
	{"IL_FLOAT", 		0x1406},
	{"IL_DOUBLE", 		0x140A},
	{"IL_HALF", 		0x140B},
	{0,0}
};

const char *ilFormatToString(ILint format)
{
	for (unsigned int i = 0; i < elementsOf(ilFormatStr); i++){
		if (format == ilFormatStr[i].e) return ilFormatStr[i].string;
	}
	return 0;
}
const char *ilTypeToString(ILint format)
{
	for (unsigned int i = 0; i < elementsOf(ilTypeStr); i++){
		if (format == ilTypeStr[i].e) return ilTypeStr[i].string;
	}
	return 0;
}
bool hasAlpha[]={
	0,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	0,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1
};

void Image3::loadFromMemory(void *mem, const FORMAT fmt, const int w, const int h, const int d, const int mipMapCount, bool ownsMemory){
	create(fmt, w, h, d, mipMapCount, 0);
/*
	if (ownsMemory){
		pixels = (unsigned char *) mem;
	} else {
		int size = getMipMappedSize(0, nMipMaps);
		pixels = new unsigned char[size];
		stx_memcpy(pixels, mem, size);
	}
*/
	setDataBuffer((ubyte*)mem);
}

void Image3::Dump()
{
	BindImage();
#if 0
	// Attributes
	LOG_PRINT("Width	=%d\n",ilGetInteger (IL_IMAGE_WIDTH));
	LOG_PRINT("Height	=%d\n",ilGetInteger (IL_IMAGE_HEIGHT));
	LOG_PRINT("Depth	=%d\n",ilGetInteger (IL_IMAGE_DEPTH));
	LOG_PRINT("FormatInt	=%d\n",ilGetInteger (IL_IMAGE_FORMAT));
	LOG_PRINT("Bpp		=%d\n",ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));

	LOG_PRINT("IL_IMAGE_FORMAT		=%s\n",ilFormatToString(ilGetInteger (IL_IMAGE_FORMAT)));
	LOG_PRINT("IL_IMAGE_TYPE		=%s\n",ilTypeToString(ilGetInteger (IL_IMAGE_TYPE)));


	LOG_PRINT("ComponentsCount=%d\n", getComponentsCount());
	LOG_PRINT("ArraySize	  =%d\n", getArraySize());
	if(is1D()) LOG_PRINT("Is 1D\n");
	else LOG_PRINT("Is not 1D\n");
	if(is2D()) LOG_PRINT("Is 2D\n");
	else LOG_PRINT("Is not 2D\n");
	if(is3D()) LOG_PRINT("Is 3D\n");
	else LOG_PRINT("Is not 3D\n");
	if(isCube()) LOG_PRINT("Is Cube\n");
	else LOG_PRINT("Is not Cube\n");
	if(isArray()) LOG_PRINT("Is Array\n");
	else LOG_PRINT("Is not Array\n");

	LOG_PRINT("Pointer	=%x\n",ilGetData ());


#endif
}


#if 1

#include <string.h>//??? QT_BUILD
const FORMAT getFormatFromString(char *string){
	for (unsigned int i = 0; i < elementsOf(formatStrings); i++){
#ifdef _WIN32
		if (stx_strcasecmp(string, formatStrings[i].string) == 0) return formatStrings[i].format;
#else
		if (strcmp(string, formatStrings[i].string) == 0) return formatStrings[i].format;
#endif
	}
	return FORMAT_NONE;
}
#endif

ILenum ilFormat[]={
	0     ,
	IL_LUMINANCE       ,
	IL_RGB      ,
	IL_RGB     ,
	IL_RGBA    ,
	IL_LUMINANCE      ,
	IL_LUMINANCE_ALPHA     ,
	IL_RGB    ,
	IL_RGBA   ,
	IL_LUMINANCE      ,
	IL_RGB     ,
	IL_RGB    ,
	IL_RGBA   ,
	IL_LUMINANCE     ,
	IL_LUMINANCE_ALPHA    ,
	IL_RGB   ,
	IL_RGBA  ,
	IL_LUMINANCE     ,
	IL_LUMINANCE_ALPHA    ,
	IL_RGB   ,
	IL_RGBA  ,
	IL_LUMINANCE     ,
	IL_LUMINANCE_ALPHA    ,
	IL_RGB   ,
	IL_RGBA  ,
	IL_LUMINANCE     ,
	IL_LUMINANCE_ALPHA    ,
	IL_RGB   ,
	IL_RGBA  ,
	IL_LUMINANCE     ,
	IL_LUMINANCE_ALPHA    ,
	IL_RGB   ,
	IL_RGBA  ,
	IL_LUMINANCE    ,
	IL_LUMINANCE_ALPHA   ,
	IL_RGB  ,
	IL_RGBA ,
	IL_LUMINANCE    ,
	IL_LUMINANCE_ALPHA   ,
	IL_RGB  ,
	IL_RGBA ,
	0    ,
	0   ,
	0 ,

	0   ,
	0    ,
	0  ,






	0     ,
	0     ,
	0     ,
	0    ,
	0    ,
	0     ,
	0   ,
	0    ,
	0  ,
	0  ,
	0   ,
	0 ,
};
ILenum ilType[]={
	0     ,
	IL_BYTE       ,
	IL_BYTE      ,
	IL_BYTE     ,
	IL_BYTE    ,
	IL_SHORT      ,
	IL_SHORT     ,
	IL_SHORT    ,
	IL_SHORT   ,
	IL_BYTE      ,
	IL_BYTE     ,
	IL_BYTE    ,
	IL_BYTE   ,
	IL_SHORT     ,
	IL_SHORT    ,
	IL_SHORT   ,
	IL_SHORT  ,
	IL_HALF     ,
	IL_HALF    ,
	IL_HALF   ,
	IL_HALF  ,
	IL_FLOAT     ,
	IL_FLOAT    ,
	IL_FLOAT   ,
	IL_FLOAT  ,
	IL_SHORT     ,
	IL_SHORT    ,
	IL_SHORT   ,
	IL_SHORT  ,
	IL_INT     ,
	IL_INT    ,
	IL_INT   ,
	IL_INT  ,
	IL_UNSIGNED_SHORT    ,
	IL_UNSIGNED_SHORT   ,
	IL_UNSIGNED_SHORT  ,
	IL_UNSIGNED_SHORT ,
	IL_UNSIGNED_INT    ,
	IL_UNSIGNED_INT   ,
	IL_UNSIGNED_INT  ,
	IL_UNSIGNED_INT ,
	0    ,
	0   ,
	0 ,
	0   ,
	0    ,
	0  ,
	0     ,
	0     ,
	0     ,
	0    ,
	0    ,
	0     ,
	0   ,
	0    ,
	0  ,
	0  ,
	0   ,
	0 ,
};

std::map<std::string, ILuint> Image3::m_Image3_filecache;

ILuint Image3::find_from_Image3_cache(const char* fileName)
{
	std::map<std::string, ILuint>::iterator it;
	it = m_Image3_filecache.find(fileName);
	if (it != m_Image3_filecache.end())
		return it->second;
	return 0;
}

void Image3::insert_to_Image3_cache(const char* fileName, ILuint id)
{
	std::map<std::string, ILuint>::iterator it;
	it = m_Image3_filecache.find(fileName);
	if (it == m_Image3_filecache.end())
		m_Image3_filecache.insert ( std::pair<std::string,ILuint>(fileName, id) );
}

bool Image3::loadImageLibImage(const char *fileName0, const bool useMipMaps)
{

	#if 1
	std::string f=std::string(fileName0);
	std::string pName=stx_convertpath(f);
	stx_fileExists(pName.c_str());

	ILuint newimg;
	std::map<std::string, ILuint>::iterator it;
	it = m_Image3_filecache.find(pName.c_str());
	if (it != m_Image3_filecache.end())
		newimg=it->second;
	else
		ilGenImages (1, &newimg);

	imageId=newimg;
	BindImage();
	//ilBindImage (imageId);
#if 1
	/* if (stx_strcmp(pName.c_str(),"Future.png")==0)
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else */ 
	if(pName.substr(pName.find_last_of(".") + 1) == "dds")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "bmp")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "tga")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "DDS")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "BMP")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "TGA")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);

	ILboolean success = false;
	if (it == m_Image3_filecache.end())
	{
    	LOG_PRINT_X("Image3::loadImageLibImage:ilLoadImage=%s\n",pName.c_str());
	success = ilLoadImage(pName.c_str());
	}

#endif
	if(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL)==32)
		//ilConvertImage (IL_RGBA, ilGetInteger(IL_IMAGE_TYPE));
		format=FORMAT_RGBA8;
	else if(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL)==24)
		//ilConvertImage (IL_RGB, ilGetInteger(IL_IMAGE_TYPE));
		format=FORMAT_RGB8;
	else
	{
		format=FORMAT_RGB8;
		ilConvertImage (ilFormat[format], ilType[format]);
	}
	if(useMipMaps) iluBuildMipmaps();
#else
	if(0)//imageId)
	{
		ilDeleteImages(1, &imageId);
		//iluDeleteImage (imageId);
		//imageId=0;
	}
	ilGenImages (1, &imageId);
	//std::string name2=stx_convertpath(fileName0);
	//stx_fileExists(name2.c_str());
	BindImage();
	char fileName2[MAX_PATH];
	stx_strlcpy(fileName2,pName.c_str(),MAX_PATH);
		std::string fileloc=
			stx_strlwr(
			fileName2);
#if 1
	/* if (stx_strcmp(pName.c_str(),"Future.png")==0)
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else */ 
	if(pName.substr(pName.find_last_of(".") + 1) == "dds")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "bmp")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "tga")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "DDS")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "BMP")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else if(pName.substr(pName.find_last_of(".") + 1) == "TGA")
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	else
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
#endif
	LOG_PRINT_X("Image3::loadImageLibImage:ilLoadImage=%s\n",pName.c_str());
	ILboolean success = ilLoadImage(pName.c_str());
	if(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL)==32)
		//ilConvertImage (IL_RGBA, ilGetInteger(IL_IMAGE_TYPE));
		format=FORMAT_RGBA8;
	else if(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL)==24)
		//ilConvertImage (IL_RGB, ilGetInteger(IL_IMAGE_TYPE));
		format=FORMAT_RGB8;
	else
	{
		format=FORMAT_RGB8;
		ilConvertImage (ilFormat[format], ilType[format]);
	}
	iluBuildMipmaps();
#endif
	if (it == m_Image3_filecache.end())
		m_Image3_filecache.insert ( std::pair<std::string,ILuint>(pName.c_str(),newimg) );
	Dump();

	return true;
}
const ubyte * Image3::setAlpha(float3& colorkey)
{
	BindImage();
	ubyte* data0=(ubyte*)getPixels(0);
	//ubyte *data0 = (ubyte*)ilGetData();
	return data0;
	uint nChannels = getChannelCount(format);
	if(nChannels<4) addChannel(0.0f);
	nChannels = getChannelCount(format);
	if(nChannels<4) return data0;
	int nPixels = getPixelCount(0, getMipMapCount());
	int bpc = getBytesPerChannel(format);
	int cs = bpc * nChannels;
    	for (int i = 0; i < nPixels; i++)
	{
		if( data0[4*i+0] == (uint) (255.0f * saturate(colorkey.x)))
		if( data0[4*i+1] == (uint) (255.0f * saturate(colorkey.y)))
		if( data0[4*i+2] == (uint) (255.0f * saturate(colorkey.z))) 
        		data0[4*i+3] = (uint) (0.0f);
    		else
        		data0[4*i+3] =  (uint) (255.0f);
	}
	return data0;
}
bool Image3::saveImageLibImage(const char *fileName, const bool useMipMaps){

		if(!getPixels())
			return false;

	//ILuint imageId = 0;
	BindImage(); /* Generation of numTextures image names */

#if 0
	BYTE* data=getPixels(0);
		if(format==FORMAT_RGB8)
ilSetPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);

		else if(format==FORMAT_RGBA8)

ilSetPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGBA, IL_UNSIGNED_BYTE, data);

		else

		{

			convert(FORMAT_RGB8);

			data=getPixels();

			ilSetPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);

		}

#endif
	//LOG_FNLN_X
	Dump();

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(fileName);

	return true;
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
/* ---------------------------------------------- */
Image3::Image3():imageId(0){
	//ilGenImages (1, &imageId);
	format = (FORMAT)FORMAT_NONE;
	//BindImage();
	//LOG_FNLN_X
	//Dump();
}
Image3::Image3(const Image3 &img):imageId(0){
	format = img.format;

	ilGenImages (1, &imageId);
	BindImage();

	// Copy the new image
	if(img.imageId) ilCopyImage (img.imageId);

	//LOG_FNLN_X
	Dump();
}

Image3::~Image3(){
	if(0)//imageId)
	{
		ilDeleteImages(1, &imageId);
		//iluDeleteImage (imageId);
		imageId=0;
	}
}


void Image3::CopyPixels(ubyte* data)
{
	BindImage ();
int w=ilGetInteger(IL_IMAGE_WIDTH);
int h=ilGetInteger(IL_IMAGE_HEIGHT);
int d=ilGetInteger(IL_IMAGE_DEPTH);
//data=new ubyte[w*h*d*getChannelCount(format)];
ilCopyPixels(0, 0, 0,
w,h,d,
ilFormat[format], ilType[format], data);
}
	void Image3::setPixels (ubyte * data)
	{
		BindImage();
#if 0
		ilTexImage(
			ilGetInteger (IL_IMAGE_WIDTH),
			ilGetInteger (IL_IMAGE_HEIGHT),
			ilGetInteger (IL_IMAGE_DEPTH),
			ilGetInteger (IL_IMAGE_BITS_PER_PIXEL),
			ilFormat[format],
			ilType[format],
			data);
#else

ilSetPixels(0, 0, 0,
ilGetInteger(IL_IMAGE_WIDTH),
ilGetInteger(IL_IMAGE_HEIGHT),
ilGetInteger(IL_IMAGE_DEPTH),
 ilFormat[format], ilType[format], data);

#endif
}
	const ubyte* Image3::getPixels (int mipMapLevel) const
	{
		BindImage();
		int nmm=ilGetInteger(IL_NUM_MIPMAPS);

		ilActiveMipmap(mipMapLevel+1);
		ubyte * r=ilGetData();
		ilActiveMipmap(1);
if (mipMapLevel>nmm)
	r=0;
		return r;
	}
ubyte* Image3::create(const FORMAT fmt, const int w, const int h, const int d, const int mipMapCount, const int arraysize)
{

	if(0)//imageId)
	{
		ilDeleteImages(1, &imageId);
		//iluDeleteImage (imageId);
		//imageId=0;
	}

	ilGenImages (1, &imageId);

	BindImage();


#if 1
	// New image sizing
	//iluImageParameter (ILU_FILTER, ILU_NEAREST);
	iluScale (w, h, d);//getChannelCount(fmt));

	ilConvertImage (ilFormat[fmt], ilType[fmt]);

#if 0
	std::vector<ubyte> Data(w*h*d*getChannelCount(fmt));

	Data.clear();

	ilSetPixels(0,0,0, w,h,d, ilFormat[fmt], ilType[fmt], &Data[0]);

#endif
#else
	ilNewImageFull(w,h,d, getChannelCount(fmt), ilFormat[fmt], ilType[fmt], 0);
#endif
	//LOG_FNLN_X
	Dump();

    	format = fmt;

	//iluBuildMipmaps();

	return ilGetData();
}
void Image3::removeChannels(bool keepCh0, bool keepCh1, bool keepCh2, bool keepCh3)
{
	BindImage();
	int chc=0;
	if (keepCh0) chc++;
	if (keepCh1) chc++;
	if (keepCh2) chc++;
	if (keepCh3) chc++;
	ILenum fmt=IL_RGBA;
	switch(chc)
	{
		case 1:
			fmt=IL_LUMINANCE;
			break;
		case 2:
			fmt=IL_LUMINANCE_ALPHA;
			break;
		case 3:
			fmt=IL_RGB;
			break;
		case 4:
			fmt=IL_RGBA;
			break;
	};
	ilConvertImage (fmt, ilGetInteger(IL_IMAGE_TYPE));
}

void Image3::addChannel(const float value)
{
	BindImage();

#if 0
	int nc=ilGetInteger(IL_IMAGE_CHANNELS);
	if(nc<4)
		ilSetInteger(IL_IMAGE_CHANNELS, nc+1);

	if(nc==3)//???
		ilSetAlpha(value);
#else
	uint nChannels = getChannelCount(format);
	if (isCompressedFormat(format) || nChannels == 4) return;
	uint val;
	if (format <= FORMAT_RGBA8){
		val = (uint) (255.0f * saturate(value));
	} else if (format <= FORMAT_RGBA16){
		val = (uint) (65535.0f * saturate(value));
	} else if (format <= FORMAT_RGBA16f){
		val = half(value).sh;
	} else {
		val = *(unsigned int *) &value;
	}
	int nPixels = getPixelCount(0, getMipMapCount());
	int bpc = getBytesPerChannel(format);
	int cs = bpc * nChannels;

    	std::vector<ubyte> newPixels(nPixels * (nChannels + 1) * bpc);
	ubyte *dest = &newPixels[0];
	ubyte *src = ilGetData();
	for (int i = 0; i < nPixels; i++){

		stx_memcpy(dest, src, cs);
		dest += cs;
		src += cs;

		stx_memset(dest, val, bpc);//???
		dest += bpc;
	}
	format = (FORMAT) (format + 1);
	ilConvertImage (ilFormat[format], ilType[format]);

ilSetPixels(0, 0, 0,
ilGetInteger(IL_IMAGE_WIDTH),
ilGetInteger(IL_IMAGE_HEIGHT),
ilGetInteger(IL_IMAGE_DEPTH),
 ilFormat[format], ilType[format], &newPixels[0]);



#endif
}

bool Image3::convert(const FORMAT newFormat)
{
	BindImage();
#if 0
	uint nChannels1 = getChannelCount(format);
	uint nChannels2 = getChannelCount(newFormat);
	uint hasAlpha1 = hasAlpha[format];
	uint hasAlpha2 = hasAlpha[newFormat];
	if((nChannels2>nChannels1)&&hasAlpha2&&(!hasAlpha1))
		for(unsigned int i=0;i<(nChannels2-nChannels1);i++)
			addChannel();
	else //if(nChannels2<nChannels1)
	/*{
		removeChannels(keepCh0, keepCh1, keepCh2, keepCh3);
	}*/
#endif
		ilConvertImage (ilFormat[newFormat], ilType[newFormat]);
        //if(newFormat==FORMAT_RGBA8) ilSetAlpha(1.0f);
        format=newFormat;
	return true;
}
int Image3::getMipMapCountFromDimesions() const {

	BindImage();
	int width=ilGetInteger(IL_IMAGE_WIDTH);
	int height=ilGetInteger(IL_IMAGE_HEIGHT);
	int depth=ilGetInteger(IL_IMAGE_DEPTH);


	int m = max(width, height);
	m = max(m, depth);
	int i = 0;
	while (m > 0){
		m >>= 1;
		i++;
	}
	return i;
}
int Image3::getPitch(int ml) const {
#if 0
	int bpp=getBytesPerPixel(format);
	int m = max(width, height);
	int w = width;
	m = max(m, depth);
	int i = 0;
	while ((i<ml)&&(m > 0)){
		m >>= 1;
		w >>= 1;
		i++;
	}
	return w*bpp;
#else
	int pitch = 0;
								int slicePitch=0;
				uint nMipMaps = getMipMapCount();
				uint nSlices = isCube()? 6 : 1;
				uint arraySize = getArraySize();
				//static D3D10_SUBRESOURCE_DATA texData[1024];
				//D3D10_SUBRESOURCE_DATA *dest = texData;
				//for (uint n = 0; n < arraySize; n++)
				{
				//	for (uint k = 0; k < nSlices; k++)
					{
						for (uint i =
ml//0
; i <
ml+1//nMipMaps
; i++)
								  {
							if (isCompressedFormat(format))
							{
								pitch = ((getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
								//slicePitch = pitch * ((img->getHeight(i) + 3) >> 2);
							}
							else
							{
								pitch = getWidth(i) * getBytesPerPixel(format);
								//slicePitch = pitch * img->getHeight(i);
							}
/*
							dest->pSysMem = img->getPixels(i, n) + k * slicePitch;
							dest->SysMemPitch = pitch;
							dest->SysMemSlicePitch = slicePitch;
							dest++;
*/
						}
					}
				}
				return pitch;
#endif
}

int Image3::getMipMappedSize(const int firstMipMapLevel, int nMipMapLevels, FORMAT srcFormat) const {
	int w = getWidth (firstMipMapLevel);
	int h = getHeight(firstMipMapLevel);
	int d = getDepth (firstMipMapLevel);
	if ((srcFormat) == FORMAT_NONE) (srcFormat) = format;
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
	return (getDepth () == 0)? 6 * size : size;
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
	return (getDepth () == 0)? 6 * size : size;
}
#elif 0
//bool Image3::m_inited=false;
unsigned int Image3::s_id=0;
std::map<std::string, unsigned int> Image3::m_Image3_filecache;

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

		m_Image3_filecache.insert ( std::pair<std::string,unsigned int>(fileName, id) );
}
#endif
#if 0
		STXString::STXString(const int n):std::string("")
		{
			//
		if(n>0) for(unsigned int i=0;i<n;i++)std::string::append(" ");}
		STXString::STXString():std::string("")
		{
			//
		}

		STXString::STXString(const char* str) : std::string(str)

		{
			//

		}
		STXString::STXString(char* str) : std::string(str)

		{
			//

		}
/*
		STXString::operator const char *() const
		{

		return c_str();}*/

		STXString::STXString(std::string& str):std::string(str)
		{
			//
		//(*this)="";if(str.length())append(str.c_str());
}
		STXString::STXString(const STXString& rhs):std::string(rhs.c_str())
		{
			//
		//(*this)="";if(rhs.length())append(rhs.c_str());
}
/*
		STXString& STXString::operator=(const STXString& rhs)
		{

		(*this)="";if(rhs.length())append(rhs.c_str());}
*/
		STXString& STXString::operator=(const int& rhs)
		{
			//
		(*this)="";
		return *this;
}


		void STXString::tolower()

		{
			//

			if(!length())return;
			std::string tmp=c_str();
			for(unsigned int i=0;i<length();i++)
				tmp[i]=STX_tolower(*(tmp.c_str()+i));
			*this=tmp;

		}
		void STXString::toupper()

		{
			//

			if(!length())return;
			std::string tmp=c_str();
			for(unsigned int i=0;i<length();i++)
				tmp[i]=STX_toupper(*(tmp.c_str()+i));
			*this=tmp;
		}
		//virtual
		STXString::~STXString()

		{
			//

			std::string::clear();
		}

	const int STXString::count(const char c) const

		{
			//

		if(!length())return 0;
		int cc=0;
		for(unsigned int i=0;i<length();i++)
			if(c==*(c_str()+i)) cc++;
		return cc;
	}

	const STXString STXString::find(const STXString& what) const

		{
			//

		if(length()&&(strstr(c_str(), what.c_str())))
			return STXString(strstr(c_str(), what.c_str()));
		return "";
	}
	const STXString STXString::after(const STXString& what) const

		{
			//

		if(length()&&(strstr(c_str(), what.c_str())))
			return STXString(strstr(c_str(), what.c_str())+stx_strlen(what.c_str()));
		return "";
	}
	const STXString STXString::before(const STXString& what) const
		{
			//

		if(!length()) return "";

		char* c=const_cast<char*>(strstr(c_str(), what.c_str()));
		if(c)
		{
			int n=int(c-c_str())+1;

			std::string buf;
			buf.resize(n);

			for(unsigned int i=0;i<n-1;i++)
				buf[i]=c_str()[i];
			buf[n-1]='\0';
			STXString ret(buf);

			return ret;
		}


		return "";
	}
		char* STXString::str() const
		{
			//
		return const_cast<char*>(c_str());}



void STXString::replace(const STXString& afrom,
const STXString& ato) {

const std::string from=afrom.c_str();
const std::string to=ato.c_str();
std::string context=*this;
size_t lookHere = 0;
size_t foundHere;
while((foundHere = context.find(from, lookHere))
!= string::npos) {
context.replace(foundHere, from.size(), to);
lookHere = foundHere + to.size();
}
	*this="";
	append(context.c_str());
}

//include "IRenderer"
const STXString& STXString::operator+=(const STXString& s)
{
	//
	append(s.c_str());
	return *this;
}
#endif

#if !defined(__MINGW32__) && !defined(__MINGW64__)

#ifdef _MSC_VER
/* The first (low-resolution) ticks value of the application */
 __DWORD__
#else
/* The first ticks value of the application */
#ifdef HAVE_CLOCK_GETTIME
struct timespec
#else
struct timeval
#endif /* HAVE_CLOCK_GETTIME */
#endif
		m_dwstart;


#if !defined(__APPLE__)
#if 0
STX_Dummy_TIMER::STX_Dummy_TIMER():STX_TIMER(){}
STX_Dummy_TIMER::~STX_Dummy_TIMER(){}
Uint32 STX_Dummy_TIMER::stx_GetTicks(STX_TIMER* _Timer){}
void STX_Dummy_TIMER::stx_Delay(Uint32 ms,STX_TIMER* _Timer){}
#endif

#include <stdio.h>
#ifndef _MSC_VER
#include <sys/time.h>
#include <unistd.h>
/* The clock_gettime provides monotonous time, so we should use it if
   it's available. The clock_gettime function is behind ifdef
   for __USE_POSIX199309
   Tuomas Kyntola (tuomas.kyntola@ray.fi) 27/09/2005
*/
#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
#endif

/* The first ticks value of the application */
#ifdef HAVE_CLOCK_GETTIME
static struct timespec start;
#else
static struct timeval start;
#endif /* HAVE_CLOCK_GETTIME */



#elif !defined(__MINGW32__) && !defined(__MINGW64__)
#include <mmsystem.h>
#define TIME_WRAP_VALUE	(~(__DWORD__)0)
#ifdef _MSC_VER
/* The first (low-resolution) ticks value of the application */
static __DWORD__ start;
#else
/* The first ticks value of the application */
#ifdef HAVE_CLOCK_GETTIME
static struct timespec start;
#else
static struct timeval start;
#endif /* HAVE_CLOCK_GETTIME */
#endif
#ifndef USE_GETTICKCOUNT
/* Store if a high-resolution performance counter exists on the system */
static BOOL hires_timer_available;
/* The first high-resolution ticks value of the application */
static LARGE_INTEGER hires_start_ticks;
/* The number of ticks per second of the high-resolution performance counter */
static LARGE_INTEGER hires_ticks_per_second;

#endif


//include "SDL_timer.h"
#endif
#include <errno.h>

#ifdef _MSC_VER
/* The first (low-resolution) ticks value of the application */
 __DWORD__
#else
/* The first ticks value of the application */
#ifdef HAVE_CLOCK_GETTIME
struct timespec
#else
struct timeval
#endif /* HAVE_CLOCK_GETTIME */
#endif
STXSTX_StartTicks(void)
{
#ifndef _MSC_VER
    /* Set first ticks value */
#ifdef HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_MONOTONIC, &start);
#else
    gettimeofday(&start, 0);
#endif
#elif !defined(__MINGW32__) && !defined(__MINGW64__)

    /* Set first ticks value */
#ifdef USE_GETTICKCOUNT
    start = GetTickCount();
#else
#if 0                           /* Apparently there are problems with QPC on Win2K */
    if (QueryPerformanceFrequency(&hires_ticks_per_second) == TRUE) {
        hires_timer_available = TRUE;
        QueryPerformanceCounter(&hires_start_ticks);
    } else
#endif
    {
        hires_timer_available = FALSE;
        timeBeginPeriod(1);     /* use 1 ms timer precision */
        start = STX_Service::GetTimerInstance()->stx_GetTicks(0);
    }
#endif
#endif
	return start;
}

Uint32
STXSTX_GetTicks(void)
{
#ifndef _MSC_VER
#if HAVE_CLOCK_GETTIME
    Uint32 ticks;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    ticks =
        (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec -
                                              start.tv_nsec) / 1000000;
    return (ticks);
#elif !defined(__MINGW32__) && !defined(__MINGW64__)

    Uint32 ticks;
    struct timeval now;

    gettimeofday(&now, 0);
    ticks =
        (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec -
                                              start.tv_usec) / 1000;
    return (ticks);
#endif
#else
	    __DWORD__ now, ticks;
#ifndef USE_GETTICKCOUNT
    LARGE_INTEGER hires_now;
#endif

#ifdef USE_GETTICKCOUNT
    now = GetTickCount();
#else
    if (hires_timer_available) {
        QueryPerformanceCounter(&hires_now);

        hires_now.QuadPart -= hires_start_ticks.QuadPart;
        hires_now.QuadPart *= 1000;
        hires_now.QuadPart /= hires_ticks_per_second.QuadPart;

        return (__DWORD__) hires_now.QuadPart;
    } else {
        now = STX_Service::GetTimerInstance()->stx_GetTicks(0);
    }
#endif

    if (now < start) {
        ticks = (TIME_WRAP_VALUE - start) + now;
    } else {
        ticks = (now - start);
    }
    return (ticks);
#endif
}

#ifdef STX_HAS_64BIT_TYPE
Uint64
STXSTX_GetPerformanceCounter(void)
{
#ifndef _MSC_VER
#if HAVE_CLOCK_GETTIME
    Uint64 ticks;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    ticks = now.tv_sec;
    ticks *= 1000000000;
    ticks += now.tv_nsec;
    return (ticks);
#else
    Uint64 ticks;
    struct timeval now;

    gettimeofday(&now, 0);
#if 1//!defined(FALSE_ANDROID)
    ticks = now.tv_sec;
    ticks *= 1000000;
    ticks += now.tv_usec;
#endif
    return (ticks);
#endif
	#else
    LARGE_INTEGER counter;

    if (!QueryPerformanceCounter(&counter)) {
        return SDL_GetTicks();
    }
    return counter.QuadPart;
#endif
}

Uint64
STXSTX_GetPerformanceFrequency(void)
{
#if HAVE_CLOCK_GETTIME
    return 1000000000;
#else
    return 1000000;
#endif
}
#endif
void
STXSTX_Delay(Uint32 ms)
{
    int was_error;

#if HAVE_NANOSLEEP
    struct timespec elapsed, tv;
#else
    struct timeval tv;
    Uint32 then, now, elapsed;
#endif

    /* Set the timeout interval */
#if HAVE_NANOSLEEP
    elapsed.tv_sec = ms / 1000;
    elapsed.tv_nsec = (ms % 1000) * 1000000;
#else
    then = STXSTX_GetTicks();
#endif
    do {
        errno = 0;

#if HAVE_NANOSLEEP
        tv.tv_sec = elapsed.tv_sec;
        tv.tv_nsec = elapsed.tv_nsec;
        was_error = nanosleep(&tv, &elapsed);
#else
        /* Calculate the time interval left (in case of interrupt) */
        now = STXSTX_GetTicks();
        elapsed = (now - then);
        then = now;
        if (elapsed >= ms) {
            break;
        }
        ms -= elapsed;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;

        was_error = select(0, 0, 0, 0, &tv);
#endif /* HAVE_NANOSLEEP */
    } while (was_error && (errno == EINTR));
}
void STX_Dummy_TIMER::Reset(){start=m_dwstart;}
STX_Dummy_TIMER::STX_Dummy_TIMER():STX_TIMER(){m_dwstart=STXSTX_StartTicks();}
STX_Dummy_TIMER::~STX_Dummy_TIMER(){}
Uint32 STX_Dummy_TIMER::stx_GetTicks(STX_TIMER* _Timer){return STXSTX_GetTicks();}
void STX_Dummy_TIMER::stx_Delay(Uint32 ms,STX_TIMER* _Timer){STXSTX_Delay(ms);}
#endif
#endif

std::string STX_Service::LoadTxtFile_(const std::string aFileName, std::string& file)
{
	file="";
	std::string f;
	f.append(aFileName);
	std::string fn=stx_convertpath(f);
  std::string line;
  file.clear();
  std::ifstream infile (fn.c_str(), std::ios_base::in);
  while (getline(infile, line, '\n'))
  {
    file.append (line);
  }
  //std::cout << "Read " << file.size() << " lines.\n";
  return file;
}
void STX_Service::LoadTxtFile(const std::string aFileName, std::vector<std::string>& file)
{
	file.clear();
	std::string f;
	f.append(aFileName);
	std::string fn=stx_convertpath(f);
  std::string line;
  file.clear();
  std::ifstream infile (fn.c_str(), std::ios_base::in);
  while (getline(infile, line, '\n'))
  {
    file.push_back (line);
  }
  //std::cout << "Read " << file.size() << " lines.\n";
}
std::string STX_Service::LoadTxtFile(const std::string aFileName, std::string& contents)
{
	std::string fn=stx_convertpath(aFileName);
	std::ifstream in(fn.c_str(), std::ios::in | std::ios::binary);
  	if (in)
  	{
    		contents=(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
  	}
	//LOG_PRINT("contents:\n\"%s\"\n", contents.c_str());
  	return contents;
}

void STX_Service::WriteTxtFile(const char* aFileName, const char* contents)
{
	std::string fn=aFileName;//stx_convertpath(aFileName);
	ofstream myfile;
	myfile.open (fn.c_str(), fstream::in | fstream::out | fstream::app);
	myfile << contents;
	myfile << "\n";
	myfile.close();
}
std::vector<BYTE> STX_Service::ReadFile(const char* fn_)
{
	std::string fn=stx_convertpath(fn_);
    // open the file:
    std::ifstream file(fn.c_str(), std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<BYTE> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<BYTE>(file),
               std::istream_iterator<BYTE>());

    return vec;
}

