sampler2D iChannel0;
highp vec4 font2(int c) {
  vec4 v = vec4(0);
  v=mix(v, vec4(0x3c66, 0x6e6e, 0x6062, 0x3c00), step(-0.500, float(c)));
  v=mix(v, vec4(0x183c, 0x667e, 0x6666, 0x6600), step(0.500, float(c)));
  v=mix(v, vec4(0x7c66, 0x667c, 0x6666, 0x7c00), step(1.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x6060, 0x6066, 0x3c00), step(2.500, float(c)));
  v=mix(v, vec4(0x786c, 0x6666, 0x666c, 0x7800), step(3.500, float(c)));
  v=mix(v, vec4(0x7e60, 0x6078, 0x6060, 0x7e00), step(4.500, float(c)));
  v=mix(v, vec4(0x7e60, 0x6078, 0x6060, 0x6000), step(5.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x606e, 0x6666, 0x3c00), step(6.500, float(c)));
  v=mix(v, vec4(0x6666, 0x667e, 0x6666, 0x6600), step(7.500, float(c)));
  v=mix(v, vec4(0x3c18, 0x1818, 0x1818, 0x3c00), step(8.500, float(c)));
  v=mix(v, vec4(0x1e0c, 0xc0c, 0xc6c, 0x3800), step(9.500, float(c)));
  v=mix(v, vec4(0x666c, 0x7870, 0x786c, 0x6600), step(10.500, float(c)));
  v=mix(v, vec4(0x6060, 0x6060, 0x6060, 0x7e00), step(11.500, float(c)));
  v=mix(v, vec4(0x6377, 0x7f6b, 0x6363, 0x6300), step(12.500, float(c)));
  v=mix(v, vec4(0x6676, 0x7e7e, 0x6e66, 0x6600), step(13.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x6666, 0x6666, 0x3c00), step(14.500, float(c)));
  v=mix(v, vec4(0x7c66, 0x667c, 0x6060, 0x6000), step(15.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x6666, 0x663c, 0xe00), step(16.500, float(c)));
  v=mix(v, vec4(0x7c66, 0x667c, 0x786c, 0x6600), step(17.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x603c, 0x666, 0x3c00), step(18.500, float(c)));
  v=mix(v, vec4(0x7e18, 0x1818, 0x1818, 0x1800), step(19.500, float(c)));
  v=mix(v, vec4(0x6666, 0x6666, 0x6666, 0x3c00), step(20.500, float(c)));
  v=mix(v, vec4(0x6666, 0x6666, 0x663c, 0x1800), step(21.500, float(c)));
  v=mix(v, vec4(0x6363, 0x636b, 0x7f77, 0x6300), step(22.500, float(c)));
  v=mix(v, vec4(0x6666, 0x3c18, 0x3c66, 0x6600), step(23.500, float(c)));
  v=mix(v, vec4(0x6666, 0x663c, 0x1818, 0x1800), step(24.500, float(c)));
  v=mix(v, vec4(0x7e06, 0xc18, 0x3060, 0x7e00), step(25.500, float(c)));
  v=mix(v, vec4(0x3c30, 0x3030, 0x3030, 0x3c00), step(26.500, float(c)));
  v=mix(v, vec4(0xc12, 0x307c, 0x3062, 0xfc00), step(27.500, float(c)));
  v=mix(v, vec4(0x3c0c, 0xc0c, 0xc0c, 0x3c00), step(28.500, float(c)));
  v=mix(v, vec4(0x18, 0x3c7e, 0x1818, 0x1818), step(29.500, float(c)));
  v=mix(v, vec4(0x10, 0x307f, 0x7f30, 0x1000), step(30.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x0, 0x0), step(31.500, float(c)));
  v=mix(v, vec4(0x1818, 0x1818, 0x0, 0x1800), step(32.500, float(c)));
  v=mix(v, vec4(0x6666, 0x6600, 0x0, 0x0), step(33.500, float(c)));
  v=mix(v, vec4(0x6666, 0xff66, 0xff66, 0x6600), step(34.500, float(c)));
  v=mix(v, vec4(0x183e, 0x603c, 0x67c, 0x1800), step(35.500, float(c)));
  v=mix(v, vec4(0x6266, 0xc18, 0x3066, 0x4600), step(36.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x3c38, 0x6766, 0x3f00), step(37.500, float(c)));
  v=mix(v, vec4(0x60c, 0x1800, 0x0, 0x0), step(38.500, float(c)));
  v=mix(v, vec4(0xc18, 0x3030, 0x3018, 0xc00), step(39.500, float(c)));
  v=mix(v, vec4(0x3018, 0xc0c, 0xc18, 0x3000), step(40.500, float(c)));
  v=mix(v, vec4(0x66, 0x3cff, 0x3c66, 0x0), step(41.500, float(c)));
  v=mix(v, vec4(0x18, 0x187e, 0x1818, 0x0), step(42.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x18, 0x1830), step(43.500, float(c)));
  v=mix(v, vec4(0x0, 0x7e, 0x0, 0x0), step(44.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x18, 0x1800), step(45.500, float(c)));
  v=mix(v, vec4(0x3, 0x60c, 0x1830, 0x6000), step(46.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x6e76, 0x6666, 0x3c00), step(47.500, float(c)));
  v=mix(v, vec4(0x1818, 0x3818, 0x1818, 0x7e00), step(48.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x60c, 0x3060, 0x7e00), step(49.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x61c, 0x666, 0x3c00), step(50.500, float(c)));
  v=mix(v, vec4(0x60e, 0x1e66, 0x7f06, 0x600), step(51.500, float(c)));
  v=mix(v, vec4(0x7e60, 0x7c06, 0x666, 0x3c00), step(52.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x607c, 0x6666, 0x3c00), step(53.500, float(c)));
  v=mix(v, vec4(0x7e66, 0xc18, 0x1818, 0x1800), step(54.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x663c, 0x6666, 0x3c00), step(55.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x663e, 0x666, 0x3c00), step(56.500, float(c)));
  v=mix(v, vec4(0x0, 0x1800, 0x18, 0x0), step(57.500, float(c)));
  v=mix(v, vec4(0x0, 0x1800, 0x18, 0x1830), step(58.500, float(c)));
  v=mix(v, vec4(0xe18, 0x3060, 0x3018, 0xe00), step(59.500, float(c)));
  v=mix(v, vec4(0x0, 0x7e00, 0x7e00, 0x0), step(60.500, float(c)));
  v=mix(v, vec4(0x7018, 0xc06, 0xc18, 0x7000), step(61.500, float(c)));
  v=mix(v, vec4(0x3c66, 0x60c, 0x1800, 0x1800), step(62.500, float(c)));
  v=mix(v, vec4(0x0, 0xff, 0xff00, 0x0), step(63.500, float(c)));
  v=mix(v, vec4(0x81c, 0x3e7f, 0x7f1c, 0x3e00), step(64.500, float(c)));
  v=mix(v, vec4(0x1818, 0x1818, 0x1818, 0x1818), step(65.500, float(c)));
  v=mix(v, vec4(0x0, 0xff, 0xff00, 0x0), step(66.500, float(c)));
  v=mix(v, vec4(0x0, 0xffff, 0x0, 0x0), step(67.500, float(c)));
  v=mix(v, vec4(0xff, 0xff00, 0x0, 0x0), step(68.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xffff, 0x0), step(69.500, float(c)));
  v=mix(v, vec4(0x3030, 0x3030, 0x3030, 0x3030), step(70.500, float(c)));
  v=mix(v, vec4(0xc0c, 0xc0c, 0xc0c, 0xc0c), step(71.500, float(c)));
  v=mix(v, vec4(0x0, 0xe0, 0xf038, 0x1818), step(72.500, float(c)));
  v=mix(v, vec4(0x1818, 0x1c0f, 0x700, 0x0), step(73.500, float(c)));
  v=mix(v, vec4(0x1818, 0x38f0, 0xe000, 0x0), step(74.500, float(c)));
  v=mix(v, vec4(0xc0c0, 0xc0c0, 0xc0c0, 0xffff), step(75.500, float(c)));
  v=mix(v, vec4(0xc0e0, 0x7038, 0x1c0e, 0x703), step(76.500, float(c)));
  v=mix(v, vec4(0x307, 0xe1c, 0x3870, 0xe0c0), step(77.500, float(c)));
  v=mix(v, vec4(0xffff, 0xc0c0, 0xc0c0, 0xc0c0), step(78.500, float(c)));
  v=mix(v, vec4(0xffff, 0x303, 0x303, 0x303), step(79.500, float(c)));
  v=mix(v, vec4(0x3c, 0x7e7e, 0x7e7e, 0x3c00), step(80.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xff, 0xff00), step(81.500, float(c)));
  v=mix(v, vec4(0x367f, 0x7f7f, 0x3e1c, 0x800), step(82.500, float(c)));
  v=mix(v, vec4(0x6060, 0x6060, 0x6060, 0x6060), step(83.500, float(c)));
  v=mix(v, vec4(0x0, 0x7, 0xf1c, 0x1818), step(84.500, float(c)));
  v=mix(v, vec4(0xc3e7, 0x7e3c, 0x3c7e, 0xe7c3), step(85.500, float(c)));
  v=mix(v, vec4(0x3c, 0x7e66, 0x667e, 0x3c00), step(86.500, float(c)));
  v=mix(v, vec4(0x1818, 0x6666, 0x1818, 0x3c00), step(87.500, float(c)));
  v=mix(v, vec4(0x606, 0x606, 0x606, 0x606), step(88.500, float(c)));
  v=mix(v, vec4(0x81c, 0x3e7f, 0x3e1c, 0x800), step(89.500, float(c)));
  v=mix(v, vec4(0x1818, 0x18ff, 0xff18, 0x1818), step(90.500, float(c)));
  v=mix(v, vec4(0xc0c0, 0x3030, 0xc0c0, 0x3030), step(91.500, float(c)));
  v=mix(v, vec4(0x1818, 0x1818, 0x1818, 0x1818), step(92.500, float(c)));
  v=mix(v, vec4(0x0, 0x33e, 0x7636, 0x3600), step(93.500, float(c)));
  v=mix(v, vec4(0xff7f, 0x3f1f, 0xf07, 0x301), step(94.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x0, 0x0), step(95.500, float(c)));
  v=mix(v, vec4(0xf0f0, 0xf0f0, 0xf0f0, 0xf0f0), step(96.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xffff, 0xffff), step(97.500, float(c)));
  v=mix(v, vec4(0xff00, 0x0, 0x0, 0x0), step(98.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x0, 0xff), step(99.500, float(c)));
  v=mix(v, vec4(0xc0c0, 0xc0c0, 0xc0c0, 0xc0c0), step(100.500, float(c)));
  v=mix(v, vec4(0xcccc, 0x3333, 0xcccc, 0x3333), step(101.500, float(c)));
  v=mix(v, vec4(0x303, 0x303, 0x303, 0x303), step(102.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xcccc, 0x3333), step(103.500, float(c)));
  v=mix(v, vec4(0xfffe, 0xfcf8, 0xf0e0, 0xc080), step(104.500, float(c)));
  v=mix(v, vec4(0x303, 0x303, 0x303, 0x303), step(105.500, float(c)));
  v=mix(v, vec4(0x1818, 0x181f, 0x1f18, 0x1818), step(106.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xf0f, 0xf0f), step(107.500, float(c)));
  v=mix(v, vec4(0x1818, 0x181f, 0x1f00, 0x0), step(108.500, float(c)));
  v=mix(v, vec4(0x0, 0xf8, 0xf818, 0x1818), step(109.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0x0, 0xffff), step(110.500, float(c)));
  v=mix(v, vec4(0x0, 0x1f, 0x1f18, 0x1818), step(111.500, float(c)));
  v=mix(v, vec4(0x1818, 0x18ff, 0xff00, 0x0), step(112.500, float(c)));
  v=mix(v, vec4(0x0, 0xff, 0xff18, 0x1818), step(113.500, float(c)));
  v=mix(v, vec4(0x1818, 0x18f8, 0xf818, 0x1818), step(114.500, float(c)));
  v=mix(v, vec4(0xc0c0, 0xc0c0, 0xc0c0, 0xc0c0), step(115.500, float(c)));
  v=mix(v, vec4(0xe0e0, 0xe0e0, 0xe0e0, 0xe0e0), step(116.500, float(c)));
  v=mix(v, vec4(0x707, 0x707, 0x707, 0x707), step(117.500, float(c)));
  v=mix(v, vec4(0xffff, 0x0, 0x0, 0x0), step(118.500, float(c)));
  v=mix(v, vec4(0xffff, 0xff00, 0x0, 0x0), step(119.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xff, 0xffff), step(120.500, float(c)));
  v=mix(v, vec4(0x303, 0x303, 0x303, 0xffff), step(121.500, float(c)));
  v=mix(v, vec4(0x0, 0x0, 0xf0f0, 0xf0f0), step(122.500, float(c)));
  v=mix(v, vec4(0xf0f, 0xf0f, 0x0, 0x0), step(123.500, float(c)));
  v=mix(v, vec4(0x1818, 0x18f8, 0xf800, 0x0), step(124.500, float(c)));
  v=mix(v, vec4(0xf0f0, 0xf0f0, 0x0, 0x0), step(125.500, float(c)));
  v=mix(v, vec4(0xf0f0, 0xf0f0, 0xf0f, 0xf0f), step(126.500, float(c)));
  return v;
}

