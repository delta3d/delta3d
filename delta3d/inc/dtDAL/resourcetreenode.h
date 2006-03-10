/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */

#ifndef DELTA_RESOURCE_TREE_NODE
#define DELTA_RESOURCE_TREE_NODE

#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/export.h"

namespace dtDAL 
{

   /**
    * @class ResourceTreeNode
    * @brief A tree node used when indexing resources.
    */
   class DT_DAL_EXPORT ResourceTreeNode 
   {
      public:
         ResourceTreeNode() {}

         ResourceTreeNode(const std::string& nodeText,
                        const std::string& fullCategory, ResourceDescriptor* resource = NULL):
            nodeText(nodeText), fullCategory(fullCategory) 
         {
            if (resource == NULL) 
            {
               category = true;
            } 
            else
            {
               category = false;
               this->resource = *resource;
            }
         }

         ResourceTreeNode(const ResourceTreeNode& toCopy) 
         {
            this->resource = toCopy.resource;
            this->nodeText = toCopy.nodeText;
            this->category = toCopy.category;
            this->fullCategory = toCopy.fullCategory;
         }

         ResourceTreeNode& operator=(const ResourceTreeNode& toAssign) 
         {
            this->resource = toAssign.resource;
            this->nodeText = toAssign.nodeText;
            this->category = toAssign.category;
            this->fullCategory = toAssign.fullCategory;
            return *this;
         }

         bool operator==(const ResourceTreeNode& toCompare) const 
         {
            if (category)
               return this->nodeText == toCompare.nodeText &&
                  this->category == toCompare.category;
            else
               return this->resource == toCompare.resource &&
                  this->category == toCompare.category;
         }

         bool operator>(const ResourceTreeNode& toCompare) const 
         {
            if (isCategory())
               return this->nodeText > toCompare.nodeText;
            return this->resource > toCompare.resource;
         }

         bool operator<(const ResourceTreeNode& toCompare) const 
         {
            if (isCategory())

               return this->nodeText < toCompare.nodeText;
            return this->resource < toCompare.resource;
         }

         /**
          * @return true if this tree node represents a category, false for a resource.
          */
         bool isCategory() const { return category; }

         /**
          * @return the display text for the tree node.
          */
         const std::string& getNodeText() const { return nodeText; }


         /**
          * @return the full category for this tree node, not including the datatype.
          */
         const std::string& getFullCategory() const { return fullCategory; }

         /**
          * @return the resource descriptor.  This descriptor is dataless if the node is a category.
          */
         const ResourceDescriptor& getResource() const { return resource; }
      private:
         std::string nodeText;
         std::string fullCategory;
         ResourceDescriptor resource;
         bool category;
   };
}

#endif
