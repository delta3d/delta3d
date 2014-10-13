/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009
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

#ifndef lodcullcallback_h__
#define lodcullcallback_h__

#include <dtCore/observerptr.h>
#include <osg/Drawable>

namespace dtAnim
{
   class Cal3DModelWrapper;

   /**
    * A CullCallback used to adjust the animated character's
    * level of detail.  Relies on the LODOptions provided by the Cal3DModelData
    * and the distance to the Camera.
    */
   class LODCullCallback: public osg::Drawable::CullCallback
   {
   public:
      LODCullCallback(Cal3DModelWrapper& wrapper, int meshID);

      virtual bool cull(osg::NodeVisitor* nv, osg::Drawable* drawable, osg::RenderInfo* renderInfo) const;

   private:
      // To avoid a circular reference.
      dtCore::ObserverPtr<Cal3DModelWrapper> mWrapper;
      //CalHardwareModel* mHardwareModel;
      int mMeshID;
   };

} // namespace dtAnim

#endif // lodcullcallback_h__
