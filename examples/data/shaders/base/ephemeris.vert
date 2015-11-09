#version 120

varying float vFog;

//const float distanceFogStart = 15.0;
//const float distanceFogEnd = 4.0;
const float degreesToSkyTop = 3.0;

void main(void)
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   //Determine the total fog contribution - This is a band effect where the backdrop fog is
   // gradiated from the zenith (top) of the sphere down to the horizon in a linear fashion
   float fogStart = 20.0;

   float fogAngle = radians(90.0 + fogStart);   
   float angleFromTopToVertex = acos(dot(vec3(0.0, 0.0, 1.0), gl_Normal)); // in radians
   float fogEndInRadians = radians(angleFromTopToVertex);

   float fogAngleScale = (5.5/(fogAngle - fogEndInRadians));
   vFog = -1.1 * clamp(fogAngleScale * (angleFromTopToVertex - fogAngle), -1.0, 0.0);
}

