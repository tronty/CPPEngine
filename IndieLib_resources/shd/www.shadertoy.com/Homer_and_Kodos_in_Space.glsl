// Dangerous! Kodos helmet is reflective!
#define ALLOWREFLECTIONS

// Slows things too.. Bends Kodos teeth and arm.
#define ALLOWBENDS

//#define JUSTKODOS
//#define WITHOUTKODOS

#define PI acos(-1.)
#define ZERO (min(iFrame,0))

// Scene Spacings:
// 0 is center.

#define KODOSLIVEHERE 20.0
#define HOMERLIVESHERE -20.0
#define CHARACTERSCALE 0.25
#define CAMERAZOOM 25.0
#define MAXDISTANCE 100.0
#define WHERETHEFLOORLIVES 10

// Color Selectors:
#define ABEAUTIFULBACKGROUND 199.0
#define WHITEASMYSOUL 2.0
#define HOMERYELLOW 1.0
#define TONGUERED 3.0
#define BROWNBEARD 4.0
#define KODOSGREEN 7.0
#define BLACKASNIGHT 0.0
#define BLUEPANTS 5.0
#define GREYSHOES 6.0
#define BLOODREDEYE 99.0
#define YELLOWEYEBALLS 97.0
#define LIVERSPOTS 98.0
#define DROOLBLUE 96.0
#define KODOSVENT 95.0
#define KODOSBASECOLOR 10.0
#define KODOSPINKLIGHTBULB 11.0
#define DONUTBROWN 70.0

// Timings for positions:
#define PRECOMPUTEDTIME -1.
#define PRECOMPUTEDKODOSTIME 1.
#define PRECOMPUTEDBLINKTIME sin(iTime)

int sceneStack[12];

vec2 homersHeadSDF(vec3 p, float swingTime) {
    vec2 head = vec2(sdVerticalCapsule(p,5.0,5.0), HOMERYELLOW);
    
    float leftEyeSpacing = -1.5;
    
    float eyeSize = 1.8;
    
    vec3 origP = p;
    
    vec3 eyePt = p;
    eyePt.x = abs(eyePt.x);
    eyePt = translate(eyePt, vec3(0,-1.5,-1.6));
    eyePt.yz = rotate(eyePt.yz, PI * -0.15);
    vec2 eyes = vec2(sdSphere(translate(eyePt, vec3(abs(leftEyeSpacing),2.5,-4)), eyeSize), WHITEASMYSOUL);
    
    p.x = -abs(p.x);
    
    vec2 leftTopEyelid = vec2(sdSphere(translate(p, vec3(leftEyeSpacing,2.5,-4)), eyeSize+.1), HOMERYELLOW);
    vec2 leftBottomEyelid = vec2(sdSphere(translate(p, vec3(leftEyeSpacing,2.5,-4)), eyeSize+.1), HOMERYELLOW);
    
    vec3 pupilP = eyePt;
    vec2 pupils = vec2(sdSphere(translate(pupilP, vec3(abs(leftEyeSpacing)+0.15,2.15+(swingTime*0.05),-5.3)), 0.5), BLACKASNIGHT);
    
    vec3 eyeSlicePoint = translate(p, vec3(0,3.5,0));
    eyeSlicePoint.yz = rotate(eyeSlicePoint.yz, PI * abs(PRECOMPUTEDBLINKTIME) * -0.25);
    vec2 pln = vec2(sdPlane(eyeSlicePoint, normalize(vec4(0,1,0,1))),1.0);
    
    diff(leftTopEyelid,pln);
    
    vec3 eyeSlicePointB = translate(p, vec3(leftEyeSpacing,1.5,-2));
    eyeSlicePointB.yz = rotate(eyeSlicePointB.yz, PI * abs(PRECOMPUTEDBLINKTIME) * 0.5);
    vec2 plnB = vec2(sdPlane(eyeSlicePointB, normalize(vec4(0,-1,0,1))),1.0);
    
    diff(leftBottomEyelid,plnB);
    
    vec2 nose = vec2(sdCapsule(p, vec3(0.0,1.0,0.0), vec3(0.0,1.0,-6.5), 1.0), HOMERYELLOW);
    
    vec3 neckP = translate(p, vec3(0,-3,1));
    neckP.yz = rotate(neckP.yz, PI * -0.95);
    vec2 neck = vec2(sdRoundCone(neckP, 3.0, 3.5, 5.0), HOMERYELLOW);
    
    // hair
    vec3 a = translate(p, vec3(0,8,-1));
    vec3 b = translate(p, vec3(0,8,1));
    vec2 reset = p.yz;
    
    a.yz = rotate(a.yz, PI * 0.5);
    b.yz = rotate(b.yz, PI * 0.5);
    
    // Top Hairs
    vec2 d5 = vec2(sdTorus(a, vec2(2.75,0.3)), BLACKASNIGHT);
    vec2 d6 = vec2(sdTorus(b, vec2(2.75,0.3)), BLACKASNIGHT);
    
    vec2 lefttEyeBrow = vec2(sdTorus(translate(p, vec3(-1.6,4.5,-4)), vec2(1,0.5)), HOMERYELLOW);
    
    vec2 beard = vec2(sdSphere(translate(p, vec3(0,-2,-3)), 3.5), BROWNBEARD);
    
    vec3 mouthPoint1 = translate(p, vec3(0,-3,-4.0));
    mouthPoint1.yz = rotate(mouthPoint1.yz, PI * 0.15);
    vec2 d11 = vec2(sdTorus(mouthPoint1, vec2(2,0.5)), BROWNBEARD);
    
    beard =  vec2(opSmoothUnion(beard.x, d11.x, 0.25), BROWNBEARD);
    
    vec3 mouthPoint2 = translate(p, vec3(0,-2.15,-4.25));
    mouthPoint2.yz = rotate(mouthPoint2.yz, PI * 0.05);
    vec2 d12 = vec2(sdTorus(mouthPoint2, vec2(2.1,0.7)), BROWNBEARD);
    
    beard =  vec2(opSmoothUnion(beard.x, d12.x, 0.75), BROWNBEARD);
    
    vec3 mouthPoint3 = translate(p, vec3(0,-3.5,-4.25));
    mouthPoint3.yz = rotate(mouthPoint3.yz, PI * 0.05);
    vec2 cylMouth = vec2(sdTorus(mouthPoint3, vec2(2.1,0.7)), BLACKASNIGHT);
    
    vec3 earPoint1 = translate(p, vec3(-5,2,0));
    earPoint1.xy = rotate(earPoint1.xy, PI * 0.5);
    earPoint1.yz = rotate(earPoint1.yz, PI * -0.15);
    earPoint1.x *= 0.75;
    vec2 leftEar = vec2(sdTorus(earPoint1, vec2(0.75,0.4)), HOMERYELLOW);
    
    vec3 sidehairP = p;
    vec2 d15 = vec2(sdCapsule(sidehairP, vec3(-5,6,-1), vec3(-4.75,4,-2), 0.25), BLACKASNIGHT);
    vec2 d16 = vec2(sdCapsule(sidehairP, vec3(-5,6,-1), vec3(-5,4,0), 0.25), BLACKASNIGHT);
    vec2 d17 = vec2(sdCapsule(sidehairP, vec3(-5,6,1), vec3(-5,4,0), 0.25), BLACKASNIGHT);
    vec2 d18 = vec2(sdCapsule(sidehairP, vec3(-5,6,1), vec3(-5,4,2), 0.25), BLACKASNIGHT);
    
    
    // Yellow Parts first
    head = vec2(opSmoothUnion(head.x, lefttEyeBrow.x, 0.75), HOMERYELLOW);
    head = vec2(opSmoothUnion(head.x, neck.x, 1.0), HOMERYELLOW);
    
    
    add(head, nose);
    add(head, eyes);
    
    add(head, d5);
    add(head, d6);
    
    vec3 mouthPoint = p;
    
    mouthPoint = translate(p, vec3(0,-3.5,-5));
    mouthPoint.yz = rotate(mouthPoint.yz, PI * 0.05);
    vec2 openMouth = vec2(sdCappedCylinder(mouthPoint, vec2(3,0.5)), BLACKASNIGHT);
    
    vec3 teethPoint = p;
    teethPoint = translate(p, vec3(0,-3.5,-5));
    teethPoint.yz = rotate(teethPoint.yz, PI * 0.1);
    vec2 teethTop = vec2(sdCappedCylinder(translate(teethPoint, vec3(0,0.85,1.75)), vec2(2.5,0.5)), WHITEASMYSOUL);
    vec2 teethBottom = vec2(sdCappedCylinder(translate(teethPoint, vec3(0,-0.75,2)), vec2(2,0.25)), WHITEASMYSOUL);
    
    vec2 slicePln = vec2(sdPlane(translate(mouthPoint, vec3(0,0,-1)), normalize(vec4(0,0,-1,1))), BLACKASNIGHT);
    diff(openMouth,slicePln);
    
    vec3 mouthPointA = mouthPoint;
    mouthPointA.xy = rotate(mouthPointA.xy, PI * 0.5);
    
    vec2 openMouthA = vec2(sdCappedCylinder(mouthPointA, vec2(0.75+ swingTime * 0.10, 3)), BLACKASNIGHT);
    add(openMouth, openMouthA);
    
    beard = vec2(opSmoothSubtraction(openMouth.x, beard.x, 0.8), BROWNBEARD);
    
    vec3 tonguePoint = translate(p, vec3(0,-3.75,-4.5));
    tonguePoint.x *= .5;
    tonguePoint.yz = rotate(tonguePoint.yz, PI * -0.5);
    
    vec2 tongue = vec2(sdRoundCone(tonguePoint, 0.25, 0.5, 1.75), TONGUERED);
    
    vec2 subHole = vec2(sdSphere(translate(p, vec3(0,-2.75,-2.85)), 2.25), BLACKASNIGHT);
    
    vec2 hollowHole = vec2(sdSphere(translate(p, vec3(0,-2.75,-2.75)), 2.25), BLACKASNIGHT);
    vec2 slicePlnH = vec2(sdPlane(translate(p, vec3(0,0,-3.2)), normalize(vec4(0,0,1,1))), BLACKASNIGHT);
    
    diff(hollowHole, slicePlnH);
    
    hollowHole = vec2(opOnion(hollowHole.x,0.05), BLACKASNIGHT);
    
    diff(beard, subHole);
    
    add(beard, tongue);
    add(beard, hollowHole);
    
    add(beard, teethTop);
    add(beard, teethBottom);
    
    add(head, beard);
    
    // ears
    add(head, leftEar);
    
    // hairs
    add(head, d15);
    add(head, d16);
    add(head, d17);
    add(head, d18);
    
    add(head, leftTopEyelid);
    add(head, leftBottomEyelid);
    
    add(head, pupils);
    
    return head;
}

