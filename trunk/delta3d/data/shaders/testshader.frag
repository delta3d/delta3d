//About the most innovative shader ever
//but it shows what is the bare bone minimum for a fragment shader

//declaring color as a varying means it's value can be set in the 
//vertex shader and be referenced here
varying vec4 color;


void main(void)
{
	
   //here I add some diffuse lighting to brighten up the scene
   color += vec4(0.2);
   
   //setting gl_FragColor with the value of a vec4 is the only requirement
   //for a fragment shader, we clamp the color from 0.0 to 1.0 
   //because these are the values GL expects
   gl_FragColor = clamp( color, 0.0, 1.0 );
}

