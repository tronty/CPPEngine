// Written using ChatGPT (GPT4)

// Constants
const float tableWidth = 0.8;
const float tableHeight = 0.4;
const float ballRadius = 0.03;

// Ball colors
const vec4 whiteBallColor = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 ballColor1 = vec4(0.8, 0.0, 0.0, 1.0);
const vec4 ballColor2 = vec4(0.0, 0.8, 0.0, 1.0);
const vec4 ballColor3 = vec4(0.0, 0.0, 0.8, 1.0);

// Ball positions relative to the table (center of the table is at (0.5, 0.5))
const vec2 whiteBallInitialPosition = vec2(0.5 - 0.25 * tableWidth, 0.5);
const vec2 ballPosition1 = vec2(0.5 + 0.25 * tableWidth, 0.5);
const vec2 ballPosition2 = vec2(0.5 + 0.25 * tableWidth + ballRadius * 2.0, 0.5 + ballRadius * sqrt(3.0) / 2.0);
const vec2 ballPosition3 = vec2(0.5 + 0.25 * tableWidth - ballRadius * 2.0, 0.5 + ballRadius * sqrt(3.0) / 2.0);

// Camera properties
const vec3 cameraPosition = vec3(0.5, 0.5, 1.5);
const vec3 cameraTarget = vec3(0.5, 0.5, 0.0);
const float cameraAngle = -0.2;

const float strikeTime = 1.0; // Time (in seconds) it takes for the cue to strike the ball
const float backOffTime = 0.2; // Time (in seconds) it takes for the cue to go back after the strike
const float whiteRollTime = 1.0;
const float ballsRollTime = 1.0;

const float totalTime = (strikeTime + backOffTime + whiteRollTime + ballsRollTime);

vec2 whiteBallPosition()
{
    float t = mod(iTime, totalTime);
    vec2 direction = normalize(ballPosition3 - whiteBallInitialPosition);

    // White ball rolling after being hit
    if (t >= (strikeTime + backOffTime) && t < (strikeTime + backOffTime + whiteRollTime))
    {
        float rollProgress = (t - (strikeTime + backOffTime)) / whiteRollTime;
        vec2 newPosition = whiteBallInitialPosition + direction * rollProgress * (length(whiteBallInitialPosition - ballPosition3) - 2.0 * ballRadius);
        return newPosition;
    }
    // White ball goes back a bit after the hit at a 30-degree angle
    else if (t >= (strikeTime + backOffTime + whiteRollTime) && t < totalTime)
    {
        float backOffProgress = (t - (strikeTime + backOffTime + whiteRollTime)) / ballsRollTime;
        vec2 backOffDirection = vec2(-direction.x * cos(radians(30.0)) - direction.y * sin(radians(30.0)), direction.x * sin(radians(30.0)) + direction.y * cos(radians(30.0)));
        vec2 newPosition = (ballPosition3 - direction * 2.0 * ballRadius) + backOffDirection * backOffProgress * ballRadius * 2.0;
        return newPosition;
    }
    // White ball remains in its initial position or at the end of the backoff motion
    else
    {
        return whiteBallInitialPosition;
    }
}