highp vec4 font(int c) {
    if (c < 128) return font2(c);
    return vec4(0xffff) - font2(c - 128);
}

vec4 colors(int c) {
    if (c ==  0) return vec4(0x00,0x00,0x00,1);
    if (c ==  1) return vec4(0xFF,0xFF,0xFF,1);
    if (c ==  2) return vec4(0x68,0x37,0x2B,1);
    if (c ==  3) return vec4(0x70,0xA4,0xB2,1);
    if (c ==  4) return vec4(0x6F,0x3D,0x86,1);
    if (c ==  5) return vec4(0x58,0x8D,0x43,1);
    if (c ==  6) return vec4(0x35,0x28,0x79,1);
    if (c ==  7) return vec4(0xB8,0xC7,0x6F,1);
    if (c ==  8) return vec4(0x6F,0x4F,0x25,1);
    if (c ==  9) return vec4(0x43,0x39,0x00,1);
    if (c == 10) return vec4(0x9A,0x67,0x59,1);
    if (c == 11) return vec4(0x44,0x44,0x44,1);
    if (c == 12) return vec4(0x6C,0x6C,0x6C,1);
    if (c == 13) return vec4(0x9A,0xD2,0x84,1);
    if (c == 14) return vec4(0x6C,0x5E,0xB5,1);
    if (c == 15) return vec4(0x95,0x95,0x95,1);
    return vec4(0);
}

void main( )
{    
	vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    vec2 sz = vec2(40.0, 20.0);
    vec2 fb_pos = floor(uv * sz) + vec2(0.5, 0.5);
    fb_pos.y = sz.y - fb_pos.y;
    fb_pos /= iResolution.xy;
    
	vec4 fb = texture(iChannel0, fb_pos);
    highp vec4 char = font(int(fb.x));

    vec2 p = mod(uv * sz * 8.0, 8.0);
	int line = 7 - int(p.y);
    highp float pixels = 0.0;
    if (line == 0) pixels = char.x / 256.0;
    if (line == 1) pixels = char.x;
    if (line == 2) pixels = char.y / 256.0;
    if (line == 3) pixels = char.y;
    if (line == 4) pixels = char.z / 256.0;
    if (line == 5) pixels = char.z;
    if (line == 6) pixels = char.w / 256.0;
    if (line == 7) pixels = char.w;

    if (mod(pixels * pow(2.0, floor(p.x)), 256.0) > 127.5) {
        gl_FragColor = colors(int(fb.y)) / 180.0;
    } else {
        gl_FragColor = colors(int(fb.z)) / 180.0;
    }
}