void homerTorsoSDF(vec3 p, float swingTime, inout vec2 head) {
    float bounce = (clamp(swingTime,0.0,-1.0) * 0.75);
    vec3 collarPoint = translate(p, vec3(0,-8,1.75));
    
    vec2 collar = vec2(onion(sdCappedCone(collarPoint, 3.0, 5.5, 3.5), 0.03), WHITEASMYSOUL);
    vec2 collarTopSlice = vec2(sdPlane(translate(collarPoint, vec3(0,-0.5,0)), normalize(vec4(0,1,0,1))), WHITEASMYSOUL);
    vec2 collarBottomSlice = vec2(sdPlane(translate(collarPoint, vec3(0,0,0)), normalize(vec4(0,-1,0,1))), WHITEASMYSOUL);
    
    diff(collar, collarTopSlice);
    diff(collar, collarBottomSlice);
    
    vec2 collarInside = vec2(onion(sdCappedCone(collarPoint, 3.0, 3.5, 4.25), 0.03), WHITEASMYSOUL);
    vec2 collarInsideTopSlice = vec2(sdPlane(translate(collarPoint, vec3(0,0,0)), normalize(vec4(0,1,0,1))), WHITEASMYSOUL);
    vec2 collarInsideBottomSlice = vec2(sdPlane(translate(collarPoint, vec3(0,0,0)), normalize(vec4(0,-1,0,1))), WHITEASMYSOUL);
    
    diff(collarInside, collarInsideTopSlice);
    diff(collarInside, collarInsideBottomSlice);
    
    vec2 frontPiece = vec2(sdCappedCone(translate(collarPoint, vec3(0,0,-5)), 3.0, 3.0, 2.0), WHITEASMYSOUL);
    
    vec2 frontPieceInside = vec2(sdCappedCone(translate(collarPoint, vec3(0,2,-4)), 3.0, 0.0, 5.0), WHITEASMYSOUL);
    
    diff(collar, frontPiece);
    add(head, collar);
    
    diff(collarInside, frontPieceInside);
    
    vec2 bodyTop = vec2(sdCappedCone(translate(p, vec3(0,-12,2)), 2.6, 6.0, 4.), WHITEASMYSOUL);
    
    collarInside = vec2(opSmoothUnion(collarInside.x, bodyTop.x, 1.0), WHITEASMYSOUL);
    
    vec3 tummyPoint = translate(p, vec3(0,-22,2));
    vec2 tummy = vec2(sdRoundCone(tummyPoint, 10.0-bounce, 3.0, 8.0), WHITEASMYSOUL);
    collarInside = vec2(opSmoothUnion(collarInside.x, tummy.x, 1.0), WHITEASMYSOUL);
    
    vec2 tummySlice = vec2(sdPlane(translate(collarPoint, vec3(0.0,-13.0+bounce,0.0)), normalize(vec4(0,1,0,1))), WHITEASMYSOUL);
    diff(collarInside, tummySlice);
    
    add(head, collarInside);
}

void makeALeg(vec3 lP, vec3 leftLegTopPoint, float swingTime, inout vec2 bottom) {
    
    vec3 leftLegBottomPoint = translate(leftLegTopPoint, vec3(0,-10, 0));
    
    if (swingTime > 0.0) {
        leftLegTopPoint.yz = rotate(leftLegTopPoint.yz, PI * swingTime * -0.23);
        leftLegTopPoint = translate(leftLegTopPoint, vec3(0.0,swingTime * -5.0, swingTime * -8.0));
        leftLegBottomPoint.yz = rotate(leftLegBottomPoint.yz, PI * clamp(swingTime,0.0,1.0) * 0.23);
        leftLegBottomPoint = translate(leftLegBottomPoint, vec3(0,clamp(swingTime,0.0,1.0) * 7.0,0.0));
    }
    
    vec2 leftLegTop = vec2(sdRoundCone(leftLegTopPoint, 4.0, 4.0, 8.0), BLUEPANTS);
    vec2 leftLegBottom = vec2(sdRoundCone(leftLegBottomPoint, 4.0, 4.0, 8.0), BLUEPANTS);
    leftLegTop = vec2(opSmoothUnion(leftLegTop.x, leftLegBottom.x, 1.0), BLUEPANTS);
    
    vec3 leftCapPoint = translate(lP, vec3(leftLegTopPoint.x + lP.x,-27, 0));
    
    if (swingTime > 0.0) {
        leftCapPoint = translate(leftCapPoint, vec3(0,clamp(swingTime,0.0,1.0) * 6.0,clamp(swingTime,0.0,1.0) *-3.5));
        leftCapPoint.yz = rotate(leftCapPoint.yz, PI * clamp(swingTime,0.0,1.0) * 0.23);
    }
    
    vec2 leftCap = vec2(sdCappedCylinder(leftCapPoint, vec2(4, 1)), BLACKASNIGHT);
    add(leftLegTop, leftCap);
    
    bottom = vec2(opSmoothUnion(bottom.x, leftLegTop.x, 1.0), BLUEPANTS);
}

