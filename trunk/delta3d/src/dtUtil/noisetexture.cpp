// NoiseTexture.cpp
// Based on updated implementation (2002) of Ken Perlin's Noise
// Modified to support 2d noise and to to be tile-able to every
// dimension


// Notes : width, height and slices parameters of the constructor
// should be power of 2
// If slices are not passed in to the constructor (or slices = 1)
// a 2d texture is created.
// For pixel format it should be used GL_ALPHA (for a transparency map),
// GL_LUMINANCE, GL_RGB or GL_RGBA
 
#include "dtUtil/noisetexture.h"
#include "time.h"

using namespace dtUtil;


// Constructors
NoiseTexture::NoiseTexture() { }

NoiseTexture::NoiseTexture(int    octaves,
                               int    frequency,
                               double amp,
                               double persistence,
                               int    width,
                               int    height,
                               int    slices)
   : mWidth(width),
     mHeight(height),
     mSlices(slices),
     mOctaves(octaves),
     mFrequency(frequency),
     mAmplitude(amp),
     mPersistence(persistence)
{
   
}

NoiseTexture::~NoiseTexture() {}



// Create the osg::Image
// Note - Do NOT try to create very large 3d textures
// A 3d texture of 512^3 with only one component (like ALPHA),
// results in a dds file of 134,217,728 bytes (128 Mbytes)
osg::Image *NoiseTexture::makeNoiseTexture(GLenum format)
{

    mImage = new osg::Image;
    
    int freq   = mFrequency;
    double amp = mAmplitude;

    GLenum pixelFormat           = format;  // GL_ALPHA, GL_LUMINANCE, GL_RGB or GL_RGBA
    GLenum internalTextureFormat = format;
    GLenum dataType              = GL_UNSIGNED_BYTE;
    int components               = osg::Image::computeNumComponents(pixelFormat);

    int f, i, j, k;
    double inci, incj, inck;
    double ni[3];

    /* Make sure 0's are written in the image's data segment*/
    int imageSize = mWidth * mHeight * mSlices * components;
    unsigned char * dataPtr = new unsigned char[imageSize], *ptr, data;
    unsigned char charToFill = 0;
    memset(dataPtr, charToFill, imageSize);  

    for (f = 0; f < mOctaves; ++f, freq *= 2, amp *= mPersistence)
    {
        ptr = dataPtr;

        ni[0] = ni[1] = ni[2] = 0;

        inci = (float)freq / mSlices;
        incj = (float)freq / mHeight;
        inck = (float)freq / mWidth;

        mNoise.Initialize(osg::Vec3f(0.0f, 0.0f, 0.0f), osg::Vec3f(mWidth, mHeight, mSlices));
       
        for (i = 0; i < mSlices; ++i, ni[2] += inci, ni[1] =  0)
        {
            for (j = 0; j < mHeight; ++j, ni[1] += incj, ni[0] = 0)
            {
                for (k = 0; k < mWidth; ++k, ni[0] += inck)
                {
                   data = (unsigned char) (((mNoise.GetNoise(osg::Vec3f(ni[0], ni[1], ni[2])) + 1.0) * amp) * 128);

                    switch(format)
                    {
                    case GL_RGB:
                        if (*(ptr) + data >  255)
                        {
                            *(ptr++) = 255;
                            *(ptr++) = 255;
                            *(ptr++) = 255;
                        }
                        else
                        {
                            *(ptr++) +=  data;
                            *(ptr++) +=  data;
                            *(ptr++) +=  data;
                        }
                        break;
                    case GL_RGBA:
                        if (*(ptr) + data >  255)  // Clamping because wrapping could occur
                        {
                            *(ptr++) = 255;  //R
                            *(ptr++) = 255;  //G
                            *(ptr++) = 255;  //B
                            *(ptr++) = 255;  //A
                        }
                        else  
                        {
                            *(ptr++) +=  data;  //R
                            *(ptr++) +=  data;  //G
                            *(ptr++) +=  data;  //B
                            *(ptr++) = 255;     //A
                        }
                        break;
                    case GL_ALPHA:
                    case GL_LUMINANCE:
                        if (*(ptr) + data >  255)
                            *(ptr++) = 255;
                        else
                            *(ptr++) +=  data;

                        break;

                    default:
                        return NULL;
                    }
                }
            }
        }
    }
    
    mImage->setImage(mWidth, mHeight, mSlices, pixelFormat, internalTextureFormat, dataType,
        dataPtr, osg::Image::USE_NEW_DELETE);

    return mImage;
}



/*

// The noise function is used both for 3d and 2d noise (with Z = 0)
double NoiseTexture::noise(double x, double y, double z, int freq) 
{
   int X = (int)floor(x) & 255,              FIND UNIT CUBE THAT 
   Y = (int)floor(y) & 255,              CONTAINS POINT.     
   Z = (int)floor(z) & 255;

   x -= floor(x);                         FIND RELATIVE X,Y,Z 
   y -= floor(y);                         OF POINT IN CUBE.   
   z -= floor(z);

   double  u = fade(x),                        COMPUTE FADE CURVES 
   v = fade(y),                        FOR EACH OF X,Y,Z.  
   w = fade(z);


   int Xmod = (X+1) % freq;                    These changes make 
   int Ymod = (Y+1) % freq;                    tileable noise     
   int Zmod = (Z+1) % freq;                    (% freq) will wrap the noise 

   int A1 = (p[X]    + Y),
   A2 = (p[A1]   + Z ),
   A3 = (p[A1]   + Zmod),

   A4 = (p[X]    + Ymod),
   A5 = (p[A4]   + Z),
   A6 = (p[A4]   + Zmod),

   B1 = (p[Xmod] + Y),
   B2 = (p[B1]   + Z),
   B3 = (p[B1]   + Zmod),


   B4 = (p[Xmod] + Ymod),
   B5 = (p[B4]   + Z),
   B6 = (p[B4]   + Zmod);


   return lerp(w,                               Trilinear interpolation 
               lerp(v,
               lerp(u, grad(p[A2], x, y,   z),   grad(p[B2], x-1, y,   z)),
               lerp(u, grad(p[A5], x, y-1, z),   grad(p[B5], x-1, y-1, z))),

               lerp(v,
               lerp(u, grad(p[A3], x, y,   z-1), grad(p[B3], x-1, y,   z-1)),
               lerp(u, grad(p[A6], x, y-1, z-1), grad(p[B6], x-1, y-1, z-1))));

}

*/

