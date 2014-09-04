/* -*-c++-*-
* testActorLibrary - testdalenvironmentactor (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* William E. Johnson II
*/
#ifndef DELTA_TEST_DAL_ENVIRONMENT_ACTOR
#define DELTA_TEST_DAL_ENVIRONMENT_ACTOR

#include <dtCore/plugin_export.h>
#include <dtCore/environmentactor.h>
#include <dtABC/weather.h>
#include <dtCore/cloudplane.h>

class DT_PLUGIN_EXPORT TestDALEnvironmentActor : public dtCore::IEnvironmentActor, public dtCore::DeltaDrawable
{
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      /// Constructor
      TestDALEnvironmentActor();

      /// Destructor
      virtual ~TestDALEnvironmentActor();

      /**
       * Adds an actor proxy to the internal hierarchy of the environment
       * @param proxy The proxy to add
       */
      virtual void AddActor(dtCore::DeltaDrawable &dd);

      /**
       * Removes an actor proxy from the internal hierarchy
       * @param proxy The proxy to remove
       */
      virtual void RemoveActor(dtCore::DeltaDrawable &dd);

      /**
       * Removes all actors associated with this environment
       */
      virtual void RemoveAllActors();

      /**
       * Called to see if this environment has the specified proxy
       * @param proxy The proxy to look for
       * @return True if it contains it, false if not
       */
      virtual bool ContainsActor(dtCore::DeltaDrawable &dd) const;

      /**
       * Gets all the actors associated with this environment
       * @param vec The vector to fill
       */
      virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec);

      /**
       * Sets the date and time on this environment
       * @param year The year to set
       * @param month The month to set
       * @param day The day to set
       * @param hour The hour to set in military time
       * @param min The minute to set
       * @param sec The second to set
       */
      virtual void SetTimeAndDate(const int year, const int month, const int day,
         const int hour, const int min,   const int sec);

      /**
       * Gets the time and date of the current environment
       * @param year The year to get
       * @param month The month to get
       * @param day The day to get
       * @param hour The hour to get
       * @param min The minute to get
       * @param sec The second to get
       */
      virtual void GetTimeAndDate(int &year, int &month, int &day,
               int &hour, int &min, int &sec) const;

      /**
       * Returns the number of children this environment has
       * @return The number of children
       */
      unsigned int GetNumEnvironmentChildren() const;
      
      osg::Node* GetOSGNode() { return mNode.get(); }
      const osg::Node* GetOSGNode() const { return mNode.get(); }
            
   private:

      dtCore::RefPtr<osg::Node> mNode;
};

class DT_PLUGIN_EXPORT TestDALEnvironmentActorProxy : public dtCore::BaseActorObject
{
   public:

      TestDALEnvironmentActorProxy();

      virtual ~TestDALEnvironmentActorProxy();

      virtual void CreateDrawable() { SetDrawable(*new TestDALEnvironmentActor); }

      virtual void BuildPropertyMap();

      virtual bool IsPlaceable() const { return false; }
};
#endif