void makeAShoe(vec3 leftCapPoint, float swingTime, inout vec2 bottom, bool right) {
    if (swingTime > 0.0) {
        leftCapPoint = translate(leftCapPoint, vec3(0,clamp(swingTime,0.0,1.0) * 6.0,clamp(swingTime,0.0,1.0) *-3.5));
        leftCapPoint.yz = rotate(leftCapPoint.yz, PI * clamp(swingTime,0.0,1.0) * 0.23);
    }
    vec3 leftShoePoint = translate(leftCapPoint, vec3(0,-4,1));
    
    leftShoePoint.yz = rotate(leftShoePoint.yz, PI * 0.5);
    vec2 leftShoe = vec2(sdRoundCone(leftShoePoint, 3.5, 3.5, 5.0), GREYSHOES);
    
    vec2 leftShoeSlice = vec2(sdPlane(translate(leftCapPoint, vec3(0,-3,0)), normalize(vec4(0,1,0,1))), GREYSHOES);
    diff(leftShoe, leftShoeSlice);
    
    vec3 showSlicePoint = translate(leftCapPoint, vec3(0,-4,0));
    showSlicePoint.xz = rotate(showSlicePoint.xz, PI * 0.5);
    vec2 shoeSlice = vec2(sdTriPrism(showSlicePoint, vec2(1,50)), GREYSHOES);
    diff(leftShoe, shoeSlice);
    add(bottom, leftShoe);
}

void homerBottomSDF(vec3 p, float swingTime, inout vec2 head) {
    float bounce = (clamp(swingTime,0.0,-1.0) * 0.75);
    vec3 pantsPoint = translate(p, vec3(0.0,-20.0 + bounce, 2.0));
    vec2 bottom = vec2(sdSphere(pantsPoint, 9.5), BLUEPANTS);
    vec2 bottomSlice = vec2(sdPlane(translate(pantsPoint, vec3(0,0,0)), normalize(vec4(0,-1,0,1))), WHITEASMYSOUL);
    
    diff(bottom, bottomSlice);
    
    vec3 lP = pantsPoint;
    
    lP.yz = rotate(lP.yz, PI * swingTime * -0.15);
    
    vec3 leftLegTopPoint = translate(lP, vec3(-4.5,-15, 0));
    makeALeg(lP, leftLegTopPoint, swingTime, bottom);
    
    vec3 rP = pantsPoint;
    
    float swingTimeOld = swingTime;
    swingTime = -PRECOMPUTEDTIME;
    rP.yz = rotate(rP.yz, 3.141592657 * swingTime * -0.15);
    
    vec3 rightLegTopPoint = translate(rP, vec3(4.5,-15, 0));
    makeALeg(rP, rightLegTopPoint, swingTime, bottom);
    
    vec3 leftCapPoint = translate(lP, vec3(leftLegTopPoint.x + lP.x,-27, 0));
    makeAShoe(leftCapPoint, swingTimeOld, bottom, false);
    vec3 rightCapPoint = translate(rP, vec3(rightLegTopPoint.x + rP.x,-27, 0));
    makeAShoe(rightCapPoint, swingTime, bottom, true);
    add(head, bottom);
}

vec2 makeAnArm(vec3 leftArmPoint) {
    vec2 leftArmSlice = vec2(sdPlane(translate(leftArmPoint, vec3(0,0.0,0)), normalize(vec4(0,1,0,1))), WHITEASMYSOUL);
    vec2 leftArm = vec2(sdSphere(leftArmPoint, 3.0), WHITEASMYSOUL);
    diff(leftArm, leftArmSlice);
    
    vec2 leftArmA = vec2(sdRoundCone( translate(leftArmPoint, vec3(0,-5,0)), 2.5, 2.5, 3.0), HOMERYELLOW);
    
    vec3 leftArmPointB = leftArmPoint;
    leftArmPointB.yz = rotate(leftArmPointB.yz, PI * -0.25);
    vec2 leftArmB = vec2(sdRoundCone( translate(leftArmPointB, vec3(0,-9,4)), 2., 2.5, 3.0), HOMERYELLOW);
    
    
    vec3 Lthumbpoint = translate(leftArmPointB, vec3(0,-13,0));
    Lthumbpoint.yz = rotate(Lthumbpoint.yz, PI * 0.75);
    vec2 leftArmBT = vec2(sdRoundCone( translate(Lthumbpoint, vec3(0,-5,1)), 0.5, 0.5, 3.0), HOMERYELLOW);
    
    vec3 b1pt =  translate(leftArmPointB, vec3(-0.6,-13,4));
    b1pt.xy = rotate(b1pt.xy, PI * -0.15);
    vec2 leftArmB1 = vec2(sdRoundCone( translate(b1pt, vec3(1.6,-.6,0)), 0.5, 0.5, 3.0), HOMERYELLOW);
    vec2 leftArmB2 = vec2(sdRoundCone( translate(b1pt, vec3(1.6,-.6,1.5)), 0.5, 0.5, 3.0), HOMERYELLOW);
    vec2 leftArmB3 = vec2(sdRoundCone( translate(b1pt, vec3(1.6,-.6,-1.5)), 0.5, 0.5, 3.0), HOMERYELLOW);
    
    leftArmB = vec2(opSmoothUnion(leftArmB.x, leftArmBT.x, 1.0), HOMERYELLOW);
    leftArmB = vec2(opSmoothUnion(leftArmB.x, leftArmB1.x, 1.0), HOMERYELLOW);
    leftArmB = vec2(opSmoothUnion(leftArmB.x, leftArmB2.x, 1.0), HOMERYELLOW);
    leftArmB = vec2(opSmoothUnion(leftArmB.x, leftArmB3.x, 1.0), HOMERYELLOW);
    
    leftArmA = vec2(opSmoothUnion(leftArmA.x, leftArmB.x, 1.0), HOMERYELLOW);
    
    add(leftArm, leftArmA);
    return leftArm;
}

void homerArmsSDF(vec3 p, float swingTime, inout vec2 head) {
    vec3 leftArmPoint = translate(p, vec3(-6,-13,0));
    leftArmPoint.xy = rotate(leftArmPoint.xy, PI * 0.25);
    leftArmPoint.yz = rotate(leftArmPoint.yz, PI * PRECOMPUTEDTIME * -0.15);
    
    vec2 leftArm = makeAnArm(leftArmPoint);
    
    add(head, leftArm);
    
    vec3 rightArmPoint = translate(p, vec3(7,-13,2));
    rightArmPoint.xy = rotate(rightArmPoint.xy, PI * 0.05);
    
    rightArmPoint.yz = rotate(rightArmPoint.yz, PI * PRECOMPUTEDTIME * 0.5);
    
    vec2 rightArm = makeAnArm(rightArmPoint);
    
    add(head, rightArm);
}

vec2 homerSDF(vec3 point) {
    float swingTime = 0.0;
    swingTime = PRECOMPUTEDTIME;
    float bounce = (clamp(swingTime,0.0,-1.0) * 0.75);
    vec3 p = translate(point, vec3(0.0,18.0+bounce, HOMERLIVESHERE));
    vec2 head = homersHeadSDF(p, swingTime);
    homerTorsoSDF(p, swingTime, head);
    homerBottomSDF(p, swingTime, head);
    homerArmsSDF(p, swingTime, head);
    return head;
}

void makeATooth(vec3 origin, vec3 location, vec3 shape, inout vec2 head) {
    vec3 toothPoint = translate(origin, location);
    toothPoint.yz = opBend(toothPoint.yz, 0.05);
    vec2 tooth = vec2(sdCappedCone(toothPoint, shape.x,shape.y, shape.z), WHITEASMYSOUL);
    add(head, tooth);
}

void makeMouthSide(vec3 origin, vec3 location, float xyRot, float yxRot, inout vec2 head) {
    vec3 leftSideMouthPoint = translate(origin, location);
    leftSideMouthPoint.xy = rotate(leftSideMouthPoint.xy, PI * xyRot);
    leftSideMouthPoint.yz = rotate(leftSideMouthPoint.yz, PI * yxRot);
    vec2 leftSideMouth = vec2(sdTorus(leftSideMouthPoint, vec2(7.6, 1.75)), KODOSGREEN);
    vec2 leftSideMouthSlice = vec2(sdPlane(translate(leftSideMouthPoint, vec3(0,0,0)), normalize(vec4(0,0,1.2,1))), WHITEASMYSOUL);
    diff(leftSideMouth, leftSideMouthSlice);
    head =  vec2(opSmoothUnion(head.x, leftSideMouth.x, 0.55), KODOSGREEN);
    add(head, leftSideMouth);
}

