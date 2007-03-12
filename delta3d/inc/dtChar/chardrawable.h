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
#include "dtChar/model.h"
#include "dtChar/export.h"

namespace dtChar
{
   /** Simple class that wraps up a dtChar::Model so it can be added to the
     * Scene.  It is expected to interact with the public dtChar::Model mModel
     * directly.
     *
     * Usage:
     * @code
     *   dtCore::RefPtr<dtChar::CharDrawable> char = new dtChar::CharDrawable();
     *   dtCore::RefPtr<dtChar::CoreModel> core = new dtChar::CoreModel();
     *   core->LoadSkeleton(...);
     *   core->LoadAnimation(...);
     *   char->mModel->Create( *core );
     *   myScene->AddDrawable( *char );
     * @endcode
     */
   class DT_CHAR_EXPORT CharDrawable : public dtCore::Transformable
   {
   public:
   	CharDrawable();
   	~CharDrawable();

      osg::ref_ptr<dtChar::Model> mModel; ///<handle to the actual dtChar::Model
   };
}
#endif // chardrawable_h__