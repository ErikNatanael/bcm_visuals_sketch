#version 330
precision highp float;


uniform sampler2DRect tex0;
uniform float amount;
uniform float tint;
uniform float contrast;

in vec2 texCoordVarying;

out vec4 outputColor;


float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587, 0.114));
}

void main()
{
	vec2 st = gl_FragCoord.xy;
  vec2 coords = vec2(texCoordVarying.x, texCoordVarying.y);
  // vec2 coords = st*1080.;
	vec3 color = texture(tex0, coords).rgb;// * vec4(1., 0., 0. ,1.);

  vec3 redTinted = texture(tex0, coords + vec2(-10.0*amount, -1.)).rgb * vec3(1.2, -.5, -0.2);
  vec3 greenTinted = texture(tex0, coords + vec2(5.0*amount, 1.)).rgb * vec3(-.5, 1.2, -0.2);
  // vec3 tintedColor = vec3(0.5, 1., 0.7)*0.05 + vec3(0.5, 1., 0.7) * (color * (1.-amount));// * vec4(1., 0., 0. ,1.);
  // color = color*(1.-tint) + tintedColor*tint;

  color += (redTinted + greenTinted) * amount;

  // contrast
  // color = color*color*contrast + color*(1.1-contrast);

  // color += vec3(0.5, 0, 0);

  outputColor = vec4(color, 1.);
}
