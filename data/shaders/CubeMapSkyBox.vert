
/* 
*  This shader takes a 2D orthographic vertex and multiplies it's clip space vert
*  by the inverse of the actual View and Projection matrix
*  which gives us a sudo 3D vertex in view space to do a lookup into our cubemap
*
*  Bradley Anderegg
*/

uniform mat4 inverseModelViewProjMatrix;

varying vec3 texCoord;


void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	vec4 texCoord4 = inverseModelViewProjMatrix * gl_Position;
	
	texCoord = vec3(texCoord4.x, -texCoord4.z, texCoord4.y);
}

