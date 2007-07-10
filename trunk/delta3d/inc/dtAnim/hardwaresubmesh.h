/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 06/27/2007
 */

#ifndef DELTA_HARDWARE_SUBMESH
#define DELTA_HARDWARE_SUBMESH

#include <osg/Drawable>
#include "dtAnim/export.h"
#include <dtCore/refptr.h>


class CalHardwareModel; 

namespace osg
{
   class Program;
}

namespace dtAnim 
{
   class Cal3DModelWrapper;      

class DT_ANIM_EXPORT HardwareSubMeshDrawable: public osg::Drawable
{
public:

   HardwareSubMeshDrawable(Cal3DModelWrapper *wrapper, CalHardwareModel* model, osg::Program* shader, const std::string& boneUniformName, unsigned numBones, unsigned mesh, unsigned vertexVBO, unsigned indexVBO);

    virtual void drawImplementation(osg::State& state) const;

    virtual osg::Object* cloneType() const;
    virtual osg::Object* clone(const osg::CopyOp&) const;

protected:
    ~HardwareSubMeshDrawable();

private:
    HardwareSubMeshDrawable();   ///< not implemented by design
    
    dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
    CalHardwareModel* mHardwareModel;
    dtCore::RefPtr<osg::Program> mProgram;
    dtCore::RefPtr<osg::Uniform> mBoneTransforms;
    std::string mBoneUniformName;
    unsigned mNumBones, mMeshID, mVertexVBO, mIndexVBO;
};

}; //namespace dtAnim

#endif //DELTA_HARDWARE_SUBMESH
