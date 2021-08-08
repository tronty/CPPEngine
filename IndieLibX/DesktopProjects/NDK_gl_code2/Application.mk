# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# APP_STL := stlport_static 

#APP_ABI := armeabi armeabi-v7a x86 # works on emulator
#APP_ABI := armeabi # does not work on emulator
APP_ABI := mips armeabi x86 # works
#APP_ABI := x86
#APP_STL := stlport_static

NDK_TOOLCHAIN_VERSION=4.8
APP_STL := gnustl_static
