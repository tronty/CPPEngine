#define rayO vec3(1.5, 0., 0.)
#define r1 1.
#define gr1 .7
#define c1 vec3(0.)
#define r2 1.
#define c2 vec3(0., 4., 0.)

float sdfSphere(vec3 p, vec3 c, float radius){
    return length(p - c) - radius;
}

float sdfSphere2(vec3 p, vec3 c, float radius){
    return length(p - c) - 0.;
}

float hash3(vec2 xy){
    xy = mod(xy, .19);
    float h = dot(xy.yyx, vec3(.013, 27.15, 2027.3));
    h *= h;
    h *= fract(h);
    
    return fract(h);
}


vec3 quaternionVectorRotation(vec3 v, vec4 q){
    vec3 rowOne = vec3(1. - (2. * ((q.z * q.z) + (q.w * q.w))), 2.* ((q.y * q.z) - (q.x * q.y)), 2. * ((q.x * q.z) + (q.y * q.w)));
    vec3 rowTwo = vec3(2. * ((q.y * q.z) + (q.x * q.w)), 1. - (2. * ((q.y * q.y) + (q.w * q.w))), 2. * ((q.y * q.z) + (q.x * q.w)));
    vec3 rowThree = vec3(2. * ((q.y * q.w) - (q.x * q.z)), 2. * ((q.x * q.y) + (q.z * q.w)), 1. - (2. * ((q.y * q.y) + (q.z * q.z))));
    
    mat3 r = mat3(rowOne, rowTwo, rowThree);
    return r * v;
}

float color(float x, float a, float b){
    return ((b * x) * exp(a * (x - 1.)));
}

float color2(float x, float a, float x0){
    return 1. / (exp(abs(a * (x - x0))));
}



vec4 quaternionMult(vec4 a, vec4 b){
    return vec4(a.x * b.x - dot(a.yzw, b.yzw), a.x*b.yzw + b.x*a.yzw + cross(a.yzw, b.yzw));
}


vec3 generateReflectionVector(vec3 normal, vec3 inV){
    return inV - (2. * normal);
}


void main( )
{
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    
    //get the mouse location for look direction unless mouse is not down
    vec2 muv = iMouse.z > 0.0 ? iMouse.xy / iResolution.xy : vec2(0.5, 0.5);
    
        
    vec4 col = vec4(0.);
    
    float screenRatio = iResolution.y / iResolution.x;
    
    //Setting up the ray directions and other information about the point and camera
    //##############################################################################
    
    
    //camera direction angles phi (xy plane) and theta (xz plane)
    //float phi = radians(180.);
    //float theta = radians(105.);
    float phi = radians(360. * (1.- muv.x));
    float theta = radians(180. * (1. - muv.y));

    //get the camera direction as the basis for the rotation (each ray direction is a rotation of the camera direciton vector)
    //it is in quarternion form here so its a vec4 instead of a vec3
    vec4 camD = vec4(0., cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
    
    
    float rad90 = radians(90.);
    
    float fov = 110.;
    
    float xAng = radians(fov * (.5 - uv.x));
    //replace "fov" with "(fov + (110. * pow(.5 - uv.x, 2.)))" below to add a counteractment to the fisheye lens effect
    //it basically counteracts the artifact with quaternions that happens when you rotate by a large angle on one axis then try to rotate on another axis perpendicular, it just rotates around it thus making the new direction lesser
    float yAng = radians(fov * screenRatio * (.5 - uv.y));
    
    //get the axes that the quarternions should be based around (perpendicular to the camera plane or dv)
    vec3 xRotAxis = vec3(cos(phi) * sin(theta - rad90), sin(phi) * sin(theta - rad90), cos(theta - rad90));
    vec3 yRotAxis = cross(xRotAxis, camD.yzw);//vec3(cos(phi - rad90) * sin(theta), sin(phi - rad90) * sin(theta), cos(theta));
    
    //get the quarternions of the ray direction rotations
    vec4 xQuat = vec4(cos(xAng / 2.), xRotAxis * sin(xAng / 2.));
    vec4 yQuat = vec4(cos(yAng / 2.), yRotAxis * sin(yAng / 2.));
    
    
    
    //combine the rotations
    vec4 compQuat = quaternionMult(yQuat, xQuat);
    
    
    
    //get the conjugate of the compQuart
    vec4 conjComp = vec4(compQuat.x, -compQuat.yzw);

    
    //ray direction
    vec3 rayD = quaternionMult(quaternionMult(compQuat, camD), conjComp).yzw;
    
    
    //############################################
    
    float t = 0.;
    
    float d = 100.;
    
    
    vec3 p = vec3(0.);
    
    float m = 0.;
    
    
    int steps = 300;
    
    
    
    float t0 = distance(rayO, c1) - r1;
    
    float minStep = (t0 + r1 + r1) / float(steps);
    
    vec3 n = normalize(c1-rayO);
    
    vec3 sunD = normalize(vec3(cos(iTime + 1.7), sin(iTime + 1.7), 0.));
    
    float cValue, cv2, surface;
    
    //trace 1
    for (int i = 0; i <= steps; i++){
        //float nose = cos(iTime) + sin(iTime * 2.) + (.01 * sin(20. * iTime)) + cos(.1 * iTime) + sin(iTime + 1.);
        p = rayO + (rayD * (t + t0));
        
        d = distance(p, c1);
        
        n = (c1 - p) / d;
        
        cValue = (dot(n, sunD) + .7) / 2.;
        
        cv2 = d /r1;
        
        surface = ((cv2 - gr1) / abs(cv2 - gr1) + 1.) / 2.;
        
        col += max(vec4(color2(cv2, 6., .75), color2(cv2, 7., .8) * 1., color2(cv2, 4., .85), 0.) * -sign(d - r1), vec4(0.)) * .008 * cValue * surface;
        
        t += minStep;
    }
        
    
    
    
    
    
    float shadow = dot(n, sunD);
    
    
    gl_FragColor = col;
}

