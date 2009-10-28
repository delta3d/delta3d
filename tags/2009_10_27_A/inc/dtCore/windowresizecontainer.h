/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2009 MOVES Institute 
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

#ifndef windowresizecontainer_h__
#define windowresizecontainer_h__

#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <osg/GraphicsContext>
#include <vector>

namespace dtCore
{
   class WindowResizeCallback;
   class DeltaWin;

   /** Internal class used to contain multiple WindowResizeCallback instances.
    *  This is created in the DeltaWin and will trigger any WindowResizeCallback
    *  instances that have been added when DeltaWin gets resized.
    */
   class WindowResizeContainer : public osg::GraphicsContext::ResizedCallback
   {
   public:
   	WindowResizeContainer(DeltaWin& win);
      
      virtual void resizedImplementation(osg::GraphicsContext* gc, int x, int y, int width, int height);

      /** Add a WindowResizeCallback to the container
       * @param cb The WindowResizeCallback to add
       */
      void AddCallback(WindowResizeCallback& cb);

      /** Remove an existing WindowResizeCallback from the container.
       * @param cb The Callback to remove from this container.
       */
      void RemoveCallback(dtCore::WindowResizeCallback &cb);

      /** Get a copy of all the callbacks in this container.
       * @return a copy of the internal Callback container
       */
      std::vector<RefPtr<WindowResizeCallback> > GetCallbacks() const;     

   protected:
      virtual ~WindowResizeContainer();
   	
   private:
      dtCore::ObserverPtr<dtCore::DeltaWin> mWindow; ///<observer pointer to avoid circular dependency
      std::vector<RefPtr<WindowResizeCallback> > mCallbacks;
   };
}

#endif // windowresizecontainer_h__
