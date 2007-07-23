/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2007 MOVES Institute
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
 * Erik Johnson 03/20/2007
 */

#ifndef DELTA_CAL3DLOADER
#define DELTA_CAL3DLOADER

#include <dtAnim/export.h>
#include <string>
#include <map>
#include <osg/ref_ptr>
#include <dtCore/refptr.h>
#include <osg/Referenced>


class CalCoreModel;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Texture2D;
}
/// @endcond


namespace dtAnim
{
   class Cal3DModelWrapper;
   class AnimationWrapper;
   class Animatable;
   class CharacterFileHandler;
   class Cal3DModelData;
  

   /**
    * Loads a animation definition file and returns a valid CalModel.  Caches
    * the CalCoreModel defined by the file to make it faster to create additional
    * instances of CalModels.  If you call Load() with the same filename twice,
    * it actually only loads once.
    */
   class DT_ANIM_EXPORT Cal3DLoader: public osg::Referenced
   {
      public:
   
         Cal3DLoader();
         
         ///Load an animated entity definition file and return the Cal3DModelWrapper
         bool Load(const std::string &filename, Cal3DModelData*& data_in);        

         ///empty all containers of CalCoreModels and the stored textures
         void PurgeAllCaches();
   
      protected:      
         virtual ~Cal3DLoader();

      private:
         CalCoreModel* GetCoreModel(CharacterFileHandler& handler, const std::string &filename, const std::string &path );

         //damned unpleasant place to load texture files.  Needs to be handled some other way
         void LoadAllTextures(CalCoreModel *model, const std::string &path);

         void LoadModelData(CharacterFileHandler& handler, CalCoreModel* model, Cal3DModelData* modelData);

         typedef std::map< std::string, osg::ref_ptr<osg::Texture2D> > TextureMap;
         typedef TextureMap::allocator_type::value_type TextureMapping;
         TextureMap mTextures;
 
   };
}//namespace dtAnim

#endif // __DELTA_CAL3DLOADER_H__
