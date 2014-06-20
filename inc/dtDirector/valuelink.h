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

#include <dtCore/datatype.h>

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtUtil/warningdisable.h>
#include <dtUtil/mswinmacros.h>

namespace dtCore
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
       * @param[in]  exposed        If true, this value will be exposed by default.
       */
      ValueLink(Node* owner, dtCore::ActorProperty* prop, bool isOut = false, bool allowMultiple = false, bool typeCheck = true, bool exposed = true);

      /**
       * Destructor.
       */
      virtual ~ValueLink();

      /**
       * Copy constructor.
       *
       * @param[in]  src  The source link.
       */
      ValueLink(const ValueLink& src);

      /**
       * Assignment Operator.
       */
      ValueLink& operator=(const ValueLink& src);

      /**
       * Retrieves the owner of the link.
       *
       * @return  The owner.
       */
      Node* GetOwner() {return mOwner;}

      /**
       * Sets the proxy owner.
       *
       * @param[in]  proxy  The proxy owner.
       */
      void SetProxyOwner(Node* proxy) {mProxyOwner = proxy;}

      /**
       * Sets the link this link will redirect to.
       *
       * @param[in]  redirector  The link to redirect to.
       */
      void RedirectLink(ValueLink* redirector);

      /**
       * Retrieves the property type of this link.
       *
       * @return  The property type.
       */
      virtual dtCore::DataType& GetPropertyType();

      /**
       * Retrieves the property.
       *
       * @param[in]  index    The value index.
       * @param[in]  outNode  If provided, will retrieve the value node that owns this property (if any).
       *
       * @return     The property.
       */
      dtCore::ActorProperty* GetProperty(int index = 0, ValueNode** outNode = NULL);

      /**
       * Retrieves the default property.
       *
       * @return  The default property.
       */
      dtCore::ActorProperty* GetDefaultProperty() const;

      /**
       * Sets the default property.
       *
       * @param[in]  prop  The default property.
       */
      void SetDefaultProperty(dtCore::ActorProperty* prop);

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
       * Sets the name of the link.
       *
       * @return  The name of the link.
       */
      void SetName(const std::string& name);

      /**
       * Sets the comment for the link.
       *
       * @param[in]  text  The comment text.
       */
      void SetComment(const std::string& text);

      /**
       * Retrieves the comment of the link.
       *
       * @return  The comment.
       */
      const std::string& GetComment() const;

      /**
       * Retrieves the display name of the link.
       */
      std::string GetDisplayName();

      /**
       * Accessors for the visible status of the link.
       */
      void SetVisible(bool visible) {mVisible = visible;}
      bool GetVisible() {return mVisible;}

      /**
      * Accessors for the exposed status of the link.
      */
      void SetExposed(bool exposed) {mExposed = exposed;}
      bool GetExposed() {return mExposed;}

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
       *
       * @return     True if any disconnections were made.
       */
      bool Disconnect(ValueNode* valueNode = NULL);

      /**
       * Retrieves the list of links.
       */
      const std::vector<ValueNode*>& GetLinks() const;
      std::vector<ValueNode*>& GetLinks();

   private:

      Node* mOwner;
      dtCore::ObserverPtr<Node> mProxyOwner;
      std::string mName;
      std::string mComment;
      bool mVisible;
      bool mExposed;

      ValueLink* mRedirector;

DT_DISABLE_WARNING_START_MSVC(4251)
      std::vector<ValueNode*> mLinks;
DT_DISABLE_WARNING_END

      dtCore::RefPtr<dtCore::ActorProperty> mDefaultProperty;

      bool mIsOut;
      bool mAllowMultiple;
      bool mTypeCheck;

      bool mGettingType;
  };
}

#endif
