[Vertex shader]
#version 130

struct VsOut {
    vec4 position;
    vec2 texCoord;
};
struct VsIn {
    vec2 position;
    vec2 texCoord;
};
uniform vec4 scaleBias;
VsOut xlat_main( in VsIn In ) {
    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0));
    Out.position.xy = ((In.position.xy * scaleBias.xy) + scaleBias.zw);
    Out.position.w = 1.0;
    Out.texCoord = In.texCoord;
    return Out;
}
varying vec2 xlv_TEXCOORD0;
void main() {
    VsOut xl_retval;
    VsIn xlt_In;
    xlt_In.position = vec2(gl_Vertex);
    xlt_In.texCoord = vec2(gl_MultiTexCoord0);
    xl_retval = xlat_main( xlt_In);
    gl_Position = vec4(xl_retval.position);
    xlv_TEXCOORD0 = vec2(xl_retval.texCoord);
}

[Fragment shader]
#version 130

struct VsOut {
    vec4 position;
    vec2 texCoord;
};
uniform sampler2D Base;
uniform vec4 colorRGBA;
vec4 xlat_main( in VsOut IN ) {
    return (texture2D( Base, IN.texCoord) * colorRGBA);
}
varying vec2 xlv_TEXCOORD0;
void main() {
    vec4 xl_retval;
    VsOut xlt_IN;
    xlt_IN.position = vec4(0.0);
    xlt_IN.texCoord = vec2(xlv_TEXCOORD0);
    xl_retval = xlat_main( xlt_IN);
    gl_FragData[0] = vec4(xl_retval);
}

