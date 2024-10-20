void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);  // Normaali koordinaatisto
    vec2 center = vec2(0.5, 0.5);
    
    float radius = 0.5;
    float dist = distance(uv, center);
    
    if (dist > radius) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // Musta tausta pallon ulkopuolella
        return;
    }
    
    float turbulence = sin(dist * 10.0 + sin(dist * 20.0));
    float intensity = (1.0 - dist) * 0.8 + 0.2 * turbulence;
    
    vec3 color = vec3(1.0, 0.5 + 0.5 * turbulence, 0.0) * intensity;
    gl_FragColor = vec4(color, 1.0);
}
