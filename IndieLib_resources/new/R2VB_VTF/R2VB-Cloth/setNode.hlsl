//-----------------------------------------------------------------------------
// File: setNode.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





[Vertex shader]

float4 main(float4 pos: POSITION) : POSITION {
	return pos;
}

[Fragment shader]

struct PsOut {
	float4 pos: COLOR0;
	float4 dir: COLOR1;
};

float3 pos;
float free;

PsOut main(){
	PsOut Out;

	// Set the node to a certain position and set
	// whether it should be freely moving or locked
	Out.pos = float4(pos, 0);
	Out.dir = float4(0, 0, 0, free);

	return Out;
}
