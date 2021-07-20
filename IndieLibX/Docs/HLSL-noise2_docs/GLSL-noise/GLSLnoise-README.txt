Perlin noise, 2D, 3D and 4D, classic and simplex,
in a GLSL fragment shader
=================================================


"Classic noise" is the old favourite which everyone uses.

float noise(vec2 P)
float noise(vec3 P)
float noise(vec4 P)

The implementation is pretty straightforward, and I wish to
extend my thanks to Simon Green at Nvidia who gave me a good
start with his own implementation of classic Perlin noise,
and to Bill Licea-Kane at ATI for his help with a few GLSL
quirks and texture wrapping details.


"Simplex noise" is a new and improved creation from Ken Perlin.

float snoise(vec2 P)
float snoise(vec3 P)
float snoise(vec4 P)

I re-implemented it from scratch from Perlin's verbal desciption,
because I found his code to be extremely hard to read, and
for the most part it was impossible to port it to GLSL anyway.
The 2D and 4D cases were implemented from the verbal description
only, I haven't seen any example code from anyone there.
I ended up doing the gradient lookup in a different manner
to fit into GLSL 1.10. Perlin's reference implementation makes
heavy use of bit-wise operations on integers, which is not
implemented in GLSL 1.10. My version simply uses the same
gradient hash table as the classic noise algorithm.

The determination of simplex traversal orders for 3D and 4D
simplex noise are performed by functions provided by Bill
Licea-Kane at ATI. His code is not easy to understand, but the
functions are more hardware friendly and faster than my original
texture lookup method.


Why simplex noise?
------------------

Simplex noise has one very big advantage: it extends to 4D and
higher with only marginal slowdown.
This is because it evaluates only N+1 points for N dimensions,
while classic noise evaluates 2^N points for N dimensions.
In 4D and higher, simplex noise is a big win. For 2D and 3D it
is not a whole lot faster - it might even be slower for 2D in
this GLSL implementation, depending on your hardware.

Simplex noise also looks a lot better, because it doesn't have
the clearly noticeable axis-aligned structure of classic noise.
This is because the underlying grid is tetrahedral, not cubical.

Contrary to classic noise, it is quite simple to compute the true
derivative of simplex noise. There is no interpolation involved,
just a summation of polynomial components, so it's just a matter
of taking the derivative of those polynomials, evaluate them
separately and then add them together.

Once you've tried simplex noise, you'll never be quite happy with
classic noise again. It's a whole lot better in every respect.
It does look a bit different, though, so it is not always a
direct plug-in replacement for classic noise if you want a
particular look to your procedural noise patterns.


Performance
-----------

High end hardware of late 2004 was reported to attain speeds of
around 100 million noise samples per second with this noise(vec3).
Direct rendering of several noise values per pixel is perfectly
possible, but for noise components that change slowly, a render to
texture with less than one update per frame is highly recommended.
This procedural noise does not come cheap.

On a low-end but GLSL-capable GeForce FX 5600 XT, I got around
2.5 million samples of noise per second with noise(vec3). That
is only about as fast as CPU-based software noise on a 2-3 GHz CPU.
2D noise was about three to four times as fast, though.
On a more capable NVIDIA GeForce Fx 6800 Pro, I got these
significantly better benchmarks, by far outperforming any
software solution I could imagine using a regular CPU:

2D classic noise, noise(vec2)  : 165 Msamples/s
2D simplex noise, snoise(vec2) : 130 Msamples/s
3D classic noise, noise(vec3)  : 84.5 Msamples/s
3D simplex noise, snoise(vec3) : 80 Msamples/s
4D classic noise, noise(vec4)  : 40 Msamples/s
4D simplex noise, snoise(vec4) : 59 Msamples/s


Stefan Gustavson (stegu@itn.liu.se) 2004-12-05,
                   slightly updated 2005-09-16
