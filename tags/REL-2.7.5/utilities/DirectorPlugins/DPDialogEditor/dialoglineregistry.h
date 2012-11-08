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

#ifndef DIRECTOR_LINE_REGISTRY
#define DIRECTOR_LINE_REGISTRY

#include <export.h>
#include <dialoglinetype.h>

#include <dtCore/refptr.h>

#include <vector>
#include <map>
#include <string>

namespace dtDirector
{
   class Node;
}

/**
 * The DialogLineRegistry is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogLineRegistry
{
public:

   /**
    * Gets the singleton instance of the registry.
    */
   static DialogLineRegistry &GetInstance();

   /**
    * Registers a line type to be used at the start of a dialog.
    *
    * @param[in]  line  An instance of the line type class.
    */
   void RegisterStartedLineType(DialogLineType* line);

   /**
    * Registers a line type to be used at the end of a dialog.
    *
    * @param[in]  line  An instance of the line type class.
    */
   void RegisterEndedLineType(DialogLineType* line);

   /**
    * Registers a line type.
    *
    * @param[in]  line  An instance of the line type class.
    *
    * @return     True if the line type was added successfully.
    *             False if a line type of the given name already exists.
    */
   bool RegisterLineType(DialogLineType* line);

   /**
    * Removes a line type from the registry.
    *
    * @param[in]  name  The name of the line type.
    *
    * @return     True if the line type was removed successfully.
    *             False if the line type was not found.
    */
   bool UnRegisterLineType(const QString& name);

   /**
    * Retrieves the started and ended line types.
    */
   const DialogLineType* GetStartedLineType() const;
   const DialogLineType* GetEndedLineType() const;

   /**
    * Retrieves a list of all line type names.
    */
   std::vector<QString> GetLineTypes() const;

   /**
    * Retrieves a line type that operates on a specific director node.
    *
    * @return  The line type, or an NULL if none was found.
    */
   const DialogLineType* GetLineTypeForNode(const dtDirector::Node* node) const;
   const DialogLineType* GetLineTypeForName(const QString& name) const;

private:

   /**
    * Constructor
    */
   DialogLineRegistry();

   /**
    * Deconstructor.
    */
   ~DialogLineRegistry();

   static DialogLineRegistry* mInstance;

   std::map<QString, dtCore::RefPtr<DialogLineType> > mLineTypes;

   dtCore::RefPtr<DialogLineType>   mStartedLineType;
   dtCore::RefPtr<DialogLineType>   mEndedLineType;
};

#endif // DIRECTOR_LINE_REGISTRY
