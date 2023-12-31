
float3      iResolution;                  // viewport resolution (in pixels)
float     iTime;                        // shader playback time (in seconds)
float     iGlobalTime;                  // shader playback time (in seconds)
float4      iMouse;                       // mouse pixel coords
float4      iDate;                        // (year, month, day, time in seconds)
float     iSampleRate;                  // sound sample rate (i.e., 44100)
float3      iChannelResolution[4];        // channel resolution (in pixels)
float     iChannelTime[4];              // channel playback time (in sec)

float2      ifFragCoordOffsetUniform;     // used for tiled based hq rendering
float     iTimeDelta;                   // render time (in seconds)
int       iFrame;                       // shader playback frame
float     iFrameRate;

struct Channel {
    float3  resolution;
    //float time;
};

