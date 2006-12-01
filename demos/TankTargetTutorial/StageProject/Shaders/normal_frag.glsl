uniform sampler2D diffuseTexture;

void main()
{
   vec4 diffuseColor = texture2D(diffuseTexture,gl_TexCoord[0].st);
   gl_FragColor = diffuseColor;
}
