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
*/

#ifndef __DELTA_MATERIALINTERFACE_H__
#define __DELTA_MATERIALINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/referencedinterface.h>
// OSG
#include <osg/Texture>
// STL
#include <string>
#include <vector>



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT MaterialInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      virtual int GetID() const = 0;

      virtual void SetName(const std::string& name) = 0;
      virtual const std::string& GetName() const = 0;

      virtual int GetTextureCount() const = 0;

      virtual void SetTexture(int index, osg::Texture* textureMap) = 0;
      virtual osg::Texture* GetTexture(int index) const = 0;

      virtual const std::string GetTextureFile(int index) const = 0;

   protected:
      virtual ~MaterialInterface() {}
   };

   typedef std::vector<dtCore::RefPtr<dtAnim::MaterialInterface> > MaterialArray;
}

#endif