vec2 ballPosition3Updated()
{
    float t = mod(iTime, totalTime);

    // Time intervals for the ball movement
    float forwardTime = 0.1;
    float backwardTime = 0.2;
    float rollTime = ballsRollTime * 0.4;

    vec2 direction = normalize(whiteBallInitialPosition - ballPosition3);

    if (t >= (strikeTime + backOffTime + whiteRollTime) && t < (strikeTime + backOffTime + whiteRollTime + forwardTime))
    {
        float forwardProgress = (t - (strikeTime + backOffTime + whiteRollTime)) / forwardTime;
        vec2 newPosition = ballPosition3 + direction * forwardProgress * ballRadius * 0.5;
        return newPosition;
    }
    else if (t >= (strikeTime + backOffTime + whiteRollTime + forwardTime) && t < (strikeTime + backOffTime + whiteRollTime + forwardTime + backwardTime))
    {
        vec2 forwardPosition = ballPosition3 + direction * ballRadius * 0.5;
        float backwardProgress = (t - (strikeTime + backOffTime + whiteRollTime + forwardTime)) / backwardTime;
        vec2 backwardDirection = vec2(direction.x * cos(radians(-20.0)) - direction.y * sin(radians(-20.0)), direction.x * sin(radians(-20.0)) + direction.y * cos(radians(-20.0)));
        vec2 newPosition = forwardPosition + backwardDirection * backwardProgress * ballRadius;
        return newPosition;
    }
    else if (t >= (strikeTime + backOffTime + whiteRollTime + forwardTime + backwardTime) && t < (strikeTime + backOffTime + whiteRollTime + forwardTime + backwardTime + rollTime))
    {
        vec2 backwardPosition = ballPosition3 + direction * ballRadius * 0.5 + vec2(direction.x * cos(radians(-50.0)) - direction.y * sin(radians(-20.0)), direction.x * sin(radians(-20.0)) + direction.y * cos(radians(-20.0))) * ballRadius;
        float rollProgress = (t - (strikeTime + backOffTime + whiteRollTime + forwardTime + backwardTime)) / rollTime;
        vec2 newPosition = backwardPosition - direction * rollProgress * ballRadius * 2.0;
        return newPosition;
    }
    else
    {
        return ballPosition3;
    }
}



float cueDist()
{
    float maxDist = 0.3; // Maximum distance the cue moves back before striking
    float backOffDist = 0.01; // Distance the cue moves back after striking the ball

    float t = mod(iTime, totalTime);

    if (t < strikeTime)
    {
        return mix(maxDist, 0.0, t / strikeTime);
    }
    else if (t >= strikeTime)// + backOffTime)
    {
        return 0.01;
    }
    else
    {
        float backOffProgress = (t - strikeTime) / backOffTime;
        return mix(0.01, backOffDist, backOffProgress);
    }
}



vec4 drawBall(in vec2 uv, in vec2 ballCenter, in vec4 ballColor)
{
    // Calculate the distance from the center of the ball
    float distanceToBall = length(uv - ballCenter);

    // Check if the point is inside the ball
    bool insideBall = distanceToBall <= ballRadius;

    if (!insideBall)
    {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }

    // Calculate the z-coordinate of the point on the ball's surface
    float z = sqrt(ballRadius * ballRadius - distanceToBall * distanceToBall);

    // Calculate the surface normal (in 3D)
    vec3 surfaceNormal = normalize(vec3(uv - ballCenter, z));

    // Define the light source
    vec3 lightPosition = vec3(0.5, 0.5, 1.0);
    vec3 lightDirection = normalize(lightPosition - vec3(ballCenter, z));

    // Calculate Lambertian shading
    float lambert = max(dot(surfaceNormal, lightDirection), 0.0);

    // Define ambient light
    float ambient = 0.3;

    // Calculate the final color
    vec4 color = (ambient + lambert) * ballColor;

    return color;
}

