
#ifndef NODEREGISTRY_H_
#define NODEREGISTRY_H_

#include "../export.h"
#include <dtDirector/nodepluginregistry.h>

namespace dtExample
{
   class TEST_APP_EXPORT DirectorNodePluginRegistry: public dtDirector::NodePluginRegistry
   {
   public:

      static dtCore::RefPtr<dtDirector::NodeType> SELECT_DESTINATION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> MOVE_TO_DESTINATION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SELECT_TARGET_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> TURN_TO_FACE_DIRECTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> CHANGE_STANCE_NODE_TYPE;

      static dtCore::RefPtr<dtDirector::NodeType> CIVILIAN_AVATAR_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> MARINE_AVATAR_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> APPLICATION_NODE_TYPE;

      DirectorNodePluginRegistry();
      virtual ~DirectorNodePluginRegistry();

      virtual std::string GetNodeLibraryType() const;

      virtual void RegisterNodeTypes();

   };
}
#endif /* NODEREGISTRY_H_ */
