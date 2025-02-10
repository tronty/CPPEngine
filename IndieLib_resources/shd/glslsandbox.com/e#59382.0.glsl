/*
Tämä fragmenttishader, joka on kirjoitettu GLSL:ll ä (OpenGL Shading Language), luo visuaalisesti mielenkiintoisen maailman, joka muistuttaa planeetan pintaa avaruudessa. Tässä on suomenkielinen selitys shaderin toiminnasta:
Yleiskuvaus

Shaderi simuloi planeetan pintaa, jossa on erilaisia alueita, kuten meriä, hiekkaa, ruohoa ja jäätiköitä. Lisäksi shaderi sisältää auringon, joka valaisee planeetan pintaa, ja yövalot, jotka näkyvät pimeällä puolella.
Muuttujat ja vakiot

    SEA, SAND, GRASS, ICE, NIGHTLIGHTS: Nämä vakiot määrittelevät eri alueiden värit. Esimerkiksi SEA on meren väri (sinertävän vihreä), kun taas ICE on jäätikön väri (vaaleansininen).

    PI: Piin arvo (3.1416), joka käytetään matemaattisissa laskuissa.

    surfacePosition: Tämä on fragmentin sijainti ruudulla, normalisoituna arvovälille [-1, 1].

Pääfunktio (main)

    p: Laskee fragmentin sijainnin ruudulla normalisoituna arvovälille [-1, 1]. Tämä mahdollistaa symmetrisen renderöinnin ruudun keskustan ympärillä.

    dist: Laskee fragmentin etäisyyden ruudun keskustasta. Tätä käytetään määrittämään, onko fragmentti planeetan sisällä vai ulkopuolella.

    sunPos: Auringon sijainti ruudulla. Auringon valo vaikuttaa planeetan pinnan valaistukseen.

    c: Laskee fragmentin värin. Jos fragmentti on planeetan sisällä (dist < br*br), kutsutaan st-funktiota, joka määrittää fragmentin värin sen sijainnin perusteella. Jos fragmentti on planeetan ulkopuolella, se näyttää taivaalta (tumman sininen) ja sisältää auringon valon.

st-funktio

    Tämä funktio määrittää fragmentin värin sen sijainnin perusteella planeetan pinnalla.

    sc: Fragmentin sijainti planeetan pinnalla normalisoituna.

    sh: Varjostusarvo, joka riippuu fragmentin sijainnista ja auringon valosta.

    rim: Reunan kirkkaus, joka lisää valoa planeetan reunoille.

    Funktio käyttää Perlin-kohinaa (perlin) luodakseen realistisen pinnan, jossa on korkeuseroja (vuoria, laaksoja jne.).

    Perlin-kohinan avulla määritetään, onko fragmentti merellä, hiekalla, ruohikolla vai jäätiköllä, ja sen mukaan valitaan väri.

Perlin-kohina (perlin-funktio)

    Tämä funktio generoi Perlin-kohinaa, joka on käytetty luomaan pinnan kohoumia ja syvänteitä.

    Perlin-kohina on tyypillisesti käytetty efekti, joka luo luonnollisen näköistä kohinaa, kuten pilviä, kiviä tai maastonmuotoja.

Kohina (noise-funktio)

    Tämä funktio generoi satunnaista kohinaa, joka on käytetty Perlin-kohinan laskennassa.

    Funktio palauttaa normalisoidun satunnaisvektorin, joka on käytetty interpolointiin Perlin-kohinan laskennassa.

Yhteenveto

Tämä fragmenttishader luo visuaalisesti rikkaan ja monipuolisen planeetan pinnan, jossa on erilaisia alueita ja valaistusefektejä. Shaderi käyttää matemaattisia menetelmiä, kuten Perlin-kohinaa, luodakseen realistisen pinnan ja valaistuksen. Lopputuloksena on kaunis ja mielenkiintoinen visuaalinen efekti, joka muistuttaa planeettaa avaruudessa.
*/

#if 0
#ifdef GL_ES
precision mediump float;
#endif


//uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif
varying vec2 surfacePosition;
#define PI 3.1416
const vec3 SEA = vec3(0.0, 0.15, 0.4);
const vec3 SAND = vec3(0.6, 0.7, 0.55);
const vec3 GRASS = vec3(0.2, 0.7, 0.3);
const vec3 ICE = vec3(0.8, 0.9, 0.9);
const vec3 NIGHTLIGHTS = vec3(0.4, 0.35, 0.2);
vec3 st(vec2 sc, float sh, float rim);
float perlin(vec2 p);
vec2 noise(vec2 n);

//@fernozzle (please excuse the fake sphere mapping and sloppy code)
// actually in space MOD! by @fernozzle

void main( void ) {

	vec2 p = (-1.0 + 2.0 * ((xlv_TEXCOORD0.xy) / resolution.xy));
	p.x *= (resolution.x / resolution.y);
	p = surfacePosition*2.0;
	float dist = dot(p, p);
	const float br = 0.8;
	vec2 sunPos = vec2(0.65, 0.2);
	vec3 c = vec3(0.05, 0.1, 0.2)/dist + vec3(1.0, 0.8, 0.3)*smoothstep(0.0, 7.0, 1.0/distance(p, sunPos))*2.0;
	if(dist < br*br){
		vec2 ssc = p/br;
		float sx = asin(ssc.x);
		float widthFromY = sqrt(1.0 - ssc.y*ssc.y);
		float nsx = sx/widthFromY;
		vec2 sc = vec2(nsx-time*0.5, p.y/br/widthFromY);
		c = st(sc, -nsx*1.1, dist/(br*br));
	}
	
	gl_FragColor = vec4(c, 1.0);
}

vec3 st(vec2 sc, float sh, float rim){
	//return vec3(mod(sc, vec2(0.1))*10., 1.);
	float x = perlin(sc*1.5)*70. + perlin(sc*7.)*20. + perlin(sc*20.)*10. + perlin(sc*60.)*7.;
	float stdsh =  -sh*0.3+0.5;
	if(x > 30. - abs(sc.y)*20.){
		return ICE * vec3(stdsh*1.15 + 0.0, stdsh + 0.1, stdsh + 0.1) + vec3(xlv_TEXCOORD0.xy / resolution.xy, 1.0)*pow(rim, 10.);
	}else if(x > 3.){
		return GRASS * (x * 0.007 + 0.7) * vec3(stdsh*3.0-0.9, stdsh*0.8+0.2, stdsh*0.8+0.2) + (NIGHTLIGHTS*max(0.,sh)*max(0.,perlin(sc*80.0))*0.9*max(0.,-x*0.3+4.)) + vec3(xlv_TEXCOORD0.xy / resolution.xy, 1.0)*pow(rim, 10.);
	}else if(x > 1.){
		return SAND * stdsh + vec3(xlv_TEXCOORD0.xy / resolution.xy, 1.0)*pow(rim, 10.);
	}else{
		return SEA * vec3(stdsh, stdsh*2.1-0.2, stdsh*0.8+0.2) + vec3(xlv_TEXCOORD0.xy / resolution.xy, 1.0)*pow(rim, 5.);
	}
}

//stolen from http://glsl.heroku.com/e#4698.0
float perlin(vec2 p) {
	vec2 q=floor(p);
	vec2 r=fract(p);
	float s=dot(noise(q),p-q);
	float t=dot(noise(vec2(q.x+1.0,q.y)),p-vec2(q.x+1.0,q.y));
	float u=dot(noise(vec2(q.x,q.y+1.0)),p-vec2(q.x,q.y+1.0));
	float v=dot(noise(vec2(q.x+1.0,q.y+1.0)),p-vec2(q.x+1.0,q.y+1.0));
	float Sx=3.0*(r.x*r.x)-2.0*(r.x*r.x*r.x);
	float a=s+Sx*(t-s);
	float b=u+Sx*(v-u);
	float Sy=3.0*(r.y*r.y)-2.0*(r.y*r.y*r.y);
	return a+Sy*(b-a);
}

vec2 noise(vec2 n) {
	vec2 ret;
	ret.x=fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453)*2.0-1.0;
	ret.y=fract(sin(dot(n.xy, vec2(34.9865, 65.946)))* 28618.3756)*2.0-1.0;
	return normalize(ret);
}

