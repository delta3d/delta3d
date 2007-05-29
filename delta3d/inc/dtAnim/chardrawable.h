/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
* Erik Johnson
*/

#ifndef chardrawable_h__
#define chardrawable_h__

#include "dtCore/transformable.h"
#include "dtAnim/export.h"

class CalModel;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
}
/// @endcond


namespace dtAnim
{
   /** Simple class that wraps up a dtAnim::Model so it can be added to the
   * Scene.  
   *
   * Usage:
   * @code
   *   dtCore::RefPtr<dtAnim::CharDrawable> char = new dtAnim::CharDrawable();
   *   dtCore::RefPtr<dtAnim::CoreModel> core = new dtAnim::CoreModel();
   *   core->LoadSkeleton(...);
   *   core->LoadAnimation(...);
   *   char->Create( *core );
   *   myScene->AddDrawable( *char );
   * @endcode
   */

   class CoreModel;
   class Cal3DAnimator;
   class Cal3DModelWrapper;

   /// A "view" of the cal3d animation state.
   class DT_ANIM_EXPORT CharDrawable : public dtCore::Transformable
   {
   public:
      CharDrawable(Cal3DModelWrapper* wrapper);
      ~CharDrawable();

      void OnMessage(dtCore::Base::MessageData *data);

      Cal3DModelWrapper* GetCal3DWrapper();
      osg::Geode* GetGeode() { return mGeode.get(); }

      /// change the data this class is viewing.
      void SetCal3DWrapper(Cal3DModelWrapper* wrapper);

   private:
      CharDrawable();

      dtCore::RefPtr<osg::Geode>    mGeode;
      dtCore::RefPtr<Cal3DAnimator> mAnimator;

      int mLastMeshCount;

      ///Delete and rebuild all the SubMeshDrawables required, based on the CalRenderer
      void RebuildSubmeshes(Cal3DModelWrapper* wrapper, osg::Geode* geode);
   };
}
#endif // chardrawable_h__
