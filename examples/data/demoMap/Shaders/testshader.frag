//About the most innovative shader ever
//but it shows what is the bare bone minimum for a fragment shader

//declaring color as a varying means it's value can be set in the 
//vertex shader and be referenced here
varying vec4 colorOut;

void main(void)
{
   //setting gl_FragColor with the value of a vec4 is the only requirement
   gl_FragColor = colorOut + vec4(0.2);
}

