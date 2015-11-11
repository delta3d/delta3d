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
namespace dtPhysics
{
   template<class T>
   TriangleRecorderVisitor<T>::TriangleRecorderVisitor(TriangleRecorder::MaterialLookupFunc func)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      , mExportSpecificMaterial(false)
      , mSkipSpecificMaterial(false)
      , mMaterialLookup(func)
   {}

   template<class T>
   std::string TriangleRecorderVisitor<T>::GetDescription(const osg::Node& node)
   {
      std::string desc;
      
      if ( ! node.getDescriptions().empty())
      {
         // Use *last* description as material tag
         desc = node.getDescription(node.getNumDescriptions()-1);
      }

      return desc;
   }

   template<class T>
   void TriangleRecorderVisitor<T>::CheckDesc(osg::Node& node)
   {
      mCurrentDescription = GetDescription(node);
   }

   template<class T>
   std::string TriangleRecorderVisitor<T>::CheckDescriptionInAncestors(const osg::Node& node)
   {
      std::string desc;

      const osg::Node* curNode = &node;
      while (curNode != NULL)
      {
         desc = GetDescription(*curNode);
         if ( ! desc.empty())
         {
            break;
         }

         // Assume a single parent.
         if ( ! curNode->getParents().empty())
         {
            curNode = curNode->getParent(0);
         }
         else
         {
            curNode = NULL;
         }
      }

      return desc;
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
   std::string TriangleRecorderVisitor<T>::GetMaterialNameFiltered(const std::string& str)
   {
      std::string matName;

      if(mMaterialNameFilter.valid())
      {
          matName = mMaterialNameFilter(str);
      }

      return matName;
   }

   template<class T>
   void TriangleRecorderVisitor<T>::apply(osg::Geode& node)
   {
      // Obtain the description for the current node.
      CheckDesc(node);

      std::string desc = mCurrentDescription;

      // Find a description that might be contained in ancestors.
      // This treats the description as if it were inherited.
      // Description information may not be applied to geodes directly
      // depending on how art tools export models for OSG.
      if (desc.empty())
      {
         desc = CheckDescriptionInAncestors(node);
      }

      //allow skipping one specific material or only exporting one material
      if((mExportSpecificMaterial && (desc != mSpecificDescription))
         || (mSkipSpecificMaterial && (desc == mSpecificDescription)))
      {
         //std::cout << "Skipping material: " << desc << std::endl;
         return;
      }

      if (!mFunctor(node))
         return;

      //for some reason if we do this on the whole scene it crashes, so we are doing it per geode
      //Simplify(&node);

      dtPhysics::MaterialIndex matID = GetMaterialID(mSpecificDescription);
      std::string matName = GetMaterialNameFiltered(mSpecificDescription);

      osg::NodePath nodePath = getNodePath();
      mFunctor.SetMatrix(osg::computeLocalToWorld(nodePath));
      mFunctor.SetCurrentMaterial(matID);
      mFunctor.SetCurrentMaterialName(matName);

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
