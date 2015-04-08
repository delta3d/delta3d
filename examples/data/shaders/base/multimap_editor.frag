#version 120

uniform vec4 effectScales;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D alphaTexture;

void main(void)
{

   float alphaScale = effectScales.x;
   float illumScale = effectScales.y;
   vec2 uv = gl_TexCoord[0].xy;

   vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);

   vec2 alpha = texture2D(alphaTexture, uv).rg * alphaScale;
   diffuse.rgb = texture2D(diffuseTexture, uv).rgb;
   diffuse.a = alpha.r;

   gl_FragColor = diffuse;
}
