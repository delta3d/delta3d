/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */

#ifndef DELTA_BASE
#define DELTA_BASE

// base.h: The root of the inheritance hierarchy.
//
//////////////////////////////////////////////////////////////////////


#include <dtUtil/refstring.h>

#include <dtCore/uniqueid.h>
#include <dtCore/export.h>
#include <dtUtil/macros.h>
#include <dtCore/sigslot.h>

#include <osg/Referenced>

namespace dtCore
{
   ///Base class to support naming and message passing

   /**
    * The Base class handles things that are required by most of the dtCore
    * classes such as naming, RTTI, and message passing.
    * To name an instance, call SetName() or pass it to the constructor.
    *
    * Inter-class message passing is handled by "subscribing" the
    * instance to a sender using AddSender().  Anytime the sender
    * calls SendMessage(), the receiver class' OnMessage() will get triggered.
    * The MessageData that gets passed to OnMessage() contains a pointer to the
    * sender, and optionally, a text message and pointer to user data.
    *
    * This class is also reference counted using the osg::Referenced class.  To
    * safely keep the reference count up-to-date, pointers to Base classes should
    * be stored in a RefPtr template.  For example:
    * \code
    * RefPtr<Base> mPointerToMyBase;
    * \endcode
    */
   class DT_CORE_EXPORT Base : public sigslot::has_slots<>, public osg::Referenced
   {
      DECLARE_MANAGEMENT_LAYER(Base)

   public:

      /**
       * Constructor.
       *
       * @param name the instance name
       */
      Base(const std::string& name = "base");

   protected:
      /**
       * Destructor.
       */
      virtual ~Base();

   public:
      /**
       * Sets the name of this instance.
       *
       * @param name the new name
       */
      virtual void SetName(const std::string& name);

      /**
       * Returns the name of this instance.
       *
       * @return the current name
       */
      const std::string& GetName() const;

      /**
       * This sets the unique ID, for general purposes this should not be used.
       */
      void SetUniqueId(const UniqueId& id) { mId = id; };

      /**
       * This class returns an instance the the UniqueID
       */
      const UniqueId& GetUniqueId() const { return mId; }

   private:
      ///< The name of this instance.
      dtUtil::RefString mName;

      UniqueId mId;
   };

} // namespace dtCore

#endif // DELTA_BASE
