#if 0
#ifdef GL_ES
precision mediump float;
#endif

//uniform float time;
uniform vec2 resolution;
#endif

const float sphere_radius = 1.0;

void main() {
    vec2 p = (xlv_TEXCOORD0.xy * 2.0 - resolution.xy) / min(resolution.x, resolution.y);
    float amplitude1 = 0.1;
    float frequency1 = 10.0;
    float speed1 = 2.0;
    float noise1 = 1.0;
    float distortion1 = 0.1;
    float distortionSpeed1 = 2.0;
    float undulation1 = 0.5;
    float undulationSpeed1 = 2.0;
    
    float amplitude2 = 0.2;
    float frequency2 = 8.0;
    float speed2 = 1.5;
    float noise2 = 0.5;
    float distortion2 = 0.2;
    float distortionSpeed2 = 1.5;
    float undulation2 = 1.0;
    float undulationSpeed2 = 1.5;
    
    vec3 pos = vec3(p, sqrt(sphere_radius * sphere_radius - dot(p, p)));
    
    pos = vec3(mat3(
        vec3(0.707, 0.000, 0.707),
        vec3(0.408, 0.816, -0.408),
        vec3(-0.577, 0.577, 0.577)
    ) * pos);
    
    float x1 = pos.x * frequency1 + time * speed1;
    float y1 = pos.y + 1.0;
    
    float c1 = sin(x1 + noise1 * sin(y1 + time * undulationSpeed1)) * amplitude1 * sin(time * undulation1);
    
    float r1 = sin(c1 + distortion1 * sin(time * distortionSpeed1)) * 1.2;
    float g1 = sin(c1 + distortion1 * sin(time * distortionSpeed1 + 2.0)) * 1.5;
    float b1 = sin(c1 + distortion1 * sin(time * distortionSpeed1 + 4.0)) * 2.0;
    
    float x2 = pos.x + 1.4;
    float y2 = pos.y * frequency2 + time * speed2;
    
    float c2 = sin(y2 + noise2 * sin(x2 + time * undulationSpeed2)) * amplitude2 * sin(time * undulation2);
    
    float r2 = sin(c2 + distortion2 * sin(time * distortionSpeed2)) * 1.5;
    float g2 = sin(c2 + distortion2 * sin(time * distortionSpeed2 + 2.0)) * 1.2;
    float b2 = sin(c2 + distortion2 * sin(time * distortionSpeed2 + 4.0)) * 1.8;
    
    vec3 color = vec3(r1 + r2, g1 + g2, b1 + b2);
    
    gl_FragColor = vec4(color, 1.0);
}

