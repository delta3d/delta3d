// NoiseTexture.cpp
// Based on updated implementation (2002) of Ken Perlin's Noise
// Modified to support 2d noise and to to be tile-able to every
// dimension


#include <prefix/dtutilprefix.h>
#include <dtUtil/noisetexture.h>
#include <cmath>
#include <cstdio>
#include <cstring>

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
   : mWidth(width)
   , mHeight(height)
   , mSlices(slices)
   , mOctaves(octaves)
   , mFrequency(frequency)
   , mAmplitude(amp)
   , mPersistence(persistence)
{
}

NoiseTexture::~NoiseTexture() {}



// Create the osg::Image
// Note - Do NOT try to create very large 3d textures
// A 3d texture of 512^3 with only one component (like ALPHA),
// results in a dds file of 134,217,728 bytes (128 Mbytes)
osg::Image* NoiseTexture::MakeNoiseTexture(GLenum format)
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

    // Make sure 0's are written in the image's data segment
    int imageSize = mWidth * mHeight * mSlices * components;
    unsigned char* dataPtr = new unsigned char[imageSize], *ptr, data;
    unsigned char charToFill = 0;
    memset(dataPtr, charToFill, imageSize);

    for (f = 0; f < mOctaves; ++f, freq *= 2, amp *= mPersistence)
    {
        ptr = dataPtr;

        ni[0] = ni[1] = ni[2] = 0;

        inci = (float)freq / mSlices;
        incj = (float)freq / mHeight;
        inck = (float)freq / mWidth;


        for (i = 0; i < mSlices; ++i, ni[2] += inci, ni[1] = 0)
        {
            for (j = 0; j < mHeight; ++j, ni[1] += incj, ni[0] = 0)
            {
                for (k = 0; k < mWidth; ++k, ni[0] += inck)
                {
                   data = (unsigned char) (((mNoise.GetNoise(osg::Vec3f(ni[0], ni[1], ni[2]), freq) + 1.0) * amp) * 128);

                    switch (format)
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
                            *(ptr++) += data;
                            *(ptr++) += data;
                            *(ptr++) += data;
                        }
                        break;
                    case GL_RGBA:
                        if (*(ptr) + data >  255)  // Clamping because wrapping could occur
                        {
                            *(ptr++) = 255; // R
                            *(ptr++) = 255; // G
                            *(ptr++) = 255; // B
                            *(ptr++) = 255; // A
                        }
                        else
                        {
                            *(ptr++) += data; // R
                            *(ptr++) += data; // G
                            *(ptr++) += data; // B
                            *(ptr++)  = 255;  // A
                        }
                        break;
                    case GL_ALPHA:
                    case GL_LUMINANCE:
                        if (*(ptr) + data >  255)
                        {
                            *(ptr++) = 255;
                        }
                        else
                        {
                            *(ptr++) +=  data;
                        }

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
