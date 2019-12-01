/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
*/

#ifndef DELTA_TEXTURE_VISITOR_H
#define DELTA_TEXTURE_VISITOR_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <osg/Geode>
#include <osg/NodeVisitor>



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class TextureVisitor : public osg::NodeVisitor
{
public:

   typedef osg::NodeVisitor BaseClass;

   TextureVisitor();

   /*override*/ void apply(osg::Node& node);
   /*override*/ void apply(osg::Geode& node);
   /*override*/ void apply(osg::StateSet& stateset);

   void clear();

   void SetUnRefImageData(bool unref);

private:
   typedef std::set< osg::ref_ptr<osg::Texture> > TextureSet;
   TextureSet mTextureSet;
};

#endif
