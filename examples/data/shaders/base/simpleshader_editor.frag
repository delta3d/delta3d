#version 120

uniform sampler2D diffuseTexture;


void main(void)
{
   vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st);

   gl_FragColor = vec4(diffuse);
}

