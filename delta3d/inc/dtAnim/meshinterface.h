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

#ifndef __DELTA_MESHINTERFACE_H__
#define __DELTA_MESHINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/referencedinterface.h>
#include <dtAnim/morphinterface.h>
// STL
#include <string>
#include <vector>



namespace dtAnim
{
   typedef std::vector<float> FloatArray;
   typedef std::vector<int> IndexArray;

   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT SubmeshInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      virtual int GetVertexCount() const = 0;
      virtual int GetFaceCount() const = 0;
      virtual int GetMorphTargetWeightCount() const = 0;

      // Returns number of vertices (num floats / 3)
      virtual int GetFaceData(dtAnim::IndexArray& outData) = 0;
      virtual int GetVertexData(dtAnim::FloatArray& outData, int stride) = 0;
      virtual int GetNormalData(dtAnim::FloatArray& outData, int stride) = 0;
      virtual int GetMorphTargetWeightData(dtAnim::FloatArray& outData) = 0;

      virtual int GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const = 0;

   protected:
      virtual ~SubmeshInterface() {}
   };

   typedef std::vector<dtCore::RefPtr<dtAnim::SubmeshInterface> > SubmeshArray;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT MeshInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      virtual int GetID() const = 0;

      virtual void SetName(const std::string& name) = 0;
      virtual const std::string& GetName() const = 0;

      virtual void SetVisible(bool visible) = 0;
      virtual bool IsVisible() const = 0;

      virtual int GetVertexCount() const = 0;
      virtual int GetFaceCount() const = 0;

      virtual int GetSubmeshCount() const = 0;
      virtual int GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const = 0;

   protected:
      virtual ~MeshInterface() {}
   };

   typedef std::vector<dtCore::RefPtr<dtAnim::MeshInterface> > MeshArray;
}

#endif
