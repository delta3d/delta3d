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

#ifndef DIRECTOR_OUTPUT_LINK
#define DIRECTOR_OUTPUT_LINK


#include <string>
#include <vector>

#include <dtDirector/export.h>
#include <dtUtil/warningdisable.h>

namespace dtDirector
{
   class Node;
   class InputLink;

   /**
    * This is the base class for all input links.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT OutputLink
   {
   public:

      /**
       * Constructs the link.
       *
       * @param[in]  owner    The owner of the link.
       * @param[in]  name     The name of the link.
       * @param[in]  comment  The optional link comment.
       */
      OutputLink(Node* owner, const std::string& name = "Out", const std::string& comment = "");

      /**
       * Destructor.
       */
      virtual ~OutputLink();

      /**
       * Copy constructor.
       *
       * @param[in]  src  The source link.
       */
      OutputLink(const OutputLink& src);

      /**
       * Assignment Operator.
       */
      OutputLink& operator=(const OutputLink& src);

      /**
       * Retrieves the owner of the link.
       *
       * @return  The owner.
       */
      Node* GetOwner() {return mOwner;}

      /**
       * Redirects this to another output link.
       *
       * @param[in]  redirector  The redirected link.
       */
      void RedirectLink(OutputLink* redirector) {mRedirector = redirector;}

      /**
       * Retrieves the redirected link.
       *
       * @return  The redirected link.
       */
      OutputLink* GetRedirectLink() {return mRedirector;}

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
      std::vector<InputLink*>& GetLinks() {return mLinks;}
      const std::vector<InputLink*>& GetLinks() const {return mLinks;}

      /**
       * Activates the input.
       */
      void Activate();

      /**
       * This will test whether this link is active or not.
       */
      bool Test();

      /**
       * Connects this output to an input.
       *
       * @param[in]  input  The input link to connect it to.
       *
       * @return     True if a connection was made.
       */
      bool Connect(InputLink* input);

      /**
       * Disconnects this output from an input.
       *
       * @param[in]  input  The input link to disconnect (NULL to disconnect all).
       *
       * @return     True if a connection was disconnected.
       */
      bool Disconnect(InputLink* input = NULL);

   private:

      std::string  mName;
      std::string  mComment;
      bool         mVisible;

      int          mActiveCount;

      Node*        mOwner;

      OutputLink*  mRedirector;

DT_DISABLE_WARNING_START_MSVC(4251)
      std::vector<InputLink*> mLinks;
DT_DISABLE_WARNING_END

   };
}

#endif