vec2 kodosSDF(vec3 point) {
    float swingTime = 0.0;
    swingTime = PRECOMPUTEDKODOSTIME;
    
    vec3 kodosPoint = translate(point, vec3(4,4,KODOSLIVEHERE));
    vec3 headPoint = translate(kodosPoint, vec3(0,0,0));
    vec3 mouthCapPoint = translate(kodosPoint, vec3(0,6,-5));
    
    vec2 mouthCap = vec2(sdTorus(mouthCapPoint, vec2(4, 4.5)), BLACKASNIGHT);
    mouthCapPoint.xy = rotate(mouthCapPoint.xy, PI * -0.5);
    vec2 mouthCap2 = vec2(sdCappedCylinder(translate(mouthCapPoint, vec3(0,0,-2)), vec2(5.5, 20)), BLACKASNIGHT);
    add(mouthCap, mouthCap2);
    
    vec2 head = vec2(sdRoundCone( headPoint, 9., 11.0, 22.0), KODOSGREEN);
    
    vec2 headTopSlice = vec2(sdPlane(translate(headPoint, vec3(0,29,0)), normalize(vec4(0,-1,0,1))), WHITEASMYSOUL);
    head = vec2(opSmoothSubtraction( headTopSlice.x,head.x, 0.8), KODOSGREEN);
    
    vec3 headInsidePoint = translate(headPoint, vec3(0,7,-2));
    headInsidePoint.xz = rotate(headInsidePoint.xz, PI * 0.5);
    vec2 headInside = vec2(sdEllipsoid( headInsidePoint, vec3(4., 10.0, 10.0)), BLACKASNIGHT);
    
    diff(head,mouthCap);
    
    vec3 eyePoint = translate(kodosPoint, vec3(0,18,-7.5));
    
    vec2 eye = vec2(sdSphere(eyePoint, 5.0),97.0);
    
    
    makeMouthSide(kodosPoint, vec3(5,6.2,-5), -0.46, -0.3, head);
    makeMouthSide(kodosPoint, vec3(-5,6.2,-5), -0.55, 0.3, head);
    
    vec3 lipPoint = translate(kodosPoint, vec3(0,12,-5));
    lipPoint.xz = rotate(lipPoint.xz, PI * -0.5);
    lipPoint.xy = rotate(lipPoint.xy, PI * 0.35);
    vec2 lip = vec2(sdEllipsoid( lipPoint, vec3(3., 7.0, 10.0)), KODOSGREEN);
    head =  vec2(opSmoothUnion(head.x, lip.x, 0.55), KODOSGREEN);
    
    vec3 lipPointB = translate(kodosPoint, vec3(0,-0.4,-3.));
    lipPointB.xz = rotate(lipPointB.xz, PI * -0.5);
    lipPointB.xy = rotate(lipPointB.xy, PI * -0.45);
    vec2 lipB = vec2(sdEllipsoid( lipPointB, vec3(3., 7.0, 10.0)), KODOSGREEN);
    head =  vec2(opSmoothUnion(head.x, lipB.x, 0.55), KODOSGREEN);
    
    add(head, headInside);
    
    makeATooth(kodosPoint,vec3(-3,4,-7), vec3(2.5, 1.5, 0.0), head);
    makeATooth(kodosPoint,vec3(-7,3,-5), vec3(2.5, 1.5, 0.0), head);
    makeATooth(kodosPoint,vec3(3,4,-7), vec3(2.5, 1.5, 0.0), head);
    makeATooth(kodosPoint,vec3(7,3,-5), vec3(2.5, 1.5, 0.0), head);
    makeATooth(kodosPoint,vec3(-2,6,-8.5), vec3(3.5,0.0, 1.5), head);
    makeATooth(kodosPoint,vec3(-6,7,-6.5), vec3(3., 0.0, 1.5), head);
    makeATooth(kodosPoint,vec3(2,6,-8.5), vec3(3.5, 0.0, 1.5), head);
    makeATooth(kodosPoint,vec3(6,7,-6.5), vec3(3., 0.0, 1.5), head);
    
    vec3 tooth1Point = translate(kodosPoint, vec3(-3,4,-7));
    vec3 droolPoint = translate(tooth1Point, vec3(-2,-2,1));
    droolPoint.yz = rotate(droolPoint.yz, PI * 0.25);
    vec2 drool = vec2(sdEllipsoid(droolPoint, vec3(2., 2., 2.0)), DROOLBLUE);
    
    vec3 drool1Point = translate(tooth1Point, vec3(-2,-2,-1));
    drool1Point.yz = rotate(drool1Point.yz, PI * 0.25);
    vec2 drool1 = vec2(sdEllipsoid(drool1Point, vec3(1., 1., 2.0)), DROOLBLUE);
    drool = vec2(opSmoothUnion(drool.x, drool1.x, 1.0), DROOLBLUE);
    add(head, drool);
    
    vec3 drool2Point = translate(drool1Point, vec3(-1,-2,-5));
    drool2Point.xy = rotate(drool2Point.xy, PI * -0.75);
    vec2 drool2 = vec2(sdEllipsoid(drool2Point, vec3(0.5, 0.5, 1.)), DROOLBLUE);
    add(head, drool2);
    
    eyePoint.xz = rotate(eyePoint.xz, PI * -0.025);
    vec3 pupilPoint = translate(eyePoint, vec3(0,0,-4.5));
    vec2 pupil = vec2(sdVesica(opRevolution(pupilPoint,0.15), 3., 2. ), BLOODREDEYE);
    eyePoint.xz = rotate(eyePoint.xz, PI * 0.025);
    
    vec3 browPoint = translate(eyePoint, vec3(0,1,-2));
    browPoint.x *= .8;
    browPoint.yz = rotate(browPoint.yz, PI * 0.65);
    vec2 brow = vec2(sdTorus(browPoint, vec2(4.25, 1)), KODOSGREEN);
    add(head, brow);
    
    vec3 browPointB = translate(eyePoint, vec3(0,-1.75,-0.5));
    browPointB.x *= .8;
    browPointB.yz = rotate(browPointB.yz, PI * -0.75);
    vec2 browB = vec2(sdTorus(browPointB, vec2(4.25, 1)), KODOSGREEN);
    add(head, browB);
    
    
    vec3 leftEarPoint = translate(kodosPoint, vec3(-11,15,0));
    vec2 leftEarSlice = vec2(sdPlane(translate(leftEarPoint, vec3(0,0,0)), normalize(vec4(0,0,1,1))), KODOSGREEN);
    leftEarPoint.xy = rotate(leftEarPoint.xy, PI * -0.15);
    vec2 leftEar = vec2(sdRoundCone( leftEarPoint, 2.5, 1.0, 9.0), KODOSGREEN);
    
    vec3 leftEarTubePoint = translate(leftEarPoint, vec3(1.5,0,-1.75));
    leftEarTubePoint.xy = rotate(leftEarTubePoint.xy, PI * -0.05);
    leftEarTubePoint.yz = rotate(leftEarTubePoint.yz, PI * -0.085);
    vec2 leftEarTube = vec2(sdRoundCone( leftEarTubePoint, 1.0, 0.8, 11.0), KODOSGREEN);
    
    diff(leftEar, leftEarSlice);
    add(leftEar, leftEarTube);
    add(head, leftEar);
    
    vec3 rightEarPoint = translate(kodosPoint, vec3(11,15,0));
    vec2 rightEarSlice = vec2(sdPlane(translate(rightEarPoint, vec3(0,0,0)), normalize(vec4(0,0,1,1))), KODOSGREEN);
    rightEarPoint.xy = rotate(rightEarPoint.xy, PI * 0.15);
    vec2 rightEar = vec2(sdRoundCone( rightEarPoint, 2.5, 1.0, 9.0), KODOSGREEN);
    
    vec3 rightEarTubePoint = translate(rightEarPoint, vec3(-1.5,0,-1.75));
    rightEarTubePoint.xy = rotate(rightEarTubePoint.xy, PI * 0.05);
    rightEarTubePoint.yz = rotate(rightEarTubePoint.yz, PI * -0.085);
    vec2 rightEarTube = vec2(sdRoundCone( rightEarTubePoint, 1.0, 0.8, 11.0), KODOSGREEN);
    
    diff(rightEar, rightEarSlice);
    add(rightEar, rightEarTube);
    add(head, rightEar);
    
    add(head, pupil);
    add(head, eye);
    
    
    vec2 baseBottom = vec2(sdRoundCone( translate(kodosPoint, vec3(0,-10,0)), 10.0, 6.0, 9.0), KODOSGREEN);
    add(head, baseBottom);
    
    vec2 liverSpot1 = vec2(sdEllipsoid(translate(kodosPoint, vec3(10,25,0)), vec3(1., 2, 2.0)), LIVERSPOTS);
    add(head, liverSpot1);
    
    vec3 liverSpot2Point = translate(kodosPoint, vec3(-3,26,-8));
    liverSpot2Point.xy = rotate(liverSpot2Point.xy, PI * 0.5);
    liverSpot2Point.yz = rotate(liverSpot2Point.yz, PI * 0.5);
    vec2 liverSpot2 = vec2(sdEllipsoid(liverSpot2Point, vec3(1., 2., 2.0)), LIVERSPOTS);
    add(head, liverSpot2);
    
    vec3 liverSpot3Point = translate(kodosPoint, vec3(4,27,-8));
    liverSpot3Point.xy = rotate(liverSpot3Point.xy, PI * 0.25);
    liverSpot3Point.yz = rotate(liverSpot3Point.yz, PI * 0.5);
    vec2 liverSpot3 = vec2(sdEllipsoid(liverSpot3Point, vec3(2., 1., 1.0)), LIVERSPOTS);
    add(head, liverSpot3);
    
    vec3 liverSpot4Point = translate(kodosPoint, vec3(-7,27,-6));
    liverSpot4Point.xy = rotate(liverSpot4Point.xy, PI * 0.25);
    liverSpot4Point.yz = rotate(liverSpot4Point.yz, PI * 0.5);
    vec2 liverSpot4 = vec2(sdEllipsoid(liverSpot4Point, vec3(1., 1., 2.0)), LIVERSPOTS);
    add(head, liverSpot4);
    
    
    return head;
}

