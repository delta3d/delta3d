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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "TextureVisitor.h"



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
TextureVisitor::TextureVisitor()
   : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{}

void TextureVisitor::apply(osg::Node& node)
{
   if (node.getStateSet()) apply(*node.getStateSet());
   traverse(node);
}

void TextureVisitor::apply(osg::Geode& node)
{
   if (node.getStateSet()) apply(*node.getStateSet());

   for (unsigned int i = 0; i<node.getNumDrawables(); ++i)
   {
      osg::Drawable* drawable = node.getDrawable(i);
      if (drawable && drawable->getStateSet()) apply(*drawable->getStateSet());
   }

   traverse(node);
}

void TextureVisitor::apply(osg::StateSet& stateset)
{
   // search for the existence of any texture object attributes
   for (unsigned int i = 0; i<stateset.getTextureAttributeList().size(); ++i)
   {
      osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i, osg::StateAttribute::TEXTURE));
      if (texture)
      {
         mTextureSet.insert(texture);
      }
   }
}

void TextureVisitor::clear()
{
   mTextureSet.clear();
}

void TextureVisitor::SetUnRefImageData(bool unref)
{
   for (TextureSet::iterator itr = mTextureSet.begin();
      itr != mTextureSet.end();
      ++itr)
   {
      osg::Texture* tex = const_cast<osg::Texture*>(itr->get());

      if (tex != nullptr)
      {
         tex->setUnRefImageDataAfterApply(unref);
      }
   }
}
