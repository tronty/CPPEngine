#if 0
#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform vec2 resolution;
uniform float time;
#endif

// random/hash function              
float hash( float n )
{
  return fract(cos(n)*41415.92653);
}

// 3d noise function
float noise( in vec3 x )
{
  vec3 p  = floor(x);
  vec3 f  = smoothstep(0.0, 1.0, fract(x));
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
    mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
    mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

float sdf_wave(vec3 p)
{
    mat2 m2 = mat2(1.6,-1.2,1.2,1.6); // 2倍スケール + 回転行列
    float speed = 3.141592 / 2.0;
    float waveHeight = .5;

    float wave;
    for (int i = 0; i < 4; i++)
    {
      p.xz *= m2;
      wave += p.y - waveHeight * sin(p.x * 0.07 + p.z * 0.21  - time * speed);
      waveHeight *= 0.6;
    }
    // wave += 12.0;
    wave += noise(p) * 0.03;
    return wave;
}

vec3 getNormal(vec3 p)
{
    float diff = 0.01;
    return normalize(vec3(
        sdf_wave(p + vec3(diff, 0, 0)) - sdf_wave(p - vec3(diff, 0, 0)),
        sdf_wave(p + vec3(0, diff, 0)) - sdf_wave(p - vec3(0, diff, 0)),
        sdf_wave(p + vec3(0, 0, diff)) - sdf_wave(p - vec3(0, 0, diff))
    ));    
}

#define SKY_COLOR vec3(19, 27, 44) / 255.0
#define SUN_COLOR_1 vec3(246, 254, 207) / 255.0
#define SUN_COLOR_2 vec3(182, 149, 95) / 255.0
// #define SUN_COLOR_3 vec3(96, 57, 28) / 255.0
#define SUN_COLOR_3 vec3(105, 54, 25) / 255.0
#define SUN_COLOR_SCALE 4.0
#define SKY_COLOR_HORIZON_1 vec3(96, 57, 28) / 255.0
#define SKY_COLOR_HORIZON_2 vec3(105, 54, 25) / 255.0

#define WATER_BASE_COLOR vec3(15, 21, 21)

// 海へのレイマーチング
void raymarch(vec3 cPos, vec3 rd, out float isHit, out vec3 rp)
{
    if (rd.y > 0.0) // Rayが上向きの場合、海にヒットしないのでRaymarchを行わない
    {
        isHit = 0.0;
        return;
    }

    float t = 0.0;
    // float isHit = 0.0;
    float st = 0.06;
    for (int i = 0; i < 500; i++)
    {
        if (i > 400) st = 0.7;

        rp = cPos + t * rd; // 
        if (sdf_wave(rp) < 0.001)
        {
            isHit = 1.0;
            break;
        }
        t += st;
    }
}

vec3 renderSky(vec2 p, vec3 rd, vec3 l)
{
    float sundot = clamp(dot(rd, l), 0.0, 1.0);

    vec3 sunGrad1 = pow(sundot, 350.0) * SUN_COLOR_1;
    vec3 sunGrad2 = pow(sundot, 15.0) * SUN_COLOR_2 * 0.7;
    vec3 sunGrad3 = pow(sundot, 4.0) * SUN_COLOR_3 * 0.7;

    return sunGrad1 + sunGrad2 + sunGrad3 + SKY_COLOR;
}

void main()
{
    vec2 p = (xlv_TEXCOORD0.xy * 2.0 - resolution.xy) / max(resolution.x, resolution.y);
    vec3 cPos = vec3(0,1.2,0); // camera position
    vec3 cDir = normalize(vec3(0,0.15,1)); // camera direction
    vec3 cSide = normalize(cross(cDir, vec3(0,1,0))); // camera side
    vec3 cUp = normalize(cross(cSide, cDir)); // camera up

    vec3 rd = normalize(cSide * p.x + cUp * p.y + cDir); // ray direction
    vec3 rp; // ray position
    float isHit; // Rayがヒットしたかどうか
    raymarch(cPos, rd, isHit, rp);

    // Light vector
    vec3 l = normalize(vec3(0, 0.25, 1.0)); // 光源への向きベクトル

    // Sky
    vec3 skyColor = renderSky(p, rd, l);

    // Bling-Phonn Shading
    vec3 n = getNormal(rp); // Rayがヒットした位置の法線
    vec3 r = reflect(-l, n);  // 光の反射ベクトル
    float d = 0.5; // 拡散反射の強度
    float s = 2.0; // 鏡面反射の強度
    float a = 0.1; // 環境光の反射の強度
    vec3 h = normalize(r + rd); // Bling-Phonnで使うベクトル
    float nDotL = clamp(dot(n, l), 0.0, 1.0);
    float nDotH = clamp(dot(n, h), 0.0, 1.0);
    float nDotV = clamp(dot(rd, n), 0.0, 1.0);
    vec3 waterColor = 
        + SKY_COLOR * a  + SKY_COLOR * d * nDotL + SUN_COLOR_2 * s * pow(nDotH, 50.0) // Bling-Phonn
        + SKY_COLOR_HORIZON_1 * nDotV * 4.0; // Specular (Phonn)

    waterColor = mix(waterColor, skyColor, 0.45);

    gl_FragColor = vec4(mix(skyColor, waterColor, isHit), 1.0);

}
