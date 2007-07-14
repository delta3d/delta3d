uniform sampler2D diffuseTexture;
uniform sampler2D secondaryTexture;
uniform float TimeDilation;

// FRAGMENT - Provides highlight by blending from a detail texture
void main()
{
   float whackyOffset = sqrt(abs(TimeDilation - 0.5) + 1.0);
   float x = gl_TexCoord[0].x;
   float y = gl_TexCoord[0].y;

   // lookup the 3 oscillating colors 
   vec2 lookup1 = vec2(x + TimeDilation, y + TimeDilation+.25);
   vec2 lookup2 = vec2(x - whackyOffset, y + whackyOffset);
   vec2 lookup3 = vec2(x - (TimeDilation*2.0), y + TimeDilation);
   vec4 color1 = texture2D(secondaryTexture, lookup1);
   vec4 color2 = texture2D(secondaryTexture, lookup2);
   vec4 color3 = texture2D(secondaryTexture, lookup3);

   // Now blend the 3 colors together to make our highlight
   vec4 highlightColor;
   highlightColor.a = 1.0;
   highlightColor.r = color1.r*0.6 + color2.r*0.3 + color3.r*0.3;
   highlightColor.g = color1.g*0.2 + color2.g*0.7 + color3.g*0.2;
   highlightColor.b = color1.b*0.2 + color2.b*0.2 + color3.b*0.8;

   // Finally, blend the original color and highlight color
   vec4 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   gl_FragColor = (0.2 * diffuseColor) + (0.8 * highlightColor);
}
