struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord);
}


