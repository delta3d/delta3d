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

#ifndef DIRECTOR_VALUE_LINK
#define DIRECTOR_VALUE_LINK


#include <string>
#include <vector>

#include <dtDirector/export.h>

#include <dtCore/refptr.h>


namespace dtDAL
{
   class ActorProperty;
}

namespace dtDirector
{
   class Node;
   class ValueNode;

   /**
    * This is the base class for all input links.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT ValueLink
   {
   public:

      /**
       * Constructs the link.
       *
       * @param[in]  owner          The owning node of this link.
       * @param[in]  prop           The default property.
       * @param[in]  isOut          True if this link will output a value.
       * @param[in]  allowMultiple  True to allow connection with multiple value nodes.
       * @param[in]  typeCheck      If true, editor will not allow linking
       *                             between this and a value not of
       *                             the same type.
       */
      ValueLink(Node* owner, dtDAL::ActorProperty* prop, bool isOut = false, bool allowMultiple = false, bool typeCheck = true);

      /**
       * Destructor.
       */
      virtual ~ValueLink();

      /**
       * Retrieves the owner of the link.
       *
       * @return  The owner.
       */
      Node* GetOwner() {return mOwner;}

      /**
       * Retrieves the property.
       *
       * @param[in]  index  The value index.
       *
       * @return     The property.
       */
      dtDAL::ActorProperty* GetProperty(int index = 0);

      /**
       * Retrieves the default property.
       *
       * @return  The default property.
       */
      dtDAL::ActorProperty* GetDefaultProperty();

      /**
       * Sets the default property.
       *
       * @param[in]  prop  The default property.
       */
      void SetDefaultProperty(dtDAL::ActorProperty* prop);

      /**
       * Retrieves the total number of values connected.
       *
       * @return  The property count.
       */
      int GetPropertyCount();

      /**
       * Retrieves the name of the link.
       *
       * @return  The name of the link.
       */
      std::string GetName() const;

      /**
       * Retrieves whether this link is an output link.
       *
       * @return  True if this is an out link.
       */
      bool IsOutLink() {return mIsOut;}

      /**
       * Sets whether this link is an output link.
       *
       * @param[in]  isOut  True if this is an output link.
       */
      void SetOutLink(bool isOut) {mIsOut = isOut;}

      /**
       * Retrieves whether this link can connect to multiple values.
       *
       * @return  True if this can multiple connect.
       */
      bool AllowMultiple() {return mAllowMultiple;}

      /**
       * Sets whether this link can connect to multiple values.
       *
       * @param[in]  allowMultiple  True to allow multiple connections.
       */
      void SetAllowMultiple(bool allowMultiple) {mAllowMultiple = allowMultiple;}

      /**
       * Retrieves whether this link will perform a type check.
       *
       * @return  True if this link performs type checking.
       */
      bool IsTypeChecking() {return mTypeCheck;}

      /**
       * Sets whether this link will perform type checking.
       *
       * @param[in]  typeCheck  True to type check.
       */
      void SetTypeChecking(bool typeCheck) {mTypeCheck = typeCheck;}

      /**
       * Connects this link to a specified value node.
       *
       * @param[in]  valueNode  The value node to connect to.
       *
       * @return     True if the connection was made.  Connection
       *              can fail based on type checking.
       */
      bool Connect(ValueNode* valueNode);

      /**
       * Disconnects this link from a specified value node.
       *
       * @param[in]  valueNode  The value node to disconnect from.
       *                         NULL to disconnect all.
       */
      void Disconnect(ValueNode* valueNode = NULL);

      /**
       * Retrieves the list of links.
       */
      std::vector<ValueNode*>& GetLinks() {return mLinks;}

   private:

      Node* mOwner;

      std::vector<ValueNode*> mLinks;
      dtDAL::ActorProperty* mDefaultProperty;

      bool mIsOut;
      bool mAllowMultiple;
      bool mTypeCheck;
  };
}

#endif
