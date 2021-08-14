[Vertex shader]
// Vertex shader for DM calculation

struct appdata
{
    float4 position : POSITION;
    float4 COL0 : COLOR0;
};

struct vfconn
{
    float4 HPOS : POSITION;
    float4 COL0 : COLOR0;
    float4 COL1 : COLOR1;
    float4 TEX0 : TEXCOORD0;    
    float4 TEX1 : TEXCOORD1;    
    float4 TEX2 : TEXCOORD2;    
    float4 TEX3 : TEXCOORD3;    
    float4 TEX4 : TEXCOORD4;    
    float4 TEX5 : TEXCOORD5;   
    float4 TEX6 : TEXCOORD6;  
};



float4 tex2D_bilinear4(uniform sampler2D tex, half4 t, float2 Scales)
{
  float size  = Scales.x;
  float scale = Scales.y;
  
  //if (f.x < 0) f.x = 1+f.x;
  //if (f.y < 0) f.y = 1+f.y;
  //t-=f;
  float4 t00 = TEX2D(tex, t);
  float4 t10 = TEX2D(tex, t + half4(scale,0,0,0));

  float2 f = frac(t.xy*size);

  float4 tA  = lerp(t00, t10, f.x);

  float4 t01 = TEX2D(tex, t + half4(0,scale,0,0));
  float4 t11 = TEX2D(tex, t + half4(scale,scale,0,0) );

  float4 tB  = lerp(t01, t11, f.x);
  return lerp(tA, tB, f.y);
}

float tex2D_bilinear4x(uniform sampler2D tex, half4 t, float2 Scales)
{
  float size  = Scales.x;
  float scale = Scales.y;
  

  float4 tAB0 = TEX2D(tex, t);

  float2 f = frac(t.xy*size);

  float2 tAB  = lerp(tAB0.xz, tAB0.yw, f.x);
  return lerp(tAB.x, tAB.y, f.y);
}

float tex2D_trilinear(uniform sampler2D tex, half4 t, float2 Scales) {

  float fr = frac(t.z);
	t.z -= fr;// floor(t.zw);
  float Res;
  if (fr < 0.30) Res =  tex2D_bilinear4x(tex, t.xyzz, Scales);
  else
   if (fr > 0.70) Res =  tex2D_bilinear4x(tex, t.xyzz+float4(0,0,1,1), Scales*float2(0.5, 2));
   else {
	    Res = tex2D_bilinear4x(tex, t.xyzz, Scales);
      float Res1 = tex2D_bilinear4x(tex, t.xyzz+float4(0,0,1,1), Scales*float2(0.5, 2));
      fr=saturate((fr-0.30)*(1/(0.70-0.30)));
      Res = Res1*fr+Res*(1-fr);
  }
  
	return Res;//Res;
}


vfconn main(appdata IN, 
            uniform sampler2D HMap0, 
            uniform sampler2D tex1, 
            uniform sampler2D tex2, 
            uniform sampler2D HMap1, 
            //uniform float4x4 ModelViewProj,
            

            uniform float4 Gabarites,
            uniform float4 VOfs     ,
            uniform float4 CPos     ,
            uniform float4 MapRTrasf,
            uniform float4 Expld[2] 

           )            
 {
  vfconn Out;
  
  
  float4 INP = IN.position;
  INP.z+=VOfs.w;
  float FarRun = 0;
  INP.xy = INP.xy*(pow(INP.z,4)*VOfs.z);
  INP.z = 0;
  

  float4 ClipPos = mul(glstate.matrix.mvp, INP);
  Out.HPOS = ClipPos;//mul(glstate.matrix.mvp, IN.position);//ClipPos;  
  Out.COL0 = IN.COL0;

  float3 b0 = abs(ClipPos.xyz) < (ClipPos.www*1.15+1.5+FarRun);
  
  
  Out.TEX3.z  = 1;
  
  if (all(b0)) {
    float vDisp = 0;
    float4 t;
    float  TexSize;
    float  scale;
    float4 vPos = INP;  
    float jf = IN.position.z+VOfs.w;
    
    t.xy = (INP.xy+VOfs.xy)*Gabarites.x;
    float MipDist2 = dot(INP.xy, INP.xy);//-CPos.xy;
    
    t.z = (MipDist2-50*50)*7/(300*300-50*50);//length(MipDist2)/3400
    t.z = clamp(t.z,0, 7);//+FogTexScale.w;
  
    const half2 CS = half2(cos(3.4*10/180),sin(3.4*10/180));
    Out.TEX2.xy = half2(t.x*CS.x+t.y*CS.y, -t.x*CS.y+t.y*CS.x);
    
    TexSize = Gabarites.y*pow(2,-floor(t.z));
    scale =1.0f/TexSize;
    float2 Scales=float2(TexSize, scale);

   
     vDisp =  tex2D_trilinear/*tex2D_bilinear4x*/(HMap0, t.xyzz, Scales);
      
     float Wavy = 1;
      


    if (jf < 200) {//second pass
      float R = saturate((jf-50) *(1.0f/(200.0f-50.0f)));
      float S = (1.0f/5.1)*(1-R);  
      
      float4 t1 = t.xyzw*5.1;
      t1.zw = 0;//flour 0;//clamp(ceil(R*3),0, 7);
      
      float TexSize = Gabarites.y*pow(2,-t1.z);
      float scale =1.0f/TexSize;
      float2 Scales=float2(TexSize, scale);
      vDisp += (tex2D_bilinear4x(HMap1, t1, Scales))*S;
    }
	
    vDisp*=Gabarites.z;

   /* dynamic Explosions -  Sorry not finished
   if (Expld[0].w > 0) {
     float time = Expld[1].w;
     float2 d = Expld[0].xy+INP.xy-CPos.xy;
     float r2=dot(d,d);
     vDisp.z += -cos(+sqrt(r2)*Expld[0].z+time)*min(Expld[0].w/r2, 10);
    }*/
    
    Out.TEX3.z  = Gabarites.w/(Gabarites.z*Gabarites.y*Wavy);
    Out.TEX3.w = (Wavy-0.25)*1.5;

    
    vPos.z = (vDisp-Gabarites.z*0.5)*Wavy;

    Out.TEX3.xy = t.xy;//vDisp.xy;
    Out.TEX6.xy = t.xy*float2(0.293*6.28, 0.371*6.28)+MapRTrasf.z;
 

    float2 tn = t.xy*float2(3.33*2.1, 3.37*5.7);
    const float2 CS2 = float2(sin(15*3.14/180), cos(15*3.14/180));
    float2 _CS = float2(-sin(15*3.14/180), cos(15*3.14/180));
  

    Out.TEX5 = t*5.1;//vDisp.xy;
    Out.TEX5.zw = float2(tn.x*CS2.y+tn.y*CS2.x, -tn.x*CS2.x+tn.y*CS2.y); //float2(dot(tn.xy,CS.yx), dot(tn.xy,CS.xy));
  
  
    Out.TEX4 = vPos-CPos;
  
    Out.HPOS = mul(glstate.matrix.mvp/*ModelViewProj*/, vPos );
  }

  return(Out);
 }