// Simple random function for creating the noise
float rand(vec2 n)
{
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

vec4 drawTable(in vec2 uv)
{
    // Table color
    vec4 tableColor = vec4(0.0, 0.5, 0.0, 1.0);

    // Table border color
    vec4 borderColor = vec4(0.6, 0.3, 0.0, 1.0);

    // Table border size
    float borderSize = 0.01;

    // Check if the point is inside the table border
    bool insideBorder = uv.x >= (0.5 - tableWidth / 2.0 - borderSize) && uv.x <= (0.5 + tableWidth / 2.0 + borderSize)
                        && uv.y >= (0.5 - tableHeight / 2.0 - borderSize) && uv.y <= (0.5 + tableHeight / 2.0 + borderSize);

    // Check if the point is inside the table
    bool insideTable = uv.x >= (0.5 - tableWidth / 2.0) && uv.x <= (0.5 + tableWidth / 2.0)
                       && uv.y >= (0.5 - tableHeight / 2.0) && uv.y <= (0.5 + tableHeight / 2.0);

    // Calculate the color of the point
    vec4 color = insideBorder ? (insideTable ? tableColor : borderColor) : vec4(0.0, 0.0, 0.0, 0.0);

    // Add baize texture to the table surface
    if (insideTable)
    {
        float noise = rand(uv * 25.0);
        color.rgb *= (1.0 - 0.1 * noise);
    }

    // Add shading to the borders
    if (!insideTable && insideBorder)
    {
        vec2 borderNormal;
        if (abs(uv.x - (0.5 - tableWidth / 2.0)) <= borderSize || abs(uv.x - (0.5 + tableWidth / 2.0)) <= borderSize)
        {
            borderNormal = vec2(1.0, 0.0);
        }
        else
        {
            borderNormal = vec2(0.0, 1.0);
        }

        vec2 lightDirection = normalize(vec2(0.5, 1.0) - uv);
        float lambert = max(dot(borderNormal, lightDirection), 0.0);
        color.rgb *= (0.4 + 0.6 * lambert);
    }

    return color;
}


vec2 project3DTo2D(in vec3 p)
{
    return p.xy;
}
vec4 drawCue(in vec2 uv, in float dist)
{
    // Calculate the direction from the white ball to the center of the triangle formed by the other three balls
    vec2 triangleCenter = (ballPosition1 + ballPosition2 + ballPosition3) / 3.0;
    vec2 direction = normalize(triangleCenter - whiteBallInitialPosition);

    // Calculate the cue start and end positions
    vec2 cueStart = whiteBallInitialPosition - direction * (ballRadius + dist);
    vec2 cueEnd = cueStart - direction * (ballRadius * 15.0);

    // Define the cue width
    float cueWidth = 0.005;

    // Calculate the distance from the point (uv) to the cue line
    float distanceToCue = length((uv - cueStart) - dot(uv - cueStart, cueEnd - cueStart) / dot(cueEnd - cueStart, cueEnd - cueStart) * (cueEnd - cueStart));

    // Check if the point is inside the cue
    bool insideCue = distanceToCue <= cueWidth;

    // Check if the point is between the start and end positions
    bool betweenStartAndEnd = dot(uv - cueStart, cueEnd - cueStart) >= 0.0 && dot(uv - cueEnd, cueStart - cueEnd) >= 0.0;

    // Calculate the color of the point
    vec4 color = insideCue && betweenStartAndEnd ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 0.0);

    // Add shading to the cue
    if (insideCue && betweenStartAndEnd)
    {
        float shading = 0.5 + 0.5 * (1.0 - distanceToCue / cueWidth);
        color.rgb *= shading;
    }

    // Calculate the shadow projection on the table
    float shadowWidth = cueWidth * 3.0;
    float shadowDistanceToCue = length((uv - cueStart) - dot(uv - cueStart, cueEnd - cueStart) / dot(cueEnd - cueStart, cueEnd - cueStart) * (cueEnd - cueStart)) - cueWidth;
    bool insideShadow = shadowDistanceToCue <= shadowWidth && betweenStartAndEnd;

    // Add the shadow over the table
    if (insideShadow)
    {
        float shadowFactor = 1.0 - 0.3 * (1.0 - shadowDistanceToCue / shadowWidth);
        color.rgb *= shadowFactor;
    }

    return color;
}

