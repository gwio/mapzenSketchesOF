#version 150

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform vec3 lightPos;
uniform vec3 rgbFac;
uniform float time;
in vec2 texCoordVarying;

out vec4 outputColor;

void main() {
  vec4 terrainTex = texture(tex0, texCoordVarying);
  vec4 normalTex = normalize(texture(tex1, texCoordVarying));
  float light = dot(normalTex.rgb, lightPos);

  vec4 shade =
      vec4((light * rgbFac.r), light * rgbFac.g, light * rgbFac.b, 1.0) * 1.0;

  vec4 green = vec4(0.42, 0.27, 0.44, 1.0);
  vec4 brown = vec4(0.65, 0.22, 0.22, 1.0);

  vec4 mapColor = mix(green, brown, terrainTex.r * 2);
  outputColor =
      ((mapColor * shade) * 2.5 + (shade * 0.43) + (terrainTex * 0.15));
}
