/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include  <STX/STX.h>

#if 1
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

//include <fstream>
//if defined(__cplusplus) && !defined(ANDROID)
#include <string>
//endif
//include <vector>
using namespace std;

#if !defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
#include <string.h>
#endif

std::string STXAPIENTRY stx_GetFileExtension(const std::string& FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}

std::string STXAPIENTRY stx_GetFileName(const std::string& aFileName)
{
	std::string FileName=aFileName;
	replaceAll(FileName, "\\", "/");
    if(FileName.find_last_of("/") != std::string::npos)
        return FileName.substr(FileName.find_last_of("/")+1);
    return "";
}

std::string& replaceAll(std::string& context, const std::string& from,
const std::string& to) {
size_t lookHere = 0;
size_t foundHere;
while((foundHere = context.find(from, lookHere))
!= std::string::npos) {
context.replace(foundHere, from.size(), to);
lookHere = foundHere + to.size();
}

return context;
} ///:~

#if 0//defined(ANDROID)
#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h> /* for atexit() */

//include "SDL.h"

//define _N 6
#define _N 3

//static SDL_mutex *mutex = NULL;
static SDL_threadID mainthread;
static SDL_Thread *threads[_N];
static SDL_Thread *thread = NULL;
static volatile int doterminate = 0;

/*
 * SDL_Quit() shouldn't be used with atexit() directly because
 *  calling conventions may differ...
 */
static void
SDL_Quit_Wrapper(void)
{
    SDL_Quit();
}

void
printid(void)
{
    LOG_PRINT("Process %lu:  exiting\n", SDL_ThreadID());
}

void
terminate(int sig)
{
    signal(SIGINT, terminate);
    doterminate = 1;
}

void
closemutex(int sig)
{
    SDL_threadID id = SDL_ThreadID();
    int i;
    LOG_PRINT("Process %lu:  Cleaning up...\n", id == mainthread ? 0 : id);
    doterminate = 1;
    for (i = 0; i < _N; ++i)
        SDL_WaitThread(threads[i], NULL);
    SDL_DestroyMutex(mutex);
    stx_exit(sig);
}

int init(unsigned int width, unsigned int height);
int SDLCALL
Run1(void *data);

int SDLCALL
Run2(void *data)
{
    if (SDL_ThreadID() == mainthread)
        signal(SIGTERM, closemutex);
    while (!doterminate) {
        LOG_PRINT("Process %lu ready to work\n", SDL_ThreadID());
        if (SDL_LockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        LOG_PRINT("Process %lu, working!\n", SDL_ThreadID());
        init(640, 512);
        LOG_PRINT("Process %lu, done!\n", SDL_ThreadID());
        if (SDL_UnlockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't unlock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        /* If this sleep isn't done, then threads may starve */
        SDL_Delay(10);
    }
    if (SDL_ThreadID() == mainthread && doterminate) {
        LOG_PRINT("Process %lu:  raising SIGTERM\n", SDL_ThreadID());
        raise(SIGTERM);
    }
    return (0);
}
int SDLCALL
Run3(void *data);
int main(int argc, char *argv[])
{
#if 1
	ApplicationLogic();
#else
	mainthread = SDL_ThreadID();
	LOG_PRINT("Main thread: %lu\n", mainthread);

	threads[0] = SDL_CreateThread(Run1, "Worker1", NULL);
        //SDL_WaitThread(thread1, NULL);
    	//SDL_DestroyMutex(mutex);

	threads[1] = SDL_CreateThread(Run2, "Worker2", NULL);
        //SDL_WaitThread(thread2, NULL);
    	//SDL_DestroyMutex(mutex);

	threads[2] = SDL_CreateThread(Run3, "Worker3", NULL);
        //SDL_WaitThread(thread3, NULL);
    	//SDL_DestroyMutex(mutex);

    	signal(SIGINT, terminate);
    	//???Run(NULL);
#endif
}
#ifdef __cplusplus
	};
#endif
#endif


int STXAPIENTRY stx_chdir(const char * _Path){return 0;}

bool STXAPIENTRY str_ends_with(const char * str, const char * suffix) {

  if( str == NULL || suffix == NULL )
    return false;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
    return false;

  int r=strncmp( str + str_len - suffix_len, suffix, suffix_len );
  if(r)
	return true;
  return false;
}

bool STXAPIENTRY str_ends_with(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() && s.rfind(suffix) == (s.size()-suffix.size());
}

bool STXAPIENTRY stx_isBigEndian()
{
	int a=1;
        return !((char*)&a)[0];
}

bool STXAPIENTRY stx_isLittleEndian()
{
	int a=1;
        return ((char*)&a)[0];
}

void STXAPIENTRY stx_exit(int a)
{
	//???DBG_HALT;
	exit(a);
}

    void * STXAPIENTRY stx_malloc(size_t size)
{
	void* p=0;
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	p=SDL_malloc(size);
#else
	p=malloc(size);
#endif
	//gs.push_back(p);
	return p;
}

    void * STXAPIENTRY stx_calloc(size_t nmemb, size_t size)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	return SDL_calloc(nmemb, size);
#else
	return calloc(nmemb, size);
#endif
}
    void * STXAPIENTRY stx_realloc(void *mem, size_t size)
{
	void* p=0;
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	p=SDL_realloc(mem, size);
#else
	p=realloc(mem, size);
#endif
	/*
	std::vector<void*>::iterator it=gs.begin();
	for(unsigned int i=0;i<gs.size();i++)
	{
		if(gs[i]==mem)
			gs.erase(it+i);
	}
	gs.push_back(mem);
	*/
	return p;
}
    void STXAPIENTRY stx_free(void *mem)
{
	
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	SDL_free(mem);
#else
	free(mem);
#endif
	/*
	std::vector<void*>::iterator it=gs.begin();
	for(unsigned int i=0;i<gs.size();i++)
	{
		if(gs[i]==mem)
		gs.erase(it+i);
	}
	*/
}
    void STXAPIENTRY stx_closeall()
{
	
	/*
	std::vector<void*>::iterator it=gs.begin();
	for(unsigned int i=0;i<gs.size();i++)
	{
		gs.erase(it+i);
	}
	gs.clear();
	gs.empty();
	*/
}
    char * STXAPIENTRY stx_getenv(const char *name)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	return SDL_getenv(name);
