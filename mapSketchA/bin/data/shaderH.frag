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

  vec4 baseColor, baseColorB;
  baseColor = vec4(hsv2rgb(vec3(0.625, 0.255, 0.99)).rgb, 1.0);
  baseColorB = vec4(hsv2rgb(vec3(0.125, 0.975, 0.25)).rgb, 1.0);

  /*
          for (int i = 0; i < 40; i++)
          {
                  if (distance(terrainTex.r, i*0.05) <= 0.025)
                  {
                          baseColor = hsv2rgb(vec3(0.125 + (i*0.025), 0.25+
     (0.005*i), 0.8 - (0.025*i)));
                  }
          }
  */
  float radius;
  if (click != 0) {
    radius = (sin(time * 0.21) + 1.0) * 7600;
    // radius =344;

  } else {
    radius = 0;
  }

  vec4 final = mix(baseColor, baseColorB, terrainTex.r);

  if (click != 0) {
    float mouseDist = length(texCoordVarying - vec2(900, 550)) +
                      (terrainTex.r * radius * 0.48);
    float radiusSmoothIn, radiusSmoothOut;
    float radiusMin = radius * 0.50;
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
  /*
  if( final.r+final.g+final.b < 0.85) {
  final = vec4(final.r-0.1, final.g-0.1, final.b-0.1, 1.0);
  }
  */
  outputColor = (final * 1.2) + (shade * 2.2) + (linesLayer * 1);
}
