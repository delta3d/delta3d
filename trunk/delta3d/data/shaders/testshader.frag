
varying vec4 color;


void main(void)
{
   color += vec4(0.2);
   gl_FragColor = clamp( color, 0.0, 1.0 );
}

