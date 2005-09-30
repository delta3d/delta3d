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
#ifndef DELTA_TERRAINDATARENDERER
#define DELTA_TERRAINDATARENDERER

#include <osg/Vec3>
#include <osg/Image>
#include "dtCore/base.h"
#include "dtCore/refptr.h"
#include "dtTerrain/terrain_export.h"
#include "dtDAL/exceptionenum.h"

///Forward declare some OSG classes.
namespace osg 
{
   class Group;
   class HeightField;
}


namespace dtTerrain
{
   class Terrain;
   
   /**
    * This enumeration contains generic errors that could occur in terrain
    * renderers.  Subclass this enumeration for specific error handling
    * identification.
    */
   class DT_TERRAIN_EXPORT TerrainRendererException : public dtDAL::ExceptionEnum
   {
      DECLARE_ENUM(TerrainRendererException);
      public:
      
         ///Thrown if the heightfield data is invalid when the renderer
         ///initializes itself.
         static TerrainRendererException INVALID_HEIGHTFIELD_DATA;
         
      protected:
         
         ///Simple enumeration constructor.
         TerrainRendererException(const std::string &name) : dtDAL::ExceptionEnum(name)
         {
            AddInstance(this);
         }
   };
   
   /**
    * This class is the interface for a terrain renderer.  A terrain renderer handles
    * rendering of the terrain.  It can access the terrain reader through its parent
    * terrain handle if data paging and such is required.  This class enables different
    * terrain rendering algorithms to operate transparently and independent of the 
    * underlying terrain data so they can be interchanged when needed.
    */
   class DT_TERRAIN_EXPORT TerrainDataRenderer : public dtCore::Base
   {
      
      public:
         
         ///Make the code a little more readable.      
         typedef dtCore::RefPtr<osg::Image> CustomImage;
      
         /**
          * Constructs the terrain renderer.
          */
         TerrainDataRenderer(const std::string &name = "TerrainRenderer");
            
         /**
          * Initializes the renderer.  Any data structures that need to be filled or data
          * calculations/conversions from the height field representation to internal 
          * data should be done here.  This includes any initial textures, scene nodes, etc.
          * @note Implementations of this method should throw exceptions if an error
          *    occurs.
          */
         virtual void Initialize() = 0;
         
         /**
          * Gets the height of the terrain at the specified (x,y) coordinates.
          * @return The height of the terrain at the specified point.
          */
         virtual float GetHeight(float x, float y) = 0;
         
         /**
          * Gets the normal vector at the specified point.
          * @return A vector perpendicular to terrain at the given point.
          */
         virtual osg::Vec3 GetNormal(float x, float y) = 0;
         
         /**
          * Returns a scene node that encapsulates the renderable terrain.  This 
          * is the entry point by which the terrain is added to the scene.
          * @return An OpenSceneGraph group node that contains all the information
          *    needed to render the terrain.
          * @note Initialize() is guarenteed to be called before this method by 
          *    the parent terrain.
          */
         virtual osg::Group *GetRootDrawable() = 0;
         
         /**
          * This method provides an interface for setting custom images to use
          * during terrain rendering.  This image list may or may not be used
          * depending on the particular terrain renderer.
          * @param imageList A list of images to pass along to the renderer.
          * @note As an example, the SoarXTerrainRenderer currently uses the first
          *    image as a base texture when rendering the terrain.
          */         
         virtual void SetCustomImageList(const std::vector<CustomImage> &imageList)
         {
            mCustomImageList = imageList;
         }
         
         /**
          * Gets the list of custom images currently assigned to this terrain
          * renderer.
          * @return A list of images.
          */
         const std::vector<CustomImage> &GetCustomImageList() const { return mCustomImageList; }
         
         /**
          * Sets the heightfield that needs to be rendered by this renderer.
          * @param hf The HeightField data to render.
          * @note This merely tells the renderer to reference a heightfield. 
          *    The parent terrain will assign this heightfield before the 
          *    Initialize() method is called.
          */
         void SetHeightField(osg::HeightField *hf);
         
         /**
          * Gets the height field generated from the terrain data.
          * @return A valid heightfield.
          */
         osg::HeightField *GetHeightField() { return mHeightField.get(); }
         
         /**
          * Gets the height field generated from the terrain data.
          * @return A valid heightfield whos contents cannot be modified.
          */     
         const osg::HeightField *GetHeightField() const { return mHeightField.get(); }
         
         /**
          * Gets the terrain object that currently owns this reader.
          * @return A pointer to the parent terrain.
          */
         dtTerrain::Terrain *GetParentTerrain() { return mParentTerrain.get(); }
         
         /**
          * Gets a read-only terrain object that currently owns this reader.
          * @return A const pointer to the parent terrain.
          */
         const dtTerrain::Terrain *GetParentTerrain() const { return mParentTerrain.get(); }
         
      protected:
      
         ///Empty destructor...
         virtual ~TerrainDataRenderer();
      
         ///Heightfield data...
         dtCore::RefPtr<osg::HeightField> mHeightField;
         
         ///List of custom images for the renderer to use.
         std::vector<CustomImage> mCustomImageList;
         
         ///Allow the terrain to have access to this class.
         friend class Terrain;
         
      private:
      
         /**
          * The terrain object that currently owns this reader.
          * @note Renderer instances can only be assigned to one terrain at a time.
          */
         dtCore::RefPtr<dtTerrain::Terrain> mParentTerrain;
   };
     
     
} 

#endif
