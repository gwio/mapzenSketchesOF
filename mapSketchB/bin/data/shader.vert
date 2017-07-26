#version 150

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewProjectionMatrix;

uniform sampler2DRect map1;
uniform sampler2DRect map2;
uniform float time;
uniform sampler2DRect lines;
in vec4 position;
in vec2 texcoord;

// this is something we're creating for this shader
out vec2 texCoordVarying;



void main()
{
  // here we move the texture coordinates


float disp1 = texture(map1, texcoord).r;
float disp2 = texture(map2, texcoord).r;
//float sinus = (sin(time*0.5)+1)/2;
float sinus =1.0;

float displace = mix (disp1, disp2, sinus);
  // send the vertices to the fragment shader
//  position.y +=disp*1;
vec4 modPos = modelViewProjectionMatrix * position;
modPos.y += displace*1000;

 gl_Position = modPos;
 texCoordVarying = texcoord;
}
