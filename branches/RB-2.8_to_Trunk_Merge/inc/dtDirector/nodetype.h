/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_NODETYPE
#define DIRECTOR_NODETYPE

#include <dtDirector/export.h>

#include <dtCore/objecttype.h>
#include <dtCore/datatype.h>
#include <osg/Vec3>

namespace dtDirector
{
   class Node;

   /**
   * This class is more or less a simple data class that has information
   * describing a particular type of Node.  Node types contain a name,
   * category and description.
   *
   * @note
   *   NodeTypes for each node must be unique.
   */
   class DT_DIRECTOR_EXPORT NodeType: public dtCore::ObjectType
   {
   public:

      enum NodeTypeEnum
      {
         EVENT_NODE,
         ACTION_NODE,
         MUTATOR_NODE,
         VALUE_NODE,
         MACRO_NODE,
         LINK_NODE,
         MISC_NODE,

         UNKNOWN_NODE,
      };

      /**
       * Constructs a new actor type object.
       *
       * @param[in]  type        The type of node.
       * @param[in]  name        The name of the node.
       * @param[in]  category    The category of the node, this determines
       *                          if this node can be used depending on
       *                          the scripts type.
       * @param[in]  folder      The folder that this node will fall under
       *                          within the UI.
       * @param[in]  desc        The description of the node.
       * @param[in]  parentType  The parent node.
       * @param[in]  color       The color of this node type.
       * @param[in]  dataType    For value nodes, this represents the data
       *                          type of the value.
       */
      NodeType(NodeTypeEnum type,
            const std::string& name,
            const std::string& category,
            const std::string& folder="",
            const std::string& desc="",
            const NodeType* parentType = NULL,
            const osg::Vec3& color = osg::Vec3(150, 150, 150),
            dtCore::DataType& dataType = dtCore::DataType::UNKNOWN);

      /**
       * Gets the parent or "super" type of this node type.
       */
      const NodeType* GetParentNodeType() const;

      /**
       * Gets the node type.
       */
      NodeTypeEnum GetNodeType() const;

      /**
       * Retrieves the folder for this node.
       *
       * @return  The folder name.
       */
      const std::string& GetFolder() const;

      /**
       * Sets the folder for this node.
       *
       * @param[in]  folder  The name of the folder.
       */
      void SetFolder(const std::string& folder);

      /**
       * Retrieves the color for this node type.
       *
       * @return  The nodes color.
       */
      const osg::Vec3& GetColor() const;

      /**
       * Retrieves the data type of this node type.
       *
       * @return  The data type.
       */
      const dtCore::DataType& GetDataType() const;

   protected:

      /**
       * Object can only be deleted through the ref_ptr interface.	
       */
      virtual ~NodeType();

   private:

      std::string       mFolder;
      NodeTypeEnum      mType;
      osg::Vec3         mColor;
      dtCore::DataType&  mDataType;
   };
}

#endif