#else
	return getenv(name);
#endif
}
    int STXAPIENTRY stx_putenv(const char *variable)
{
#if defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR) || defined(WIN_PHONE_APP)
	return 0;
#elif defined(_WIN32)
	return _putenv(variable);
#else
        return putenv((char*)variable);
#endif
}
    void STXAPIENTRY stx_qsort(void *base, size_t nmemb, size_t size,
           int (*compare)(const void *, const void *))
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	SDL_qsort(base,nmemb,size,compare);
#else
	qsort(base,nmemb,size,compare);
#endif
}
    void * STXAPIENTRY stx_memset(void *dst, int c, size_t len)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	return SDL_memset(dst,c,len);
#else
	return stx_memset(dst,c,len);
#endif
}
    void * STXAPIENTRY stx_revcpy(void *dst, const void *src, size_t len)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	//???SDL_revcpy(dst,src,len);
	return 0;//???dst;//???
#else
	return 0;//???revcpy(dst,src,len);
#endif
}
    void * STXAPIENTRY stx_memmove(void * _Dst, const void * _Src, size_t _Size)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	SDL_memmove(_Dst,_Src,_Size);
	return _Dst;
#else
	return stx_memmove(_Dst,_Src,_Size);
#endif
}
    int STXAPIENTRY stx_memcmp(const void *s1, const void *s2, size_t len)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	return SDL_memcmp(s1,s2,len);
#else
	return stx_memcmp(s1,s2,len);
#endif
}

float STXAPIENTRY stxAbs(float f)
{
    if (f < 0) {return -f;}
    return f;
}

int STXAPIENTRY stxEq(float fValue1, float fValue2)
{
    return stxAbs(fValue1 - fValue2) < stxEPSILON;
}

void * STXAPIENTRY stx_memcpy(void *d, const void *s, size_t l)
{
#if defined(SDL_1_2_15_BUILD)  || defined(SDL_2_0_5_BUILD)
	SDL_memcpy(d,s,l);
	return d;//???
#else
	return stx_memcpy(d,s,l);
#endif
}
char * STXAPIENTRY stx_strtok(char * _Str, const char * _Delim)
{
#ifdef _WIN32
	return
		strtok_s(_Str, _Delim, 0);//???
#else
	return
		strtok(_Str, _Delim);
#endif
}

#ifdef _MSC_VER
#include <direct.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
//if defined(__cplusplus) && !defined(ANDROID)
#include <cstring>
//endif

#include <stdlib.h>

char * STXAPIENTRY stx_getcwd(char * _Path)
{
	static char path[MAX_PATH];
	path[0]='\0';
#if defined(WIN32)
	_getcwd(path, MAX_PATH);
#elif 1//???
	getcwd(path, MAX_PATH);
#endif
	stx_strlcpy(_Path, path, MAX_PATH);
	return _Path;
}

           static unsigned int mynext = 1;

           // RAND_MAX assumed to be 32767
           int myrand(void) {
               mynext = mynext * 1103515245 + 12345;
               return((unsigned)(mynext/65536) % 32768);
           }

           void mysrand(unsigned seed) {
               
		mynext = seed;
           }
unsigned int STXAPIENTRY stx_randnext(void)
{
	return mynext;
}
int STXAPIENTRY stx_rand(void)
{
#if 1
	return rand();
#else
	return myrand();
#endif

}
void STXAPIENTRY stx_srand(unsigned int seed)
{
#if 1
	srand(seed);
#else
	mysrand(seed);
#endif
}

