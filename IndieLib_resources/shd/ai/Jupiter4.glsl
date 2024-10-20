precision mediump float;

#define speed 5.2
#define k2PI (2.*3.14159265359)
#define kStarDensity 0.4
#define kMotionBlur 0.2
#define kNumAngles 200.

// Funktio perus noise-tehosteen luomiseen
float noise(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Funktio fbm (fractaalinen brownian liike) turbulenssin lisäämiseksi
float fbm(vec2 uv) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(uv * frequency);
        uv *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0); // Oletetaan resoluutio 800x600
    vec2 center = vec2(0.5, 0.5);
    vec2 pos = uv - center;
    float dist = length(pos) * 4.0;

    // Tarkistetaan, onko pikseli Jupiterin pallon sisällä
    float planetMask = smoothstep(1.0, 0.98, dist);

    // Luodaan Jupiterin perusväritys
    vec2 sphereUV = uv;
    sphereUV.x *= 2.0; // Venytetään X-akselia Jupiterin pitkittäisten vöiden vuoksi
    float jupiterBaseColor = sphereUV.y + fbm(sphereUV * 5.0) * 0.1;
    vec3 color = vec3(1.0, 0.5, 0.2) * smoothstep(0.2, 0.8, jupiterBaseColor);

    // Lisätään symmetriset tummat vöitä ja pyörteitä
    float symmetricY = abs(sphereUV.y - 0.5) * 2.0; // Symmetriset vyöt ekvaattorin molemmin puolin
    float bands = sin(symmetricY * 30.0 + fbm(sphereUV * 6.0)) * 0.15;
    color -= vec3(bands, bands * 0.5, bands * 0.25);

    // Korostetaan symmetrisiä pilvivöitä
    float cloudBands = cos(symmetricY * 40.0 + fbm(sphereUV * 8.0)) * 0.1;
    color += vec3(cloudBands, cloudBands * 0.6, cloudBands * 0.3);

    // Lisätään pieniä pyörremäisiä kuvioita
    float swirl = fbm(sphereUV * 10.0) * 0.05;
    color += vec3(swirl, swirl * 0.5, 0.0);

	vec3 starColor;
{
	vec2 position = ( xlv_TEXCOORD0.xy -  resolution.xy*.5 ) / resolution.x;

	float A = atan(position.y,position.x);
	float angle0 = A / k2PI;
	float angle = fract(angle0 + .002*time);
	float rad = .3*length(position);
	float angleFract = fract(angle*kNumAngles);
	float angleStep = floor(angle*kNumAngles);
	float angleToRandZ = 10.*fract(angleStep*fract(angleStep*.7535)*45.1);
	float angleSquareDist = fract(angleStep*fract(angleStep*.82657)*13.724);
	float t = speed * time - angleToRandZ;
	float angleDist = (angleSquareDist+0.1);
	float adist = angleDist/rad*kStarDensity;
	float dist = abs(fract((t*.1+adist))-.5);
	float white1 = max(0.,1.0 - dist * 100.0 / (kMotionBlur*speed+adist));
	float white2 = max(0.,(.5-.5*cos(k2PI * angleFract))*1./max(0.6,2.*adist*angleDist));
	float white = white1*white2;
	starColor.r = .03*white1 + white*(0.3 + 5.0*angleDist);
	starColor.b = white*(0.1 + .5*angleToRandZ);
	starColor.g = 1.5*white;
	
	float nebD1 = 1.0/rad + 4.5*(1.0 + sin(1.1 + 3.0*A + 0.71*cos(2.0*A)));
	float nebD2 = 1.0/rad + 3.7*(1.0 + sin(3.7 + 2.0*A + 0.46*sin(3.0*A)));
	float R1 = 1.0 * rad * (1.0 + sin(0.3+3.0*A + 2.4 * cos(0.2+3.0*A)*sin(2.1+0.42*(nebD1+speed*time)) + sin(2.0*6.283*position.x) ));
	float R2 = 1.0 * rad * (1.0 + sin(1.1+4.0*A + 3.2 * cos(0.7+4.0*A)*sin(1.7+0.27*(nebD2+speed*time)) + cos(3.0*6.283*position.y) ));
	float P1 = 0.5 + .5*sin(5.7*position.x+.22*(speed*time));
	float P2 = 0.5 + .5*sin(4.44*position.y+.17*(speed*time)) ;
	starColor.r += 0.6*R1 + 0.3*R2 + 0.1*P1*P2;
	starColor.b += 0.3*R1 + 0.8*R2 + .1*P2*R1;
	starColor.g += 1.1*R1*R2*P2;
}

    // Yhdistetään Jupiterin väri ja uusi tähtitaivas
    color = mix(starColor, color, planetMask);

    gl_FragColor = vec4(color, 1.0);
}