vec2 kodosExteriorSDF(vec3 point){
    float swingTime = 0.0;
    swingTime = PRECOMPUTEDKODOSTIME;
    
    vec3 kodosPoint = translate(point, vec3(3,0,KODOSLIVEHERE));
    vec2 base = vec2(sdCappedCylinder(translate(kodosPoint, vec3(0,-3,0)), vec2(12,3)), KODOSBASECOLOR);
    vec2 lightbulb = vec2(sdSphere(translate(kodosPoint, vec3(-10,-3,-6)), 1.5), KODOSPINKLIGHTBULB);
    vec2 baseBottom = vec2(sdRoundCone( translate(kodosPoint, vec3(0,-13,0)), 12.0, 8.0, 9.0), KODOSGREEN);
    add(base, lightbulb);
    
    vec2 vent = vec2(sdBox(translate(kodosPoint, vec3(0,-3,-11)), vec3(4,1,1)), KODOSVENT);
    add(base, vent);
    
    vec2 vent1 = vec2(sdBox(translate(kodosPoint, vec3(-6.75,-3,-9.5)), vec3(1,1,1)), KODOSVENT);
    add(base, vent1);
    
    vec3 frontRightLegPoint = translate(kodosPoint, vec3(2,-13,-2));
    frontRightLegPoint.xz = opBend(frontRightLegPoint.xz, 0.05);
    frontRightLegPoint.yz = rotate(frontRightLegPoint.yz, PI * 0.6);
    frontRightLegPoint.xy = rotate(frontRightLegPoint.xy, PI * -0.05);
    vec2 frontRightLeg = vec2(sdRoundCone(frontRightLegPoint, 8.0, 2.0, 25.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontRightLeg.x, 1.0), KODOSGREEN);
    
    
    //////
    
    vec3 rightFrontTentaclePoint = translate(kodosPoint, vec3(-2.,-21.,-26));
    vec2 rightFrontTentacle = vec2( sdBezierSansClosestZ(vec3(0.,0.0,0.0), vec3(-3,-5.0,-10.0), vec3(-10.,-5.,-5.0),  rightFrontTentaclePoint, 2.), KODOSGREEN);
    add(base, rightFrontTentacle);
    
    
    ////
    
    vec3 frontLeftLegPoint = translate(kodosPoint, vec3(-2,-13,0));
    frontLeftLegPoint.xz = opBend(frontLeftLegPoint.xz, 0.05);
    frontLeftLegPoint.yz = rotate(frontLeftLegPoint.yz, PI * 0.6);
    frontLeftLegPoint.xy = rotate(frontLeftLegPoint.xy, PI * -0.25);
    vec2 frontLeftLeg = vec2(sdRoundCone(frontLeftLegPoint, 8.0, 2.0, 25.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontLeftLeg.x, 1.0), KODOSGREEN);
    
    
    vec3 leftLegPoint = translate(kodosPoint, vec3(-11.5+(2.* abs(swingTime)),-19,0.+(4.* swingTime)));
    leftLegPoint.xz = rotate(leftLegPoint.xz, PI *0.10*swingTime);
    leftLegPoint.yz = rotate(leftLegPoint.yz, PI * 1.);
    leftLegPoint.xy = rotate(leftLegPoint.xy, PI * -0.35);
    
    
    vec2 leftLeg = vec2(sdCappedCone(leftLegPoint, 7.0, 9.5, 5.5), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, leftLeg.x, 1.0), KODOSGREEN);
    
    vec3 leftArmJointPoint = translate(leftLegPoint, vec3(-7,6.5,0));
    leftArmJointPoint.xy = rotate(leftArmJointPoint.xy, PI * 0.51);
    leftArmJointPoint.yz = rotate(leftArmJointPoint.yz, PI * 0.5);
    vec2 leftArmJoint = vec2(sdTorus(leftArmJointPoint, vec2(7.,5.5)), KODOSGREEN);
    vec2 leftArmSlice = vec2(sdPlane(translate(leftArmJointPoint, vec3(-1.5,-1.5,0)), normalize(vec4(-1,0,0,1))), KODOSGREEN);
    diff(leftArmJoint, leftArmSlice);
    
    vec3 leftArmJointSlice = translate(leftArmJointPoint, vec3(-1,-1,0));
    leftArmJointSlice.xz = rotate(leftArmJointSlice.xz, PI * -0.25);
    vec2 leftArmSliceA = vec2(sdPlane(leftArmJointSlice, normalize(vec4(0,0,1,1))), KODOSGREEN);
    diff(leftArmJoint, leftArmSliceA);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, leftArmJoint.x, 0.1), KODOSGREEN);
    //
    vec3 frontLeftLegAPoint = translate(kodosPoint, vec3(-27.+(2.5* abs(swingTime)),-13.,0.+(9.* swingTime)));
    frontLeftLegAPoint.xz = rotate(frontLeftLegAPoint.xz, PI *0.10*swingTime);
    frontLeftLegAPoint.xy = rotate(frontLeftLegAPoint.xy, PI * 0.125);
    vec2 frontLeftLegA = vec2(sdRoundCone(frontLeftLegAPoint, 5.5, 2.0, 15.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontLeftLegA.x, 0.1), KODOSGREEN);
    
    vec3 frontLeftLegBPoint = translate(kodosPoint, vec3(-22.+(2.25* abs(swingTime)),2.,0.+(7.75* swingTime)));
    frontLeftLegBPoint.xz = rotate(frontLeftLegBPoint.xz, PI *0.10*swingTime);
    frontLeftLegBPoint.xy = rotate(frontLeftLegBPoint.xy, PI * -0.1);
    vec2 frontLeftLegB = vec2(sdRoundCone(frontLeftLegBPoint, 1.95, 1.5, 3.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontLeftLegB.x, 1.), KODOSGREEN);
    
    vec3 rightLegPoint = translate(kodosPoint, vec3(11.5+(2.* -abs(swingTime)),-19.,0.+(4.* -swingTime)));
    rightLegPoint.xz = rotate(rightLegPoint.xz, PI *0.10*swingTime);
    rightLegPoint.yz = rotate(rightLegPoint.yz, PI * 1.);
    rightLegPoint.xy = rotate(rightLegPoint.xy, PI * 0.35);
    vec2 rightLeg = vec2(sdCappedCone(rightLegPoint, 7.0, 9.5, 5.5), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, rightLeg.x, 1.0), KODOSGREEN);
    
    vec3 rightArmJointPoint = translate(rightLegPoint, vec3(7,6.5,0));
    rightArmJointPoint.xy = rotate(rightArmJointPoint.xy, PI * 0.31);
    rightArmJointPoint.yz = rotate(rightArmJointPoint.yz, PI * 0.5);
    vec2 rightArmJoint = vec2(sdTorus(rightArmJointPoint, vec2(7.,5.5)), KODOSGREEN);
    vec2 rightArmSlice = vec2(sdPlane(translate(rightArmJointPoint, vec3(-1.5,-1.5,0)), normalize(vec4(-1,0,0,1))), KODOSGREEN);
    diff(rightArmJoint, rightArmSlice);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, rightArmJoint.x, 0.1), KODOSGREEN);
    
    vec3 frontRightLegAPoint = translate(kodosPoint, vec3(27.+(2.75* -abs(swingTime)),-13.,0.+(8.75* -swingTime)));
    frontRightLegAPoint.xy = rotate(frontRightLegAPoint.xy, PI * -0.125);
    vec2 frontRightLegA = vec2(sdRoundCone(frontRightLegAPoint, 5.5, 2.0, 15.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontRightLegA.x, 0.1), KODOSGREEN);
    
    vec3 frontRightLegBPoint = translate(kodosPoint, vec3(22.+(3.25* -abs(swingTime)),2.,0.+(9.* -swingTime)));
    frontRightLegBPoint.xz = rotate(frontRightLegBPoint.xz, PI *0.10*-swingTime);
    frontRightLegBPoint.xy = rotate(frontRightLegBPoint.xy, PI * 0.1);
    vec2 frontRightLegB = vec2(sdRoundCone(frontRightLegBPoint, 1.95, 1.5, 3.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, frontRightLegB.x, 1.), KODOSGREEN);
    
    
    vec3 bottomRightLegPoint = translate(kodosPoint, vec3(2,-17,0));
    bottomRightLegPoint.yz = rotate(bottomRightLegPoint.yz, PI * -0.6);
    bottomRightLegPoint.xy = rotate(bottomRightLegPoint.xy, PI * 0.15);
    bottomRightLegPoint.xz = rotate(bottomRightLegPoint.xz, PI * -0.15);
    vec2 bottomRightLeg = vec2(sdRoundCone(bottomRightLegPoint, 8.0, 3.0, 20.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, bottomRightLeg.x, 1.0), KODOSGREEN);
    
    vec3 bottomLeftLegPoint = translate(kodosPoint, vec3(-2,-17,0));
    bottomLeftLegPoint.yz = rotate(bottomLeftLegPoint.yz, PI * -0.6);
    bottomLeftLegPoint.xy = rotate(bottomLeftLegPoint.xy, PI * -0.15);
    bottomLeftLegPoint.xz = rotate(bottomLeftLegPoint.xz, PI * -0.15);
    vec2 bottomLeftLeg = vec2(sdRoundCone(bottomLeftLegPoint, 8.0, 3.0, 20.0), KODOSGREEN);
    baseBottom = vec2(opSmoothUnion(baseBottom.x, bottomLeftLeg.x, 1.0), KODOSGREEN);
    
    add(base, baseBottom);
    return base;
}

