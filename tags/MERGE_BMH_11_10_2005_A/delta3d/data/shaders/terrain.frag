uniform sampler2D detailGradient;
uniform sampler2D detailScale;
uniform sampler2D baseGradient;
uniform sampler2D baseColor;

void main(void)
{
  vec4 detail = texture2D(detailGradient, gl_TexCoord[0].st);
  vec4 scale = texture2D(detailScale, gl_TexCoord[1].st);
  vec4 base = texture2D(baseGradient, gl_TexCoord[1].st);
  detail *= 2.0; detail -= 1.0;
  scale *= 2.0; scale -= 1.0;
  base *= 2.0; base -= 1.0;
  vec4 combined = (detail * scale) + base;
  vec3 normal = gl_NormalMatrix * normalize(vec3(combined.y, combined.z, 0.5));
  gl_FragColor = texture2D(baseColor, gl_TexCoord[1].st);
  gl_FragColor.rgb *= max(0.0, dot(normal, vec3(gl_LightSource[0].position)));
  const float LOG2E = 1.442695;   // = 1/log(2)
  float fog = exp2(-gl_Fog.density * .3 * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2E);
  fog = clamp(fog, 0.0, 1.0);
  gl_FragColor = mix(gl_Fog.color, gl_FragColor, fog);
}
