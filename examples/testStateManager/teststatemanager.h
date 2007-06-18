#ifndef DELTA_TESTSTATEMANAGER
#define DELTA_TESTSTATEMANAGER

#include <dtCore/base.h>  // for base class
#include <dtABC/statemanager.h>

class StateWalker : public dtCore::Base
{
public:
   typedef dtCore::Base BaseClass;

   StateWalker(dtABC::StateManager* gm);

   void OnMessage(dtCore::Base::MessageData* msg);
   void DisplayEventChoicesAndWaitForInput();

   void SetStartState(dtABC::State* start)   { mStartState = start; }
   const dtABC::State* getStartState() const { return mStartState.get(); }
   dtABC::State* getStartState()             { return mStartState.get(); }

   void SetStateManager(dtABC::StateManager* mgr)     { mStateManager = mgr; }
   dtABC::StateManager* GetStateManager()             { return mStateManager.get(); }
   const dtABC::StateManager* GetStateManager() const { return mStateManager.get(); }

protected:
   virtual ~StateWalker();
   void DisplayExtraEventChoices(unsigned int index);
   void HandleExtraEventChoices(unsigned int index_size, unsigned int choice);

private:
   dtCore::RefPtr<dtABC::StateManager> mStateManager;
   dtCore::RefPtr<dtABC::State> mStartState;
};

#endif // DELTA_TESTSTATEMANAGER
