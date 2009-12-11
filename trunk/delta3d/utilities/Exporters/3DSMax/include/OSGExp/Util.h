/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  Rune Schmidt Jensen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *	FILE:			Util.h
 *
 *	DESCRIPTION:	Header file for the static Util class
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 2.11.2003
 */
#ifndef __UTIL__H
#define __UTIL__H

// OSG includes
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Matrix>

// 3dsMax includes
#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "decomp.h"			// Affine Parts structure

#define ALMOST_ZERO 1.0e-3f

class Util{
	public:
		// Small utility methods
		static BOOL							isPoint3Equal(Point3 p1, Point3 p2);
		static BOOL							isVec3Equal(osg::Vec3 p1, osg::Vec3 p2);
		static BOOL							isVec2Equal(osg::Vec2 p1, osg::Vec2 p2);
		static BOOL							isIdentity(osg::Matrix mat);
		static BOOL							isGeodeEqual(osg::Geode* geode1, osg::Geode* geode2);
		static IParamBlock2*				isReferencedByHelperObject(INode* node, Class_ID helperID);
		static IParamBlock2*				isReferencedByHelperObjects(INode* node, std::vector<Class_ID> helperIDs);
		static osg::Geode*					getGeode(osg::Group* group);
		static std::string					getHexString(unsigned int i);
		static unsigned int					getUInt(std::string s);
};

/*
 Iterating ReferenceTarget objects changed in version 12 (2010).
 The following macros should be used for iterating so the code
 can compile on older and newer versions.
 */
#if MAX_RELEASE >= 12000

#define BEGIN_REF_ITERATE(node) {  DependentIterator di(node); \
								   ReferenceMaker *maker = NULL; \
								   while (NULL!=(maker=di.Next())) {
									   
#define END_REF_ITERATE()		} }

#else

#define BEGIN_REF_ITERATE(node) { RefList refList = node->GetRefList(); \
								   RefListItem* ptr = refList.first; \
								   while (ptr) { \
									   ReferenceMaker *maker = ptr->maker; \
								       if(maker) {
#define END_REF_ITERATE()		} ptr = ptr->next; } }

#endif

#endif // __UTIL__H
