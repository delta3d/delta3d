/*  -*- c++ -*-
Copyright (C) 2003 <ryu@gpul.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

#ifndef DELTA_SUBMESH_H__
#define DELTA_SUBMESH_H__

#include <osg/Drawable>
#include <cal3d/cal3d.h>
#include "dtAnim/export.h"
#include <dtCore/refptr.h>

namespace dtAnim {

   class Cal3DModelWrapper;

///Adapter that converts cal3d submeshes into osg::Drawables

/** 
 * The easy way would be to draw all the character (CalModel) in a single
 * Drawable, but this approach lacks from state sorting. Each submesh of
 * each mesh of a model can have different state attributes. With the current
 * approach, if you have 1000 soldiers with two different textures each one,
 * there will be only two state changes per frame, and not 2000.
 *
 * Users of the osgCal library doesn't need to know about this class, it is
 * internal.
 */
class DT_ANIM_EXPORT SubMeshDrawable: public osg::Drawable {
public:
    /**
     * Creates a submesh for one model given the mesh and submesh of this mesh
     */
    SubMeshDrawable(Cal3DModelWrapper *wrapper, unsigned mesh, unsigned submesh);

    /**
     * Draws the geometry.
     */
    virtual void drawImplementation(osg::State& state) const;

    /**
     * Accept PrimitiveVisitor, in this case a TriangleVisitor
     */

    /** Return true, SubMesh does support accept(PrimitiveFunctor&). */
	virtual bool supports(osg::PrimitiveFunctor&) const { return true; }

    /** Accept a PrimitiveFunctor and call its methods to tell it
    about the interal primitives that this Drawable has. */
    
	virtual void accept(osg::PrimitiveFunctor& pf) const;

    virtual osg::Object* cloneType() const;
    virtual osg::Object* clone(const osg::CopyOp&) const;

protected:
    ~SubMeshDrawable();

private:
    SubMeshDrawable();   ///< not implemented by design

    unsigned int mMeshID;
    unsigned int mSubmeshID;
    float       *mMeshVertices;
    float       *mMeshNormals;
    float       *mMeshTextureCoordinates;
    int         *mMeshFaces;
    dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
    mutable unsigned int vertexCount;
    mutable unsigned int faceCount;

    void setUpMaterial();
};

}; //namespace dtAnim

#endif