vec4 drawCueShadow(in vec2 uv, in float dist)
{
    // Calculate the direction from the white ball to the center of the triangle formed by the other three balls
    vec2 triangleCenter = (ballPosition1 + ballPosition2 + ballPosition3) / 3.0;
    vec2 direction = normalize(triangleCenter - whiteBallInitialPosition);

    // Calculate the cue start and end positions
    vec2 cueStart = whiteBallInitialPosition - direction * (ballRadius + dist);
    vec2 cueEnd = cueStart - direction * (ballRadius * 15.0);

    // Define the shadow width and offset
    float shadowWidth = 0.008;
    vec2 shadowOffset = vec2(0.002, -0.002);

    // Calculate the distance from the point (uv) to the cue line
    float distanceToShadow = length((uv - cueStart - shadowOffset) - dot(uv - cueStart - shadowOffset, cueEnd - cueStart) / dot(cueEnd - cueStart, cueEnd - cueStart) * (cueEnd - cueStart));

    // Check if the point is inside the shadow
    bool insideShadow = distanceToShadow <= shadowWidth;

    // Check if the point is between the start and end positions
    bool betweenStartAndEnd = dot(uv - cueStart - shadowOffset, cueEnd - cueStart) >= 0.0 && dot(uv - cueEnd - shadowOffset, cueStart - cueEnd) >= 0.0;

    // Calculate the color of the point
    vec4 color = insideShadow && betweenStartAndEnd ? vec4(0.0, 0.0, 0.0, 0.3) : vec4(0.0, 0.0, 0.0, 0.0);

    return color;
}

vec4 drawBallShadow(in vec2 uv, in vec2 ballPosition, in vec4 ballColor)
{
    // Define the shadow offset and size
    vec2 shadowOffset = vec2(0.003, -0.003);
    float shadowSize = ballRadius * 1.2;

    // Calculate the distance from the point (uv) to the ball position
    float distanceToShadow = length(uv - ballPosition - shadowOffset);

    // Check if the point is inside the shadow
    bool insideShadow = distanceToShadow <= shadowSize;

    // Calculate the shadow intensity based on the distance to the shadow
    float shadowIntensity = smoothstep(shadowSize, shadowSize * 0.9, distanceToShadow);

    // Calculate the color of the point
    vec4 color = insideShadow ? vec4(0.0, 0.0, 0.0, shadowIntensity * 0.5) : vec4(0.0, 0.0, 0.0, 0.0);

    return color;
}

vec4 drawScene(in vec3 p)
{
    // Project the 3D point to 2D
    vec2 uv = project3DTo2D(p);

    // Draw the billiard table
    vec4 color = drawTable(uv);

    // Draw the white ball and its shadow
    vec4 whiteBall = drawBall(uv, whiteBallPosition(), whiteBallColor);
    vec4 whiteBallShadow = drawBallShadow(uv, whiteBallPosition(), whiteBallColor);
    color = mix(color, whiteBallShadow, whiteBallShadow.a);

    // Draw the other balls and their shadows
    vec4 ball1 = drawBall(uv, ballPosition1, ballColor1);
    vec4 ball1Shadow = drawBallShadow(uv, ballPosition1, ballColor1);
    color = mix(color, ball1Shadow, ball1Shadow.a);

    vec4 ball2 = drawBall(uv, ballPosition2, ballColor2);
    vec4 ball2Shadow = drawBallShadow(uv, ballPosition2, ballColor2);
    color = mix(color, ball2Shadow, ball2Shadow.a);

    vec4 ball3 = drawBall(uv, ballPosition3Updated(), ballColor3);
    vec4 ball3Shadow = drawBallShadow(uv, ballPosition3Updated(), ballColor3);
    color = mix(color, ball3Shadow, ball3Shadow.a);

    // Draw the cue and its shadow
    vec4 cue = drawCue(uv, cueDist());
    vec4 cueShadow = drawCueShadow(uv, cueDist());
    color = mix(color, cueShadow, cueShadow.a);

    // Combine the colors
    color = mix(color, whiteBall, whiteBall.a);
    color = mix(color, ball1, ball1.a);
    color = mix(color, ball2, ball2.a);
    color = mix(color, ball3, ball3.a);
    color = mix(color, cue, cue.a);

    return color;
}





void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0 / iResolution.xy;

    // Set up the camera
    float angle = cameraAngle;
    mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    vec3 p = vec3(uv, 0.0);
    p.xy = rotation * (p.xy - 0.5) + 0.5;
    p += cameraPosition - cameraTarget;

    // Draw the scene
    vec4 color = drawScene(p);

    // Output to screen
    gl_FragColor = color;
}


