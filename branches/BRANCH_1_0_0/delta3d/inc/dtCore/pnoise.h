#ifndef DELTA_PNOISE
#define DELTA_PNOISE

// Pnoise.h
// Based on updated implementation (2002) of Ken Perlin's Noise
// Modified to support 2d noise and to to be tileable to every
// dimension


#include <math.h>
#include <stdio.h>

#include <osg/Image>
#include "dtCore/export.h"
namespace dtCore
{

    class DT_EXPORT NoiseGenerator
    {
    public:

        NoiseGenerator();
        
        NoiseGenerator( int    octaves,
                        int    frequency,
                        double amp,
                        double persistance,
                        int    width,
                        int    height,
                        int    slices = 1 //default for 2d images
            );

        ~NoiseGenerator();

        void setOctaves(int o)          { mOctaves     = o; }
        void setFrequency(int f)        { mFrequency   = f; }
        void setAmplitude(double a)     { mAmplitude   = a; }
        void setPersistence(double p)   { mPersistence = p; }
        void setWidth(int w)            { mWidth = w; }
        void setHeight(int h)           { mHeight = h; }
        void setSlices(int s)           { mSlices = s; }

        osg::Image *makeNoiseTexture(GLenum format);

    private:

        void initNoise(void);

        inline double fade(double t){ return t * t * t * (t * (t * 6 - 15) + 10); }
        inline double lerp(double t, double a, double b){ return a + t * (b - a); }
        inline double grad(int, double, double, double);
        void reseed(void);
        double noise(double x, double y, double z, int freq);
 
        // Data members
        osg::Image *mImage;
        int mWidth;
        int mHeight;
        int mSlices;
        int mOctaves;
        int mFrequency;
        double mAmplitude;
        double mPersistence;

    };
}


#endif // DELTA_PNOISE
