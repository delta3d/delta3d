//
// Cloud Fragment Shader
//
// Author: George Tarantilis
//
// Inspired by the cloud shader of OGL Orange Book

varying vec3  ModelPosition;

uniform sampler3D Noise;
uniform vec3  Offset;
uniform float Cutoff;
uniform float Exponent;
uniform vec3  CloudColor;
uniform float Bias;

void main (void)
{

    float  noise  = (texture3D(Noise, ModelPosition + Offset)).a;

    if(noise < Cutoff)
        noise = 0.0;
    else
        noise -= Cutoff;

    noise = Bias * (1.0 - pow(Exponent, noise));
    //noise = clamp(Bias * noise, 0.0, 1.0);

    gl_FragColor = vec4(CloudColor, noise * gl_Color.a);

}