int STXAPIENTRY stx_isopened(FILE* _File)
{
        if(_File)
		{return 1;}
	else
		{return 0;}
}

int STXAPIENTRY stx_fileExists(const char* fileName)
{
/*
	std::ifstream f(filename);
	return !f.fail();
*/
	FILE* r=fopen(fileName,"rb");
	if(stx_isopened(r))
	{
    		fclose(r);
    		return 1;
	}
	#ifdef LINUX
	LOG_PRINT("File %s does not exist!\n", fileName);
	#endif

	char path[MAX_PATH];
	stx_getcwd(path);
	#ifdef LINUX
	LOG_PRINT("path=%s\n", path);
	#endif

	//STX_STACK_TRACE;
	//exit(0);
	return 0;
}

const char STXAPIENTRY stx_getOsSeparator()
{
#ifndef _WIN32
	return '/';
#else
	return '\\';
#endif
}
const char* STXAPIENTRY stx_getOsSeparatorString()
{
	static char buf[2] = {0};
#ifndef _WIN32
        stx_strlcpy(buf,"/\0",2);		
	return buf;
#else
        stx_strlcpy(buf,"\\\0",2);		
	return buf;
#endif
}
#if 1//!defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR) && !defined(__APPLE__)
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> removeEmptyStrings(std::vector<std::string>& elems)
{
	std::vector<std::string> elems2;
	for(unsigned int i=0;i<elems.size();i++)
	{
		if(""==elems[i])
			continue;
		elems2.push_back(elems[i]);
	}
	return elems2;
}

std::string &stx_trim(std::string &s);

template<typename Out>
void stx_split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = stx_trim(item);
    }
}

std::vector<std::string> stx_split(const std::string &s, char delim) {
    std::vector<std::string> elems;
#ifndef _MSC_VER
    stx_split(s, delim, std::back_inserter(elems));

    for(unsigned int i=0;i<elems.size();i++)
	elems[i]=stx_trim(elems[i]);
#else
    std::string::size_type b = 0;
    while ((b = s.find_first_not_of(delim, b)) != std::string::npos) {
        auto e = s.find_first_of(delim, b);
        elems.push_back(s.substr(b, e-b));
        b = e;
    }
#endif
    return removeEmptyStrings(elems);
}

std::string stx_join(std::vector<std::string>& v, char delim) {
    std::string str="";
    if(!v.size())
	return str;
    str=v[0];
    std::string delimstr="_";
	delimstr[0]=delim;
    for(unsigned int i=1;i<v.size();i++)
    	str+=delimstr+v[i];
    return stx_trim(str);
}

std::vector<std::string> stx_split(const std::string &s, const std::string &delimiter) {
    std::vector<std::string> elems;
	std::string s_ = s;
#ifndef _MSC_VER
    size_t pos = s_.find(delimiter);
    std::string token;
    while (pos)
    {
    	token = s_.substr(0, pos);
    	s_.erase(0, pos + delimiter.length());
    	elems.push_back(stx_trim(token));
        //LOG_PRINT("str2=%s\n", elems[elems.size()-1].c_str());
	if(pos == std::string::npos)
		break;
	pos = s_.find(delimiter);
    }
#else
    while(s_.size()){
        int index = s_.find(delimiter);
        if(index!=string::npos){
            elems.push_back(s_.substr(0,index));
            s_ = s_.substr(index+delimiter.size());
            if(s_.size()==0)elems.push_back(s_);
        }else{
            elems.push_back(s_);
            s_ = "";
        }
    }
#endif
    return removeEmptyStrings(elems);
}

std::string stx_join(std::vector<std::string>& v, const std::string &delim) {
    std::string str="";
    if(!v.size())
	return str;
    str=v[0];
    std::string delimstr=delim;
    for(unsigned int i=1;i<v.size();i++)
    	str+=delimstr+v[i];
    return stx_trim(str);
}
#endif

#ifdef __APPLE__
std::string& stx_ltrim(std::string& str)
{
    const std::string& chars = "\t\n\v\f\r ";
    str.erase(0, str.find_first_not_of(chars));
    return str;
}
 
std::string& stx_rtrim(std::string& str)
{
    const std::string& chars = "\t\n\v\f\r ";
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}
#else
// stx_trim from start
std::string &stx_ltrim(std::string &s) {
#ifndef _MSC_VER
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
#else
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(stx_isspace))));
#endif
    return s;
}

// stx_trim from end
std::string &stx_rtrim(std::string &s) {
#ifndef _MSC_VER
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
#else
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(stx_isspace))).base(), s.end());
#endif
    return s;
}
#endif
// stx_trim from both ends
std::string &stx_trim(std::string &s) {
    return stx_ltrim(stx_rtrim(s));
}

#include <iterator>
std::vector<std::string> stx_split(std::string const &input)
{ 
    std::istringstream buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer), 
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}