vec2 kodosGlassSDF(vec3 point) {
    float swingTime = 0.0;
    swingTime = PRECOMPUTEDKODOSTIME;
    
    vec3 kodosPoint = translate(point, vec3(4.,4,KODOSLIVEHERE));
    vec3 headPoint = translate(kodosPoint, vec3(0,0,0));
    vec2 dome = vec2(sdRoundCone( headPoint, 12., 14.0, 19.0), 8.0);
    vec2 domeSlice = vec2(sdPlane(translate(kodosPoint, vec3(0,-4,0)), normalize(vec4(0,1,0,1))), KODOSGREEN);
    diff(dome, domeSlice);
    vec3 pointyBitPoint = translate(kodosPoint, vec3(0,35,0));
    vec2 pointyBit = vec2(sdCappedCone(pointyBitPoint, 4., 2.5, 0.0), 8.0);
    add(dome, pointyBit);
    return dome;
}

vec2 spinningDonut(vec3 point) {
    vec3 donutPoint = translate(point, vec3(0,23,-37));
    donutPoint.yz = rotate(donutPoint.yz, PI * -0.25);
    donutPoint.xy = rotate(donutPoint.xy, PI * iTime);
    vec2 donutBase = vec2(sdTorus(donutPoint, vec2(1.75,1)), DONUTBROWN);
    return donutBase;
}

vec2 sceneWithMaterials(vec3 point){
    vec2 base;
    bool skipKodos = false;
    for (int i = 0; i < 3; ++i) {
        int yayOrNay = sceneStack[i];
        if (i == 0) {
            if (yayOrNay == 1) {
                base = homerSDF(point/CHARACTERSCALE);
                base.x = base.x * CHARACTERSCALE;
            } else {
                base = kodosExteriorSDF(point/CHARACTERSCALE);
                base.x = base.x * CHARACTERSCALE;
                skipKodos = true;
            }
        } else if (i == 1) {
            if (yayOrNay == 1 && !skipKodos) {
                vec2 kodosExterior = kodosExteriorSDF(point/CHARACTERSCALE);
                kodosExterior.x = kodosExterior.x * CHARACTERSCALE;
                add(base, kodosExterior);
            }
        } else if (i == 2) {
            vec2 donut = spinningDonut(point/CHARACTERSCALE);
            donut.x = donut.x * CHARACTERSCALE;
            add(base, donut);
        }
    }
    return base;
}

vec2 sceneWithGlassMaterials(vec3 point){
    vec3 kAdjustPt = point;
    kAdjustPt = translate(kAdjustPt, vec3(-0.25,0,0.05));
    vec2 kodos = kodosGlassSDF(kAdjustPt/CHARACTERSCALE);
    kodos.x = kodos.x * CHARACTERSCALE;
    return kodos;
}

vec2 sceneInsideGlassMaterials(vec3 point){
    vec3 kAdjustPt = point;
    kAdjustPt = translate(kAdjustPt, vec3(-1.75,0,.5));
    kAdjustPt.xz = rotate(kAdjustPt.xz, PI * 0.1);
    vec2 kodos = kodosSDF(kAdjustPt/CHARACTERSCALE);
    kodos.x = kodos.x * CHARACTERSCALE;
    return kodos;
}

//////////////////STARS////////////////////////////
// Nice use of FBM to generate stars! ***
// https://www.shadertoy.com/view/llj3zV
float hash( float n ) { return fract(sin(n)*123.456789); }

vec2 rotateMe( in vec2 uv, float a)
{
    float c = cos( a );
    float s = sin( a );
    return vec2( c * uv.x - s * uv.y, s * uv.x + c * uv.y );
}

