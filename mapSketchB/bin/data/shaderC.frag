#version 150

// this is how we receive the texture
uniform sampler2DRect map1;
uniform sampler2DRect nor1;
uniform sampler2DRect lines;
uniform float time, noiseA, noiseB;
uniform vec4 globalColor;
uniform vec3 lightPos;
uniform vec3 lightColor, shadowColor;
in vec2 texCoordVarying;

out vec4 outputColor;

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }

float snoise(vec2 v) {

  // Precompute values for skewed triangular grid
  const vec4 C = vec4(0.211324865405187,
                      // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,
                      // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626,
                      // -1.0 + 2.0 * C.x
                      0.024390243902439);
  // 1.0 / 41.0

  // First corner (x0)
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);

  // Other two corners (x1, x2)
  vec2 i1 = vec2(0.0);
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec2 x1 = x0.xy + C.xx - i1;
  vec2 x2 = x0.xy + C.zz;

  // Do some permutations to avoid
  // truncation effects in permutation
  i = mod289(i);
  vec3 p =
      permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

  vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);

  m = m * m;
  m = m * m;

  // Gradients:
  //  41 pts uniformly over a line, mapped onto a diamond
  //  The ring size 17*17 = 289 is close to a multiple
  //      of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt(a0*a0 + h*h);
  m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

  // Compute final noise value at P
  vec3 g = vec3(0.0);
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * vec2(x1.x, x2.x) + h.yz * vec2(x1.y, x2.y);
  return 130.0 * dot(m, g);
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  float noiseV =
      (snoise((texCoordVarying.xy + (time * 400) + 0.998766) * noiseA) * 0.15);
  float noiseVB = (snoise((texCoordVarying.xy + 0.12133) * (noiseB)) * 0.15);

  vec4 lines = texture(lines, texCoordVarying);
  vec4 terrain1 = texture(map1, texCoordVarying);
  vec4 normal1 = texture(nor1, texCoordVarying);

  vec4 baseColor1, baseColor2;

  vec4 normalTex = normalize(normal1);
  float light = dot(vec3(normalTex.rgb), lightPos);

  vec4 shade = vec4(light, light, light, 1.0);

  for (int i = 0; i < 60; i++) {
    if (distance(terrain1.r, i * 0.01666) <= 0.01666) {
      baseColor1 =
          vec4(hsv2rgb(vec3(0.1377 - (i * 0.01666 / 2) + (noiseV) -
                                (noiseVB * 0.045),
                            0.445 - (i * 0.01666) + (noiseV) - (noiseVB * 0.45),
                            0.155 + (i * 0.01666 / 2))),
               1.0);
    }
  }

  vec4 mixColor = mix(baseColor1, baseColor2, (sin(time * 1) + 1) / 2);

  vec4 clouds = vec4(noiseV, noiseV, noiseV, 0.4);
  vec4 outC;

  // float adj = (((light*2)-1)*3.25)*clouds.r;

  float adj = (((light * 2) - 1) * 0.31);

  if (adj <= 0) {
    baseColor1.r += adj * shadowColor.r;
    baseColor1.g += adj * shadowColor.g;
    baseColor1.b += adj * shadowColor.b;
  } else {
    baseColor1.r += adj * lightColor.r;
    baseColor1.g += adj * lightColor.g;
    baseColor1.b += adj * lightColor.b;
  }
  outputColor = (baseColor1 * 1.2) + (clouds * 0.0);
}
