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
* Matthew W. Campbell
*/
#ifndef DELTA_SOARXTERRAINRENDERER
#define DELTA_SOARXTERRAINRENDERER

#include <osg/Image>
#include <osg/StateSet>
#include <osg/Program>
#include <osg/MatrixTransform>
#include <dtTerrain/terraindatarenderer.h>
#include <dtTerrain/soarxdrawable.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Texture2D;
}
/// @endcond

namespace dtTerrain
{   
   class SoarXDrawable;
   
   class DT_TERRAIN_EXPORT SoarXCacheResourceName : public dtUtil::Enumeration
   {
      DECLARE_ENUM(SoarXCacheResourceName);
      public:
      
         ///Identifier for cached preprocessed vertex data.
         static const SoarXCacheResourceName VERTEX_DATA;
         
         ///Identifier for cached preprocessed noise data for dynamically adding
         ///noise to terrain vertex data.
         static const SoarXCacheResourceName DETAIL_VERTEX_NOISE;
         
         ///Identifier for the cached preprocessed detail gradient texture used
         ///to add more detail to the terrain texturing.
         static const SoarXCacheResourceName DETAIL_GRADIENT_TEXTURE;
         
         ///Identifier for the cached preprocessed detail scale map used when
         ///adding more texture detail to the terrain.
         static const SoarXCacheResourceName DETAIL_SCALE_MAP;
         
         ///Identifier for the cached preprocessed gradient map generated on a per
         ///tile basis.
         static const SoarXCacheResourceName BASE_GRADIENT_TEXTURE;
         
      protected:
         SoarXCacheResourceName(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };
   
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
         
         /**
          * This structure represents a drawable tile.  There is a one-to-one
          * mapping of drawable entries to paged terrain tiles in the renderer.
          */
         struct DrawableEntry
         {
            dtCore::RefPtr<SoarXDrawable> drawable;
            dtCore::RefPtr<osg::Texture2D> baseGradientTexture;
            dtCore::RefPtr<osg::MatrixTransform> sceneNode;
         };            
         
         ///Help minimize some typing...
         typedef std::map<dtCore::RefPtr<PagedTerrainTile>,DrawableEntry> DrawableMap;
      
         ///Used to scale the precalculated gradient values used to render 
         ///the terrain.
         static const float GRADIENT_SCALE;
      
         /**
          * Constructs the SoarX renderer.
          * @param name Simple name to assign to this object. Default = "SoarXRenderer"
          */
         SoarXTerrainRenderer(const std::string &name="SoarXRenderer");
         
         /**
          * This method constructs a SoarXDrawable for the new terrain
          * tile that needs to be loaded.
          * @param tile The new tile.
          * @see SoarXDrawable
          */
         void OnLoadTerrainTile(PagedTerrainTile &tile);
         
         /**
          * This method updates an internal map of tiles and drawables
          * based on what tiles were unloaded from the terrain.
          * @param tile The tile being unloaded.
          */
         void OnUnloadTerrainTile(PagedTerrainTile &tile);
         
         /**
          * Gets the height of the terrain at the specified (x,y) coordinates.
          * @return The height of the terrain at the specified point.
          */
         float GetHeight(float x, float y);
         
         /**
          * Gets the normal vector at the specified point.
          * @return A vector perpendicular to terrain at the given point.
          */
         osg::Vec3 GetNormal(float x, float y);
         
         /**
          * Returns a scene node that encapsulates the renderable terrain.  
          * @return An OpenSceneGraph group node that contains all the information
          *    needed to render the terrain.
          * @note Initialize() is guarenteed to be called before this method by 
          *    the parent terrain.
          */
         osg::Group *GetRootDrawable() { return mRootGroupNode.get(); }
         
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
          *    path is "shaders/terrain.frag".
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

         /**
         * Tells the terrain whether or not to use the fog variables when rendering the terrain
         * by default there is no fog
         */
         void SetEnableFog(bool pEnableFog);

         
      protected:
      
         ///Destroy the renderer.
         virtual ~SoarXTerrainRenderer();
         
         /**
          * This method initializes data for this renderer that is shared 
          * amoungst the terrain tiles.  This includes shaders, textures,
          * some of which are read from the cache if it exists.
          */
         void InitializeRenderer();
         
         /**
          * Creates the GLSL program object.
          */
         void CreateFragmentShader();
         
         /**
          * Configures the specified drawable and stateset to use the proper textures 
          * and render state for SoarX terrain renderering.
          */
         void SetupRenderState(PagedTerrainTile &tile, DrawableEntry &entry,
            osg::StateSet &ss);
         
         /**
          * Attempts to load the detail noise data from the terrain cache.  If it
          * is not present, the data will be generated.
          */
         void CheckDetailNoiseCache();
         
         /**
          * Checks the cache for a detail gradient texture.  If found, it will load it
          * else it will be generated and then cached if enabled.
          */
         void CheckDetailGradientCache();         
         
         /**
          * Checks the cache for a detail scale map.  If found, it will load it, else
          * the data will be generated then cached if enabled.
          */
         void CheckDetailScaleCache();
         
         /**
          * This method is called for each tile that is loaded.  It checks to see if
          * there is a gradient texture in the tile's cache path and if so loads it.
          * If not, the gradient texture is generated and cached if enabled.
          */
         void CheckBaseGradientCache(const PagedTerrainTile &tile, DrawableEntry &entry);
         
         /**
          * Builds an array of noise values used to dynamically add more detail
          * to the terrain.
          */
         void CalculateDetailNoise(); 
         
      private:        
         
         ///Maps tiles to drawables.
         DrawableMap mDrawables;         
                       
         ///The root renderable for the terrain.
         dtCore::RefPtr<osg::Group> mRootGroupNode; 
         
         ///The file path to the fragment shader to use with rendering the
         ///terrain.
         std::string mFragShaderPath;
         
         float mThreshold;
         float mDetailMultiplier;
         
         ///Noise data used by each terrain tile to add vertex detail when
         ///zoomed in close to the terrain.
         float *mDetailNoise;
         int mDetailNoiseSize;
         int mDetailNoiseBits;
         float mDetailVerticalResolution;
         //float mDetailHorizonalResolution;
         
         ///Texture map holding the base gradient texture used
         ///to light the terrain.
         //dtCore::RefPtr<osg::Texture2D> mBaseGradientTexture;
         
         ///Texture map holding the detail gradient texture which
         ///is a noise texture repeated across the terrain to add
         ///a more natural "random" look to the terrain.
         dtCore::RefPtr<osg::Texture2D> mDetailGradientTexture;
         
         ///Texture map containing noise data which modifies the lighting
         ///across the terrain adding additional realism.
         dtCore::RefPtr<osg::Texture2D> mDetailScaleTexture;
         
         ///Reference to the pixel shader program used to render the SoarX
         ///terrain tiles.
         dtCore::RefPtr<osg::Program> mShaderProgram;         
        
         ///tells the renderer to use gl fog constants when rendering the terrain
         dtCore::RefPtr<osg::Uniform> mUniformRenderWithFog;
         bool mRenderWithFog;
   };   
}

#endif
