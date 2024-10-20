// o1-preview: 2D textureless, vector-based, bufferless house with chimney in a GLSL 1.3 fragment shader without uniform shader variables?

#ifdef GL_ES
precision mediump float;
#endif

// Function to draw a rectangle
bool drawRect(vec2 uv, vec2 pos, vec2 size) {
    return uv.x > pos.x && uv.x < pos.x + size.x &&
           uv.y > pos.y && uv.y < pos.y + size.y;
}

// Function to draw a triangle (gabled roof)
bool drawTriangle(vec2 uv, vec2 p1, vec2 p2, vec2 p3) {
    float d1 = (p2.x - p1.x) * (uv.y - p1.y) - (p2.y - p1.y) * (uv.x - p1.x);
    float d2 = (p3.x - p2.x) * (uv.y - p2.y) - (p3.y - p2.y) * (uv.x - p2.x);
    float d3 = (p1.x - p3.x) * (uv.y - p3.y) - (p1.y - p3.y) * (uv.x - p3.x);
    return (d1 > 0.0 && d2 > 0.0 && d3 > 0.0) || (d1 < 0.0 && d2 < 0.0 && d3 < 0.0);
}

void main(void) {
    vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;
    uv.y = 1.0 - uv.y; // Flip Y axis for correct orientation
    
    vec3 color = vec3(0.8, 0.9, 1.0); // Sky color
    
    // Define house base (rectangular part)
    vec2 housePos = vec2(0.3, 0.3);
    vec2 houseSize = vec2(0.4, 0.3);
    
    // Define roof vertices
    vec2 roofLeft = vec2(0.3, 0.6);
    vec2 roofRight = vec2(0.7, 0.6);
    vec2 roofPeak = vec2(0.5, 0.8);
    
    // Define chimney position and size
    vec2 chimneyPos = vec2(0.55, 0.65);
    vec2 chimneySize = vec2(0.05, 0.15);
    
    // Drawing house base
    if (drawRect(uv, housePos, houseSize)) {
        color = vec3(0.8, 0.5, 0.3); // House body color
    }
    
    // Drawing roof
    if (drawTriangle(uv, roofLeft, roofRight, roofPeak)) {
        color = vec3(0.6, 0.3, 0.2); // Roof color
    }
    
    // Drawing chimney
    if (drawRect(uv, chimneyPos, chimneySize)) {
        color = vec3(0.3, 0.3, 0.3); // Chimney color
    }
    
    gl_FragColor = vec4(color, 1.0);
}
