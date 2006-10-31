/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#include <osg/Image>
#include <dtUtil/noiseutility.h>
#include <dtUtil/export.h>

namespace dtUtil
{

   /**
    * Noise Texture is a class that uses SeamlessNoise to generate an osg::Image
    */

    class NoiseTexture
    {
    public:

        DT_UTIL_EXPORT NoiseTexture();
        
        /**
         * Constructor: For a more detailed listing of these params see dtUtil::Fracal
         * @param octaves the number of summations of the noise
         * @param frequency the frequency of the noise
         * @param amp the amplitude of the noise
         * @param persistance the persistance of the noise
         * @param width A power of 2, specifying the x resolution
         * @param height A power of 2, specifying the y resolution
         * @param slices A power of 2, specifying the z resolution, or 1 for 2D textures       
         * @sa dtUtil::Fractal
         */
        DT_UTIL_EXPORT NoiseTexture( int    octaves,
                        int    frequency,
                        double amp,
                        double persistance,
                        int    width,
                        int    height,
                        int    slices = 1 //default for 2d images
            );

        DT_UTIL_EXPORT ~NoiseTexture();

        /**
         * Sets octaves
         * @param o an int greater than 0
         */
        void SetOctaves(int o)          { mOctaves     = o; }
        
        /**
         * Sets frequency
         * @param f an int greater than 0
         */
        void SetFrequency(int f)        { mFrequency   = f; }
        
        /**
         * Sets amplitude
         * @param a a double greater than 0
         */
        void SetAmplitude(double a)     { mAmplitude   = a; }
        
        /**
         * sets persistence
         * @param p a double from 0-1
         */
        void SetPersistence(double p)   { mPersistence = p; }
        
        /**
         * Sets width
         * @param w texture resolution of x dimension
         */
        void SetWidth(int w)            { mWidth = w; }
        
        /**
         * Sets height
         * @param h texture resolution of y dimension
         */
        void SetHeight(int h)           { mHeight = h; }

        /**
         * Sets slices
         * @param s the number of slices
         */
        void SetSlices(int s)           { mSlices = s; }

        /**
         * This function creates the texture
         * @param format specifies the format of the texture should be used GL_ALPHA (for a transparency map),
         * GL_LUMINANCE, GL_RGB or GL_RGBA
         *
         * @return  returns a pointer to the image
         *
         */
        DT_UTIL_EXPORT osg::Image *MakeNoiseTexture(GLenum format);

        /**
         * If you need a pointer to the texture you'll find it here
         * don't call this if you haven't generated the texture yet!
         *
         * @return returns a pointer to the image created
         */
        osg::Image* GetNoiseTexture(){return mImage;}

    private:

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
