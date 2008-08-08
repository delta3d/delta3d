/* -*-c++-*-
* testStateManager - teststatemanager(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

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
