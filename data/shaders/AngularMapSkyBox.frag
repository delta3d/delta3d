/* 
*  This shader just does a simple lookup into an angular map from a 3D vertex
*
*  see: http://www.debevec.org/Probes/
*  Bradley Anderegg
*/

uniform sampler2D angularMap;

varying vec3 texCoord;


void main(void)
{
   vec3 pos = normalize(texCoord);
   float r = 0.159154943 * acos(pos.z);
   r = r / length(pos.xy);
   vec2 vert = vec2(0.5 + pos.x * r, 0.5 + pos.y * r);
   	
   vec4 color = texture2D(angularMap, vert);
	
   gl_FragColor = color;
}