[Fragment shader]
/*************************************************************************
// Complex DM Water fragment shader with Cube map and EMBM mixing and per pixel freshel
******************************************************************************/

fp40 half4 main(
            uniform half4 c0 ,
            uniform half4 c1 ,
            uniform half4 c2 ,
            uniform half4 c3 ,

            uniform half4 c6 ,
            uniform half4 c7 ,
            uniform half4 c8 ,
            uniform half4 c9 ,

            in half4 Col0              : COLOR0,
            in half4 Col1              : COLOR1,

            in half2 FogV: TEXCOORD1,
            in float4 INP : TEXCOORD3,
            in float4 INP1 : TEXCOORD5,
            
            in float3 EyeVec           : TEXCOORD4,
            in float4 TexGens: TEXCOORD2,
            in float4 FoamRndPhase: TEXCOORD6,

            uniform sampler2D   NMap,
            uniform sampler2D   FogMap,
            uniform samplerCUBE NormCMap,
            uniform sampler2D   NMap2,
            uniform samplerCUBE EnvCMap,
            uniform sampler2D   dXYMap2,// WRefl,
            uniform sampler2D   FoamMap,
            uniform sampler2D   RMap,
            uniform sampler2D   dXYMap,
            uniform sampler2D   dXYMap1,
			uniform sampler2D   dXYMap3,
            uniform samplerCUBE NormCMap2
            
           ) : COLOR
{
  // Sum (H,Nx,Ny) maps in 0..1
  half3 N = tex2D(dXYMap, INP.xy).xyz+tex2D(dXYMap1, INP1.xy).xyz*half3((1.0/3.1), 1.0, 1.0);
  
  // Add micro bump
  float4  tt = INP1.xyyy*13.1; tt.zw = -8;
  N += TEX2D(dXYMap, tt).xyz*half3((1.0/13.1/0.4), 1.0, 1.0)*0.4; 
  float FoamNx = N.x;
  
  half3 Eye = normalize(EyeVec.xyz);
  
  N.xy = -(N.yz*2-2.4);//*(1-R2);  
  N.z = INP.z;  N = normalize(N); // N -  final normal
  
  // Calc  reflection vector
  half   dotEye_N = dot(Eye, N);
  half3 reflectVec = 2*N*dotEye_N - Eye/**dot(N,N)*/;

  #if 1 // Hack to eleminate backside artifacts
	half R = saturate((-reflectVec.z+0.05)*10);
	N = normalize(lerp(half3(0,0,1), N, R));
	dotEye_N = dot(Eye, N);
	reflectVec = 2*N*dotEye_N - Eye;
  #endif
  

  half4 PlaneRefl = 0; // Sorry - Reflection of landscape not is implemented now
  
  half3 reflectColor = texCUBE(EnvCMap, reflectVec.xzy*float3(1,1,-1)).xyz;
  // Add sun road splashes
  reflectColor*=1+saturate(reflectColor.x - 0.97)*200.0h;

  half3 diffuseL = saturate(dot(N, normalize(c2.xyz)));
  half3 diffuse = (diffuseL+c0.w)*c0.xyz;
  half Freshel = pow((1-max(-dotEye_N, c3.b)), 5)*c3.g + c3.r;

  half3 WaterColor = reflectColor*Freshel+diffuse;
  
  if (FoamNx > 0.7) {// Foam caps
    half Foam = FoamNx*c9.x+c9.y;//*INP.w;
    Foam=saturate(Foam*(tex2D(FoamMap, TexGens.xy*23h).w*(tex2D(FoamMap, TexGens.xy*397h).w*0.5h+0.75h)/*-0.85*/)/**5*/);
    WaterColor = lerp(WaterColor,(diffuseL*0.4+0.5), Foam);
  }

  half4 Out; Out.xyz = WaterColor;  
  Out.w = 1;
 
  return Out;
}

