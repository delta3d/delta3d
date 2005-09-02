/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_NOISETEXTURE
#define DELTA_NOISETEXTURE

// NoiseTexture.h
// Based on updated implementation (2002) of Ken Perlin's Noise
// Modified to support 2d noise and to to be tileable to every
// dimension


#include <math.h>
#include <stdio.h>
#include <osg/Image>
#include "dtUtil/noiseutility.h"
#include "dtCore/export.h"


namespace dtUtil
{

    class DT_EXPORT NoiseTexture
    {
    public:

        NoiseTexture();
        
        NoiseTexture( int    octaves,
                        int    frequency,
                        double amp,
                        double persistance,
                        int    width,
                        int    height,
                        int    slices = 1 //default for 2d images
            );

        ~NoiseTexture();

        void setOctaves(int o)          { mOctaves     = o; }
        void setFrequency(int f)        { mFrequency   = f; }
        void setAmplitude(double a)     { mAmplitude   = a; }
        void setPersistence(double p)   { mPersistence = p; }
        void setWidth(int w)            { mWidth = w; }
        void setHeight(int h)           { mHeight = h; }
        void setSlices(int s)           { mSlices = s; }

        osg::Image *makeNoiseTexture(GLenum format);

        osg::Image* GetNoiseTexture(){return mImage;}
    private:

        void initNoise(void);

      
        // Data members
        osg::Image *mImage;
        SeamlessNoise mNoise;
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
