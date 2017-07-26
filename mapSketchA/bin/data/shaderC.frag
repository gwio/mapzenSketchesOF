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

void main() {
  vec4 terrainTex = texture(tex0, texCoordVarying);
  vec4 normalTex = normalize(texture(tex1, texCoordVarying));
  float light = dot(normalTex.rgb, lightPos);
  vec4 linesLayer = texture(linesTex, texCoordVarying);
  vec4 shade;

  shade =
      vec4((light * rgbFac.r), light * rgbFac.g, light * rgbFac.b, 1.0) * 2.0;

  vec4 green = vec4(0.42, 0.27, 0.44, 1.0);
  vec4 brown = vec4(0.65, 0.22, 0.22, 1.0);

  vec4 red = vec4(0.62, 0.17, 0.24, 1.0);
  vec4 yellow = vec4(0.45, 0.72, 0.12, 1.0);

  vec4 mapColor = mix(green, brown, terrainTex.r * 4.5);
  vec4 temp = ((mapColor * shade) * 1.1 + (shade * 0.73) + (terrainTex * 0.15));

  if (distance(terrainTex.r, 0.55) <= 0.11) {
    mapColor = mix(yellow, red, terrainTex.r * 2.5);
  } else if (distance(terrainTex.r, 0.11) <= 0.22) {
    mapColor = mix(red, green, terrainTex.r * 4.5);
  } else if (distance(terrainTex.r, 0.761) <= 0.22) {
    mapColor = mix(red, green, terrainTex.r * 4.5);
  } else {
    mapColor = mix(red, brown, terrainTex.r * 2.5);
  }

  outputColor = mix(temp, mapColor, 0.123) + (linesLayer * 0.2);
}
