#version 150

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect linesTex;
uniform vec3 lightPos;
uniform vec3 rgbFac;
uniform float time;
uniform vec2 mousePos;
uniform int click;

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
  vec4 terrainTex = texture(tex0, texCoordVarying);
  vec4 normalTex = normalize(texture(tex1, texCoordVarying));
  float light = dot(vec3(normalTex.rgb), lightPos);

  vec4 shade = vec4(light * rgbFac.r, light * rgbFac.g, light * rgbFac.b, 1.0);
  vec4 linesLayer = texture(linesTex, texCoordVarying);
  // outputColor = vec4(light,light,light, 1.0);
  // shade = shade*linesLayer.r;
  shade.b = shade.b * terrainTex.r;
  vec3 baseColor;

  for (int i = 0; i < 40; i++) {
    if (distance(terrainTex.r, i * 0.025) <= 0.025) {
      baseColor = hsv2rgb(vec3(0.45 + (i * 0.025), 0.25, 0.5 + (0.025 * i)));
    }
  }

  float radius;
  if (click != 0) {
    radius = (sin(time * 1.0) + 1.0) * 6600;
    // radius =344;

  } else {
    radius = 0;
  }

  float noiseV = snoise((texCoordVarying.xy + time * 224) * 0.0015) * 0.5;
  vec4 clouds = vec4(noiseV * shade.r, noiseV, noiseV, 0.2);

  if (click != 0) {
    float mouseDist = length(texCoordVarying - vec2(900, 550)) +
                      (terrainTex.r * radius * 0.388 * (clouds.r + 1.0));
    float radiusSmoothIn, radiusSmoothOut;
    float radiusMin = radius * 0.250;
    float radiusFac = 0.7;
    radiusSmoothIn = smoothstep(radiusMin, radius * radiusFac, mouseDist);
    radiusSmoothOut = 1.0 - smoothstep(radius * radiusFac, radius, mouseDist);
    if ((mouseDist < radius) && (mouseDist > (radiusMin))) {
      if (mouseDist < radius * radiusFac) {
        linesLayer = linesLayer * 0.54 * radiusSmoothIn;
      } else {
        linesLayer = linesLayer * 0.54 * radiusSmoothOut;
      }
    } else {
      linesLayer = linesLayer * 0.0;
    }
  } else {
    linesLayer = linesLayer * 0.0;
  }

  // shade = shade*linesLayer.r;

  vec4 final = vec4(baseColor.r, baseColor.g, baseColor.b, 1.0);

  /*
  if( final.r+final.g+final.b < 0.85) {
  final = vec4(final.r-0.1, final.g-0.1, final.b-0.1, 1.0);
}
*/
  outputColor = linesLayer + (shade * 4.80) + (final * 0.355) + (clouds * 0.00);
}
