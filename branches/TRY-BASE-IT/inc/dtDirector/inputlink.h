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

#ifndef DIRECTOR_INPUT_LINK
#define DIRECTOR_INPUT_LINK


#include <string>
#include <vector>

#include <dtDirector/export.h>

#include <dtDirector/node.h>


namespace dtDirector
{
   class Node;
   class OutputLink;

   /**
    * This is the base class for all input links.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT InputLink
   {
   public:

      /**
       * Constructs the link.
       */
      InputLink(Node* owner, const std::string& name = "In", const std::string& comment = "");

      /**
       * Destructor.
       */
      virtual ~InputLink();

      /**
       * Copy constructor.
       *
       * @param[in]  src  The source link.
       */
      InputLink(const InputLink& src);

      /**
       * Assignment Operator.
       */
      InputLink& operator=(const InputLink& src);

      /**
       * Retrieves the owner of the link.
       *
       * @return  The owner.
       */
      Node* GetOwner() {return mOwner;}

      /**
       * Redirects this link to another input link.
       *
       * @param[in]  redirector  The link to redirect to.
       */
      void RedirectLink(InputLink* redirector) {mRedirector = redirector;}

      /**
       * Retrieves the redirected link.
       *
       * @return  The redirected link.
       */
      InputLink* GetRedirectLink() {return mRedirector;}

      /**
       * Sets the name of the link.
       *
       * @param[in]  name  The name of the link.
       */
      void SetName(const std::string& name);

      /**
       * Retrieves the name of the link.
       *
       * @return  The name of the link.
       */
      const std::string& GetName() const;

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
       * Accessors for the visible status of the link.
       */
      void SetVisible(bool visible) {mVisible = visible;}
      bool GetVisible() {return mVisible;}

      /**
       * Retrieves the list of links.
       */
      std::vector<OutputLink*>& GetLinks() {return mLinks;}
      const std::vector<OutputLink*>& GetLinks() const {return mLinks;}

      /**
       * Connects this input to an output.
       *
       * @param[in]  output  The output link to connect it to.
       *
       * @return     True if a new connecton was made.
       */
      bool Connect(OutputLink* output);

      /**
       * Disconnects this input from an output.
       *
       * @param[in]  output  The output link to disconnect (NULL to disconnect all).
       *
       * @return     True if a connection was disconnected.
       */
      bool Disconnect(OutputLink* output = NULL);

      // Output Link needs access to the mLinks list.
      friend class OutputLink;

   private:

      std::string mName;
      std::string mComment;
      bool        mVisible;

      Node*       mOwner;

      std::vector<OutputLink*> mLinks;

      InputLink*  mRedirector;
   };
}

#endif
