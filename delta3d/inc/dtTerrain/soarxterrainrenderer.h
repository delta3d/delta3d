/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Matthew W. Campbell
*/
#ifndef DELTA_SOARXTERRIANRENDERER
#define DELTA_SOARXTERRAINRENDERER

#include <osg/Image>
#include "dtTerrain/terraindatarenderer.h"
#include "dtTerrain/soarxdrawable.h"

namespace dtTerrain
{
   
   class SoarXDrawable;
   
   /**
    * This renderer is an integration/implementation of the SoarX terrain
    * rendering algorithm. The SoarX algorithm is an extension of the original
    * SOAR (Stateless One-pass Adaptive Refinement) algorithm developed by
    * Peter Lindstrom and Valerio Pascucci.  The SOAR algorithm itself has 
    * several components and features including adaptive refinement, on-the-fly
    * triangle stripping and smooth geomorphing based on projected error.  The SoarX
    * implementation extends and improves on some of the shortcomings of the original
    * SOAR terrain rendering solution.  The original SoarX algorithm and demo application 
    * developed by Andras Balogh can be found at http://web.interware.hu/bandi/ranger.html.
    */
   class DT_TERRAIN_EXPORT SoarXTerrainRenderer : public TerrainDataRenderer
   {
      public:
      
         ///Used to scale the precalculated gradient values used to render 
         ///the terrain.
         static  const float GRADIENT_SCALE;
      
         /**
          * Constructs the SoarX renderer.
          * @param name Simple name to assign to this object. Default = "SoarXRenderer"
          */
         SoarXTerrainRenderer(const std::string &name="SoarXRenderer");
         
         /**
          * Initializes SoarX internal data structures using the height field data currently
          * assigned to it. Also builds the required scene representation so that it can
          * be rendered correctly.
          * @note There must be a valid heightfield assigned to the renderer before this
          *    method is called.  If not, an exception is thrown.
          */
         virtual void Initialize();
         
         /**
          * Gets the height of the terrain at the specified (x,y) coordinates.
          * @return The height of the terrain at the specified point.
          */
         virtual float GetHeight(float x, float y);
         
         /**
          * Gets the normal vector at the specified point.
          * @return A vector perpendicular to terrain at the given point.
          */
         virtual osg::Vec3 GetNormal(float x, float y);
         
         /**
          * Returns a scene node that encapsulates the renderable terrain.  
          * @return An OpenSceneGraph group node that contains all the information
          *    needed to render the terrain.
          * @note Initialize() is guarenteed to be called before this method by 
          *    the parent terrain.
          */
         virtual osg::Group *GetRootDrawable() { return mRootGroupNode.get(); }
         
         /**
          * Sets the detail multiplier on the SoarX drawable.
          * @see SoarXDrawable::SetDetailMultiplier.
          */
         void SetDetailMultiplier(float value);         
         
         /**
          * Gets the current detail multiplier.
          * @return The current detail multipler.  This will equal zero if the
          *    SoarXDrawable is not valid.
          */
         float GetDetailMultiplier() const;
                  
         /**
          * Sets the threshold value of the SoarXDrawable.
          * @see SoarXDrawable::SetThreshold()
          */
         void SetThreshold(float value);         
         
         /**
          * Gets the threshold value of the SoarXDrawable.
          * @see SoarXDrawable::GetThreshold()
          */
         float GetThreshold() const;
         
         /**
          * Sets the file containing the fragment shader to use 
          * when rendering the terrain.
          * @param filePath The path to the shader file.  This path
          *    is relative to the Delta3D data path list. The default
          *    path is "shaders/soarxterrain.frag".
          * @note This has no affect if the use of fragment shaders
          *    has been disabled.
          */
         void SetTerrainFragmentShader(const std::string &filePath)
         {
            mFragShaderPath = filePath;
            //Todo Need to update the shader program state and add
            //functions for enabling and disabling the fragment 
            //shader.
         } 
         
         /**
          * Gets the file name containing the fragment shader
          * currently in use by the terrain renderer.
          */
         const std::string &GetTerrainFragmentShader() const
         {
            return mFragShaderPath;
         }  
         
      protected:
      
         ///Destroy the renderer.
         virtual ~SoarXTerrainRenderer();
         
         /**
          * Constructs the images required to render and light the terrain.
          * @param geode The geometry node with which to attach the constructed
          *    texture maps to.  The geode in this case contains the terrain
          *    drawable itself.
          */
         void BuildTextureImages(osg::Geode *geode);
         
         /**
          * Binds the terrain textures to appropriate GLSL uniforms as well
          * as creates the fragment shader and attaches it to the terrain node.
          * @param geode The geometry node with which to attach the shaders to.
          *    The geode in this case contains the terrain drawable itself.
          */
         void CreateFragmentShader(osg::Geode *geode);
         
         /**
          * The terrain does not calculate texture coordinates, therefore,
          * we have to enable TexGen for certain texture units.  This
          * method creates the planar projection and builds the appropriate
          * TexGen structures.
          * @param geode The geometry node with which to attach the shaders to.
          *    The geode in this case contains the terrain drawable itself.
          */
         void CalculateAutoTexCoordParams(osg::Geode *geode);
         
      private:        
         
         ///The internal drawable for rendering and managing terrain data
         ///using the SoarX algorithm.
         dtCore::RefPtr<SoarXDrawable> mDrawable;
      
         ///The root renderable for the terrain.
         dtCore::RefPtr<osg::Group> mRootGroupNode; 
         
         ///The file path to the fragment shader to use with rendering the
         ///terrain.
         std::string mFragShaderPath;
         
         ///An image corresponding to the given heightfield.
         dtCore::RefPtr<osg::Image> mHeightMapImage;
         
         ///Texture map holding the base gradient texture used
         ///to light the terrain.
         dtCore::RefPtr<osg::Texture2D> mBaseGradientTexture;
         
         ///Texture map holding the detail gradient texture which
         ///is a noise texture repeated across the terrain to add
         ///a more natural "random" look to the terrain.
         dtCore::RefPtr<osg::Texture2D> mDetailGradientTexture;
         
         ///Texture map containing noise data which modifies the lighting
         ///across the terrain adding additional realism.
         dtCore::RefPtr<osg::Texture2D> mDetailScaleTexture;
         
         /**
          * Ensures that a heightfield is correct for the SoarX algorithm.
          * If it is not the correct size, the old heightfield is interpolated
          * using a bilinear filter.
          * @param The height field to check.  If it does not need to be 
          *    resized, this method will return the original.
          * @return A heightfield of size: nxn where n = (2^k) + 1.
          * @note k is the closest power of two to the current dimensions.
          */
         osg::HeightField *ResizeHeightField(osg::HeightField *oldHF);
         
         /**
          * Gets a bi-linearly interpolated height value from the heightfield.
          * @param hf The height field to get a value from.
          * @param x The x coordinate to sample from.
          * @param y The y coordinate to sample from.
          * @return The interpolated height value.
          */
         float GetInterpolatedHeight(const osg::HeightField *hf, double x, double y);
   };   
}

#endif
