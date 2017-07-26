#version 150

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect linesTex;
uniform vec3 lightPos;
uniform vec3 rgbFac;
uniform float time;

in vec2 texCoordVarying;

out vec4 outputColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  vec4 terrainTex = texture(tex0, texCoordVarying);
  vec4 normalTex = normalize(texture(tex1, texCoordVarying));
  float light = dot(vec3(normalTex.rgb), lightPos);
  vec4 linesLayer = texture(linesTex, texCoordVarying);

  vec4 shade = vec4(light * rgbFac.r, light * rgbFac.g, light * rgbFac.b, 1.0);

  vec3 baseColor;

  for (int i = 0; i < 20; i++) {
    if (distance(terrainTex.r, i * 0.05) <= 0.05) {
      baseColor = hsv2rgb(vec3(0.45 + (i * 0.05), 0.25, 0.5 + (0.05 * i)));
    }
  }

  vec4 final = vec4(baseColor.r, baseColor.g, baseColor.b, 1.0);

  outputColor = (final * 0.85) + (shade * 1.0) + (linesLayer * shade);
}
