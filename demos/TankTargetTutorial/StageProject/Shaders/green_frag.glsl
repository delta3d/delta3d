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

   //highlightColor.r = 0.1;// -.5 + color1.r*0.6 + color2.r*0.3 + color3.r*0.3;
   //highlightColor.g = 0.5 + color1.g*0.2 + color2.g*0.7 + color3.g*0.2;
   //highlightColor.b = 0.1; // + color1.b*0.2 + color2.b*0.2 + color3.b*0.8;

   //float x2 = x * 8.0;
   //float y2 = y * 16.0;
   //x2 = mod(x2,1.0);
   //y2 = mod(y2, 1.0);
   //if (((x2 > 0.5) && (y2 < 0.5)) || ((x2 < 0.5) && (y2 > 0.5)))
   //{
   //   gl_FragColor = (0.8 * vec4(0.8, 0.2, 0.2, 1.0));// + (0.4 * highlightColor);
   //}
   //else {
   //   gl_FragColor = 0.8 * vec4(0.1, 0.3, 0.9, 1.0);// + (0.4 * highlightColor); 
   //}
   

   //color1 = (color1 + color2 + color3 - .5) * 30.0;
   //highlightColor += color1;
   // Finally, blend the original color and highlight color
   vec4 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   gl_FragColor = (0.4 * diffuseColor) + (0.7 * highlightColor);
   //gl_FragColor = (highlightColor);
}
