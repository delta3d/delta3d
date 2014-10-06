/* -*-c++-*-
 * dtPhysics
 * Copyright 2014, David Guthrie
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */
#include <iostream>
namespace dtPhysics
{
   template<class T>
   TriangleRecorderVisitor<T>::TriangleRecorderVisitor(TriangleRecorder::MaterialLookupFunc func)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      , mSplit(-1)
      , mSplitCount(1)
      , mNumGeodes(0)
      , mGeodeExportCounter(0)
      , mExportSpecificMaterial(false)
      , mSkipSpecificMaterial(false)
      , mMaterialLookup(func)
   {}

   template<class T>
   void TriangleRecorderVisitor<T>::CheckDesc(osg::Node& node)
   {
      if(!node.getDescriptions().empty())
      {
         // Use *last* description as material tag
         mCurrentDescription = node.getDescription(node.getNumDescriptions()-1);
      }
      else
      {
         mCurrentDescription.clear();
      }
   }

   template<class T>
   dtPhysics::MaterialIndex TriangleRecorderVisitor<T>::GetMaterialID(const std::string& str)
   {
      dtPhysics::MaterialIndex matID = 0;
      if(mMaterialLookup.valid())
      {
          matID = mMaterialLookup(str);
      }
      return matID;
   }

   template<class T>
   void TriangleRecorderVisitor<T>::apply(osg::Geode& node)
   {
      CheckDesc(node);
      //allow skipping one specific material or only exporting one material
      if((mExportSpecificMaterial && (mCurrentDescription != mSpecificDescription))
         || (mSkipSpecificMaterial && (mCurrentDescription == mSpecificDescription)))
      {
         std::cout << "Skipping material: " << mCurrentDescription << std::endl;
         return;
      }

      if(mSplit != -1)
      {
         ++mGeodeExportCounter;
         int divCount = mNumGeodes / mSplitCount;
         if (divCount == 0) divCount = 1;
         if(((mGeodeExportCounter - 1) / divCount) != mSplit)
         {
            //skip this one since we are breaking it up into multiple parts
            return;
            std::cout << "Skipping tile number: " << mGeodeExportCounter << std::endl;
         }
         else
         {
            std::cout << "Exporting tile number: " << mGeodeExportCounter << std::endl;
         }
      }

      //for some reason if we do this on the whole scene it crashes, so we are doing it per geode
      //Simplify(&node);

      dtPhysics::MaterialIndex matID = GetMaterialID(mSpecificDescription);

      osg::NodePath nodePath = getNodePath();
      mFunctor.SetMatrix(osg::computeLocalToWorld(nodePath));
      mFunctor.SetCurrentMaterial(matID);

      for(size_t i=0;i<node.getNumDrawables();i++)
      {
         osg::Drawable* d = node.getDrawable(i);

         if (d->supports(mFunctor))
         {
            d->accept(mFunctor);
         }
      }
   }

   template<class T>
   void TriangleRecorderVisitor<T>::apply(osg::Node& node)
   {
      CheckDesc(node);
      osg::NodeVisitor::apply(node);
   }

   template<class T>
   void TriangleRecorderVisitor<T>::apply(osg::Group& g)
   {
      CheckDesc(g);
      osg::NodeVisitor::apply(g);
   }

   template<class T>
   void TriangleRecorderVisitor<T>::apply(osg::Billboard& node)
   {
      //do nothing
   }

}
