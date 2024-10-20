// Define constants for the saucer shape and light effects
const vec2 screenSize = vec2(800.0, 600.0); // screen resolution (set to a fixed value)
const vec2 saucerCenter = vec2(400.0, 300.0); // saucer's position (centered on screen)
const float saucerRadius = 100.0; // outer radius of the saucer
const float saucerInnerRadius = 60.0; // inner radius (for the "cockpit" bubble)
const float saucerHeight = 40.0; // height of the saucer

void main()
{
    // Get the fragment position normalized relative to the center of the screen
    vec2 uv = (xlv_TEXCOORD0.xy - screenSize.xy / 2.0) / min(screenSize.x, screenSize.y);
    
    // Translate UV coordinates so the saucer is centered in the middle of the screen
    vec2 pos = xlv_TEXCOORD0.xy - saucerCenter;
    
    // Distance from the center of the saucer
    float dist = length(pos);
    
    // Flying saucer's base ellipse-like shape
    float ellipseShape = saucerRadius * sqrt(1.0 - (pos.y / saucerHeight) * (pos.y / saucerHeight));

    // Create the saucer body by checking if the fragment is within the saucer's shape
    if (dist < ellipseShape && dist > saucerInnerRadius)
    {
        // Color for the saucer's body
        gl_FragColor = vec4(0.8, 0.8, 0.8, 1.0); // light gray for the metallic body
    }
    // Create the cockpit (inner circle)
    else if (dist < saucerInnerRadius)
    {
        // Color for the cockpit
        gl_FragColor = vec4(0.1, 0.1, 0.3, 1.0); // dark bluish tint for the glass cockpit
    }
    // Outside of the saucer is transparent (or black depending on blending)
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); // black background
    }
}

