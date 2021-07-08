 		Cartesian equation:	Parametrically:
Ellipse:	x2/a2 + y2/b2 = 1	x = a cos(t), y = b sin(t) 
Parabola:	y = ax2 + bx + c 
Hyperbola:	x2/a2 - y2/b2 = 1	x = a sec(t), y = b tan(t) 


//APPENDIX A Ellipse  Pascal Code 
Procedure Ellipse(a,b:long);
	long a_sqrf,b_sqrf,a22,b22,a42,b42,x_slope,y_slope;
	long d,mida,midb;
	int x, y; 
	{ x:=o; y:=b; a-sqrtf:-sqrtf (a) ; b-sqrtf: =sqrtf (b) ; a22:=a_sqr + a-rqr; b22:=b_sqr + b-sqrtf; a42:=a22 + a22; b42:-b22 + b22; x slope:=b42; Ix-slope - (4*bA*21*(x + 1) always) y_slope:-a42*(y-ll; ly-slope - 14*aAL21*(y - 1) always) mida:=a-sqrtf SHR 1; laA^2 div 2) midb:=b-sqrtf SHR 1; (ba^2 div 2) d:-b22 - a-sqrtf - y-slope SHR 1 - mida; (subtract aA*2 div 2 to optimisel (Region 1) while d <- y-slope do { Draw(x,y); if d > 0 then { d:-d - y-slope; y:-y - 1; y-slope:-y-slope - a42; } d:-d + b22 + x-slope; x:=x + 1; x-slope:=x-slope + b42; } d:-d-(x-slope+y-slope) SHR 1 +(b-sqrtf-a-sqrtf)+lmida-midb) IOptimised region change using x-slope , y-slope)

//A. Agathos el al. (Region 2) 
while y >- 0 do { Draw(x,yl; if d <= 0 then { d:-d + x-slope; x:=x + 1; x-slope :- x-slope + b42; } d:=d + a22 - y-slope; y:-y-1; y-slope:=y-slope - a42; } }



APPENDIX B Hyperbola Pascal Code Procedure Hyperbola(a,b:long;bound:int); (bound limits the hyperbola in y) var x, y,d,mida,midb:long; a22.b22,a-~qr,b-~qr:long; a42,b42:long; x-s1ope.y-s1ope:long; { x:-a; y:-0; a-sqrtf:-sqrtf(a); b-sqrtf:-sqrtf(b1; 2122:-a-sqrta-sqrtf; b22:-b-sqrtb-sqrtf; a42:=a22+a22; b42:-b22tb22; ~-slope:=b42~lx+l); (x-slope - 14*bAA21 ' (x + 11 always ) y_slope:=a42; (y-slope - 14*aAL21 ly + 11 always I mida:=a-sqrtf shr l;laA^Z div 21 mi*:-b-sqrtf shr 1; (b1"2 div 21 d:-a22 - b-sqrtf (1+2*a) + midb; [add b^^2 div 2 to optimize) (Region 1) while ld < x-slope) and ly<=boundl do { Draw1x.y); if d >- 0 then { d:-d.- x-slope; x:-X + 1; x-slope:-x-slope + b42; } d :- d + a22 + y-slope; y :- yt1; y-slope :- y-slope + a42; } d:-d - (x-slope t y-slope) shr 1 + la-sqrtb-sqrl - midb - mida; (optimlsed region change using x-slope , y-slope1 [Region 21 if a>b then while y <- bound do { Drav(x,y) ; if d<-0 then { d:-d+y-slope; y:=y+1; y-slope:=y-slope + a42; } d:-d - b22 - x-slope; x:-X + 1; x-slope:=x-slope + b42; } end



APPENDIX C Parabola  Pascal Code Procedure ParabolaIp,bound:integcrl; (bound limits the parabola in xl var x,y,d:int; ~2.~4 :int; { p2 :- 2-p; p4 :- 2.~22 x :- 0; y := 0; d:-l-p; (Region 1) while (y < p) and (x <= bound) do { Draw(x.yI; if d >- 0' then { x :- x + 1; d := d - p2; end, y :- y + 1; d :- d + 2*y + 1; } if d - 1 then d :- 1 - p4 else d :- 1 - p2; [Region 2) while x <- bound do { Drawlx.yl; if d <-0 then { y :- y + 1; d :- d + 4*y; } X :- X + 1; d :- d - p4; } 