float noise( in vec3 p )
{
    vec3 fl = floor( p );
    vec3 fr = fract( p );
    fr = fr * fr * ( 3.0 - 2.0 * fr );
    
    float n = fl.x + fl.y * 157.0 + 113.0 * fl.z;
    return mix( mix( mix( hash( n +   0.0), hash( n +   1.0 ), fr.x ),
                    mix( hash( n + 157.0), hash( n + 158.0 ), fr.x ), fr.y ),
               mix( mix( hash( n + 113.0), hash( n + 114.0 ), fr.x ),
                   mix( hash( n + 270.0), hash( n + 271.0 ), fr.x ), fr.y ), fr.z );
}

float fbm( in vec2 p, float t )
{
    float f;
    f  = 0.5000 * noise( vec3( p, t ) ); p *= 2.1;
    f += 0.2500 * noise( vec3( p, t ) ); p *= 2.2;
    f += 0.1250 * noise( vec3( p, t ) ); p *= 2.3;
    f += 0.0625 * noise( vec3( p, t ) );
    return f;
}

vec3 starryBackground(in vec3 dir)
{
    vec3 n  = abs(dir);
    vec2 uv = ( n.x > n.y && n.x > n.z ) ? dir.yz / dir.x:
    ( n.y > n.x && n.y > n.z ) ? dir.zx / dir.y:
    dir.xy / dir.z;
    
    float f = 0.0;
    
    for(int i = 0 ; i < 10; ++i)
    {
        uv = rotateMe( 1.07 * uv + vec2( 0.7 ), 0.5 );
        
        float t = 50. * uv.x * uv.y + iTime;
        vec2 u = sin( 75. * uv ) * fbm( 10. * uv, 0.0 );
        f += smoothstep( 0.5, 0.55, u.x * u.y ) * ( 0.25 * cos( t ) + 0.75 );
    }
    
    vec3 starColor = vec3( 1.0, 0.7, 0.5 );
    vec3 newColor = f * starColor;
    if (newColor == vec3(0)) {
        newColor = vec3(12.0/255.0,2.0/255.0,69.0/255.0);
    }
    return newColor;
}

//////////////////END STARS////////////////////////////

vec3 GetNormal(vec3 hitPoint) {
    float ep = 0.0002;
#if 0    
    vec3 e=vec3(1e-2,0,0);
    float d = sceneWithMaterials(hitPoint).x;
    return normalize(vec3(sceneWithMaterials(hitPoint + e.xyy).x - d,
                          sceneWithMaterials(hitPoint + e.yxy).x - d,
                          sceneWithMaterials(hitPoint + e.yyx).x - d));
#else
    // inspired by iq who was inspired by klems
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1), ((i>>1)&1), (i&1))-1.0);
        n += e*sceneWithMaterials(hitPoint+e*ep).x;
    }
    return normalize(n);
#endif    
}

vec3 GetNormal1(vec3 hitPoint) {
    float ep = 0.0002;
#if 0    
    vec3 e=vec3(1e-2,0,0);
    float d = sceneWithGlassMaterials(hitPoint).x;
    return normalize(vec3(sceneWithGlassMaterials(hitPoint + e.xyy).x - d,
                          sceneWithGlassMaterials(hitPoint + e.yxy).x - d,
                          sceneWithGlassMaterials(hitPoint + e.yyx).x - d));
#else
    // inspired by iq who was inspired by klems
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1), ((i>>1)&1), (i&1))-1.0);
        n += e*sceneWithGlassMaterials(hitPoint+e*ep).x;
    }
    return normalize(n);
#endif 
}

vec3 GetNormal2(vec3 hitPoint) {
    float ep = 0.0002;
#if 0    
    vec3 e=vec3(1e-2,0,0);
    float d = sceneInsideGlassMaterials(hitPoint).x;
    return normalize(vec3(sceneInsideGlassMaterials(hitPoint + e.xyy).x - d,
                          sceneInsideGlassMaterials(hitPoint + e.yxy).x - d,
                          sceneInsideGlassMaterials(hitPoint + e.yyx).x - d));
#else
    // inspired by iq who was inspired by klems
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1), ((i>>1)&1), (i&1))-1.0);
        n += e*sceneInsideGlassMaterials(hitPoint+e*ep).x;
    }
    return normalize(n);
#endif 
}

vec3 GetColor2(vec2 distAndMaterial, vec3 hitPoint, float traveled) {
    vec3 hitNormal = GetNormal2(hitPoint);
    float mat = 0.0;
    mat = distAndMaterial.y;
    
#ifdef ALLOWREFLECTIONS 
    vec3 baseColor = vec3(12.0/255.0,2.0/255.0,69.0/255.0);
#else
    vec3 baseColor = vec3(220.0/255.0, 220.0/255.0, 220.0/255.0);
#endif
    
    float light = dot(hitNormal, normalize( vec3(0,5,-5)))*.5+.5;
    
    vec3 color = baseColor;
    if (traveled > ABEAUTIFULBACKGROUND) {
        color = baseColor;
    } else if (mat == HOMERYELLOW) {
        baseColor = vec3(1,0.851,0.058);
        color = baseColor * light;
    } else if (mat == WHITEASMYSOUL) {
        baseColor = vec3(1,1,1);
        color = baseColor * light;
    } else if (mat == BLACKASNIGHT) {
        baseColor = vec3(0,0,0);
        color = baseColor;
    } else if (mat == TONGUERED) {
        baseColor = vec3(0.635,0.137,0.173);
        color = baseColor * light;
    } else if (mat == BROWNBEARD) {
        baseColor = vec3(0.819,0.698,0.439);
        color = baseColor * light;
    } else if (mat == BLUEPANTS) {
        baseColor = vec3(0.310,0.463,0.875);
        color = baseColor * light;
    } else if (mat == GREYSHOES) {
        baseColor = vec3(0.259,0.310,0.275);
        color = baseColor * light;
    } else if (mat == KODOSGREEN) {
        baseColor = vec3(0.643,0.691,0.231);
        color = baseColor * light;
    } else if (mat == BLOODREDEYE) {
        baseColor = vec3(221.0/255.0,56.0/255.0,50.0/255.0);
        color = baseColor * light;
    } else if (mat == LIVERSPOTS) {
        baseColor = vec3(145.0/255.0,128.0/255.0,66.0/255.0);
        color = baseColor * light;
    } else if (mat == YELLOWEYEBALLS) {
        baseColor = vec3(252.0/255.0,238.0/255.0,177.0/255.0);
        color = baseColor * light;
    } else if (mat == DROOLBLUE) {
        baseColor = vec3(194.0/255.0,220.0/255.0,199.0/255.0);
        color = baseColor * light;
    }
    
    return color;
}



vec3 GetColor(vec2 distAndMaterial, vec3 hitPoint, float traveled, vec3 dir) {
    vec3 hitNormal = GetNormal(hitPoint);
    float mat = 0.0;
    mat = distAndMaterial.y;

    vec3 baseColor = starryBackground(dir);

    float light = dot(hitNormal, normalize( vec3(0,5,-5)))*.5+.5;
    
    vec3 color = baseColor;
    
    if (traveled > ABEAUTIFULBACKGROUND) {
        color = baseColor;
    } else if (mat == HOMERYELLOW) {
        baseColor = vec3(1,0.851,0.058);
        color = baseColor * light;
    } else if (mat == WHITEASMYSOUL) {
        baseColor = vec3(1,1,1);
        color = baseColor * light;
    } else if (mat == BLACKASNIGHT) {
        baseColor = vec3(0,0,0);
        color = baseColor;
    } else if (mat == TONGUERED) {
        baseColor = vec3(0.635,0.137,0.173);
        color = baseColor * light;
    } else if (mat == BROWNBEARD) {
        baseColor = vec3(0.819,0.698,0.439);
        color = baseColor * light;
    } else if (mat == BLUEPANTS) {
        baseColor = vec3(0.310,0.463,0.875);
        color = baseColor * light;
    } else if (mat == GREYSHOES) {
        baseColor = vec3(0.259,0.310,0.275);
        color = baseColor * light;
    } else if (mat == KODOSGREEN) {
        baseColor = vec3(0.643,0.691,0.231);
        color = baseColor * light;
    } else if (mat == KODOSBASECOLOR) {
        baseColor = vec3(68.0/255.0, 118.0/255.0, 150.0/255.0);
        color = baseColor * light;
    } else if (mat == KODOSPINKLIGHTBULB) {
        baseColor = vec3(208.0/255.0, 145.0/255.0, 200.0/255.0);
        color = baseColor * light;
    } else if (mat == DONUTBROWN) {
        baseColor = vec3(196.0/255.0,147.0/255.0,65.0/255.0);
        color = baseColor * light;
    } else if (mat == KODOSVENT) {
        baseColor = vec3(219.0/255.0,46.0/255.0,32.0/255.0);
        color = baseColor * light;
    }
    
    return color;
}

vec4 RayMarchInteriorGlass(vec3 ro, vec3 dir) {
    float traveled = 0.0;
    vec2 distAndMaterial = vec2(0);
    
    for (int i=ZERO; i < 50; ++i){
        distAndMaterial = sceneInsideGlassMaterials(ro + dir * traveled);
        traveled += distAndMaterial.x;
        if (distAndMaterial.x < .01 || distAndMaterial.x > MAXDISTANCE) {
            break;
        }
    }
    
    vec3 hitPoint = ro + dir * traveled;
    
    vec3 color = vec3(1);
    color = GetColor2(distAndMaterial, hitPoint, traveled);
    return vec4(color, traveled);
}

vec3 RayMarchReflection(vec3 ro, vec3 dir) {
    float traveled = 0.0;
    vec2 distAndMaterial = vec2(0);
    
    for (int i=ZERO; i < 30; ++i){
        distAndMaterial = sceneWithMaterials(ro + dir * traveled);
        traveled += distAndMaterial.x;
        if (distAndMaterial.x < .01 || distAndMaterial.x > MAXDISTANCE) {
            break;
        }
    }
    
    vec3 hitPoint = ro + dir * traveled;
    
    vec3 color = vec3(1);
    color = GetColor(distAndMaterial, hitPoint, traveled, dir);
    return color;
}

vec3 RayMarchGlass(vec2 uv, vec3 ro, vec3 dir) {
    float traveled = 0.0;
    float everythingTraveled = 0.0;
    
    vec2 distAndMaterial = vec2(0);
    vec2 dnmEverythingElse = vec2(0);
    
    vec3 impact = vec3(1.0);
    bool goneTooFar = false;
    bool everythingsGoneTooFar = false;
    bool hitGlass = false;
    bool hitOtherThings = false;
    
    for (int i=ZERO; i < 100; ++i){
        dnmEverythingElse = sceneWithMaterials(ro + dir * everythingTraveled);
        everythingTraveled += dnmEverythingElse.x;
        
#ifndef WITHOUTKODOS
        distAndMaterial = sceneWithGlassMaterials(ro + dir * traveled);
        traveled += distAndMaterial.x;
#endif
        
        if (dnmEverythingElse.x < .01) {
            hitOtherThings = true;
        }
        
#ifndef WITHOUTKODOS
        if (distAndMaterial.x < .01) {
            hitGlass = true;
        }
        
        
        if (distAndMaterial.x > MAXDISTANCE) {
            goneTooFar = true;
        }
#endif
        
        if (dnmEverythingElse.x > MAXDISTANCE) {
            everythingsGoneTooFar = true;
        }
        
#ifndef WITHOUTKODOS
        if (hitGlass && (hitOtherThings || everythingsGoneTooFar)) {
            break;
        }
        
        if (hitOtherThings && (goneTooFar || hitGlass)) {
            break;
        }
        
        if (goneTooFar && everythingsGoneTooFar) {
            break;
        }
#else
        if (hitOtherThings) {
            break;
        }
        
        if (everythingsGoneTooFar) {
            break;
        }
#endif
    }
    
    vec3 color = starryBackground(dir);
    
    if (hitGlass && hitOtherThings) {
        vec3 scenePoint = ro + dir * everythingTraveled;
        vec3 glassPoint = ro + dir * traveled;
        
        float sceneDistance = distance(ro, scenePoint);
        float glassDistance = distance(ro, glassPoint);
        
        if (glassDistance >= sceneDistance) {
            hitGlass = false;
        } else {
            hitOtherThings = false;
        }
    }
    
    if (hitOtherThings) {
        vec3 hitPoint = ro + dir * everythingTraveled;
        color = GetColor(dnmEverythingElse, hitPoint, everythingTraveled, dir);
        return color;
    }
    
    if (hitGlass) {
        // Reflective/Refractive Surface
        color = vec3(71.0/255.0, 142.0/255.0, 232.0/255.0);;
        vec3 hitPoint = ro + dir * traveled;
        vec3 hitNormal = GetNormal1(hitPoint);
        
        vec3 refractionDirection = normalize(refract(hitNormal, -dir,1.0/1.6));
        
        bool outside = dot(dir, hitNormal) < 0.0;
        
        float surfaceWidth = 0.001;
        vec3 bias = surfaceWidth * hitNormal;
        
#ifdef ALLOWREFLECTIONS
        vec3 reflectionDirection = normalize(reflect(dir, hitNormal));
        vec3 reflectionRayOrig = outside ? hitPoint + bias : hitPoint - bias;
        vec3 reflectionColor = RayMarchReflection(reflectionRayOrig, reflectionDirection);
        float reflection = 0.04 + 0.96 * pow(1.0 - dot(-dir, hitNormal), 5.0);
#endif
        
        vec3 refractionRayOrig = outside ? hitPoint - bias : hitPoint + bias;
        vec4 refractionDandM = RayMarchInteriorGlass(refractionRayOrig, refractionDirection);
        vec3 newHit =  refractionRayOrig + refractionDirection * refractionDandM.w;
        vec3 newNormal = GetNormal2(hitPoint);
        
        float light = dot(newNormal, normalize( vec3(0,5,-5)))*.5+.5;
        float glassTravelDist =  1.-clamp(distance(refractionRayOrig, newHit) / 16.0, 0., 1.);
        
#ifdef ALLOWREFLECTIONS
        color = mix(reflectionColor*(1.0-reflection)+refractionDandM.xyz, color, 0.3);
        
        //color = reflectionColor*(1.0-reflection)+refractionDandM.xyz;
        //color = mix(reflectionColor*(1.0-reflection), color, 0.5);
#else
        color = refractionDandM.xyz;
#endif
        
        float glassLight = dot(hitNormal, normalize( vec3(0,5,-5)))*.5+.5;
        
        float specularStrength = 0.5;
        vec3 lightColor = vec3(1);
        vec3 viewDir = normalize(ro - hitPoint);
        vec3 reflectDir = reflect(-normalize( vec3(0,5,-5)), hitNormal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.);
        vec3 specular = specularStrength * spec * lightColor;
        
        color = color * (glassLight + specular);
        return color;
    }
    
    return color;
}

void main()
{
    vec2 uv = xlv_TEXCOORD0.xy/iResolution.xy - .5;
    uv.x *= iResolution.x/iResolution.y;
    
    
    // Attempt at Stack based rendering
#ifndef JUSTKODOS
    sceneStack[0] = 1;
    sceneStack[1] = 1;
#else
    sceneStack[0] = 0;
    sceneStack[1] = 1;
#endif
    
#ifdef WITHOUTKODOS
    sceneStack[1] = 0;
#endif
    
    sceneStack[2] = 1;
    
    vec3 cam = vec3(0,0,-CAMERAZOOM);
    vec3 dir = normalize(vec3(uv,1));
    
    mat3 rot = rotationXY(vec2(0.25,0.5));
    
    if (iMouse.z > 0.0) {
        rot = rotationXY( ( iMouse.xy - iResolution.xy * 0.5 ).yx * vec2( 0.01, -0.01 ) );
    }
    
    dir = rot * dir;
    cam = rot * cam;
    
    vec2 nonCorrectedUV = xlv_TEXCOORD0 / iResolution.xy;
    vec3 color = RayMarchGlass(nonCorrectedUV, cam, dir);
    
    gl_FragColor.rgb = color;
    gl_FragColor *= 1.-dot(uv,uv)*.4;
}

