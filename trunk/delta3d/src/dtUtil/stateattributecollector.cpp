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
* Matthew "w00by" Stokes
*/

#include <prefix/dtutilprefix.h>
#include <dtUtil/stateattributecollector.h>
#include <dtUtil/collectorutil.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>

#include <osg/Node>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Texture>


namespace dtUtil
{
   //Define all the different flags so that users can OR the variables together to create a mask
   const StateAttributeCollector::StateAttribFlag StateAttributeCollector::MaterialFlag =  dtUtil::Bits::Add(0,1);
   const StateAttributeCollector::StateAttribFlag StateAttributeCollector::ProgramFlag =  dtUtil::Bits::Add(0,2);
   const StateAttributeCollector::StateAttribFlag StateAttributeCollector::TextureFlag =  dtUtil::Bits::Add(0,4);

   //A Flag that will return all State Attributes defined above
   const StateAttributeCollector::StateAttribFlag StateAttributeCollector::AllAttributes = (StateAttributeCollector::MaterialFlag |
                                                                                     StateAttributeCollector::ProgramFlag |
                                                                                     StateAttributeCollector::TextureFlag);

   //StateVisitor Class
   //This object is used to traverse nodes and store their state attributes
   class StateVisitor : public osg::NodeVisitor
   {
   public:
      /**
      * Constructor for the StateVisitor Class 
      * @param NewStateAttributeCollector The StateAttributeCollector Object that wants to look for different StateAttributes
      * @param mask The different StateAttributes you wish to look for
      * @param nodeNamesIgnored A name of a StateAttribute that you don't want to look for
      */
      StateVisitor(StateAttributeCollector* NewStateAttributeCollector, StateAttributeCollector::StateAttribFlag mask, const std::string & nodeNamesIgnored)
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
         , mNodeManager(NewStateAttributeCollector)
         , mNodeMask(mask)
         , mNodeNamesIgnored(nodeNamesIgnored)
      {
         
      }

      /**
      * Function that touches a geode and stores any StateAttributes found on it.  It also stores the StateAttributes of all the Drawable's that
      * may be attached to the geode.  It than traverses the geode's children.
      * @param geode A Geode Object that you wish to visit
      */
      virtual void apply(osg::Geode& geode)
      {
         osg::StateSet* ss_geode = geode.getStateSet();
         stateSetParser (ss_geode);

         unsigned pNumDrawables = geode.getNumDrawables();

         //Traverse through all the Drawable Objects associated with the Geode
         for(unsigned i = 0; i < pNumDrawables; ++i)
         {
            osg::Drawable* draw = geode.getDrawable(i);

            //Get the StateSet object associated with the Drawable
            osg::StateSet* ss = draw->getStateSet();
            stateSetParser(ss);
         }
         traverse(geode);

      }

      /**
      * Function that vists any none Geode node and stores any StateAttributes that are associated with that Node
      * @param group A Group Node Object that will be checked
      */
      virtual void apply(osg::Node & group)
      { 
           //Get the StateSet object associated with the Drawable
           osg::StateSet* ss = group.getStateSet();
           stateSetParser(ss);
           traverse(group);
      }
      
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   protected:

      /**
       * Supporter function that adds StateAttributes to their corresponding map
       * @param ss A StateSet that will be searched for StateAttributes
       * @note This is a supporter function that is used by all of the StateVistor's apply functions
       */
      void stateSetParser(osg::StateSet * ss)
      {
         if(ss)
         {
            //Get any Material Object out of the StateSet object
            osg::Material* mat = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
            if(mat)
            {
               //Check to see if the Material Object found is something we wish to add to the Material Object map
               if( (dtUtil::Bits::Has(mNodeMask, StateAttributeCollector::MaterialFlag)) && (mat->getName() != mNodeNamesIgnored))
               {
                  mNodeManager->AddMaterial(mat->getName(), (*mat));
               }

            }
            //Get any Program Object out of the StateSet object
            osg::Program* prg = dynamic_cast<osg::Program*>(ss->getAttribute(osg::StateAttribute::PROGRAM));
            if(prg)
            {
               //Check to see if the Program StateAttribute is something we wish to add to the Program StateAttribute Map
               if( (dtUtil::Bits::Has(mNodeMask, StateAttributeCollector::ProgramFlag)) && (prg->getName() != mNodeNamesIgnored))
               {
                  mNodeManager->AddProgram(prg->getName(), (*prg));
               }
            }
            
            //Search trough all of the StateSets Texture StateAttributes
            for(unsigned int i = 0; i < ss->getTextureAttributeList().size(); i++)
            {
               //Get any Texture Object out of the StateSet object
               osg::Texture* txt = dynamic_cast<osg::Texture*>( ss->getTextureAttribute(i, osg::StateAttribute::TEXTURE) );     
               if(txt)
               {
                  //Check to see if the Texture StateAttribute is something we wish to add to the Texture StateAttribute Map
                  if( (dtUtil::Bits::Has(mNodeMask, StateAttributeCollector::TextureFlag)) &&  (txt->getName() != mNodeNamesIgnored))
                  {
                     mNodeManager->AddTexture(txt->getName(), (*txt));
                  }
               }
            }

         }
      }

   private:
      //Manages the nodes that we wish to collect
      StateAttributeCollector* mNodeManager;

      //Represents the types of nodes we wish to collect
      StateAttributeCollector::StateAttribFlag mNodeMask;

      //Represents a default node name that we do NOT want to collect
      std::string mNodeNamesIgnored;

   };





   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////BEGIN NODE COLLECTOR CLASS DEFINITION////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Default Constructor
   StateAttributeCollector::StateAttributeCollector()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Constructor that when called will automatically generate the state attribute maps that you request
   StateAttributeCollector::StateAttributeCollector(osg::Node* NodeToLoad, StateAttributeCollector::StateAttribFlag mask, const std::string & nodeNamesIgnored)
   {
      if(NodeToLoad !=  NULL)
      {
         StateVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         LOG_ERROR("Null Pointer Sent to constructor.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that when called will automatically generate the state attribute maps that you request
   void StateAttributeCollector::CollectStateAttributes(osg::Node* NodeToLoad, StateAttributeCollector::StateAttribFlag mask, const std::string & nodeNamesIgnored)
   {
      if(NodeToLoad !=  NULL)
      {
         StateVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         LOG_ERROR("Null Pointer Sent to function.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is defined to clear all the maps of their contents.
   void StateAttributeCollector::ClearAll()
   {
      mMaterialNodeMap.clear();
      mProgramNodeMap.clear();
      mTextureNodeMap.clear();
      
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //Function that is used to add a Material StateAttribute to the Material map
   void StateAttributeCollector::AddMaterial(const std::string & key, osg::Material & data)
   {
      if( !CollectorUtil::AddNode(key, &data, mMaterialNodeMap) )
      {
         LOG_WARNING("Can NOT add Material with Duplicate Key: " + key);
      }
   }

   //Function that is used to add a Program StateAttribute to the Program map
   void StateAttributeCollector::AddProgram(const std::string & key, osg::Program & data)
   {
      if( !CollectorUtil::AddNode(key, &data, mProgramNodeMap) )
      {
         LOG_WARNING("Can NOT add Program with Duplicate Key: " + key);
      }
   }

   //Function that is used to add a Texture StateAttribute to the Texture map
   void StateAttributeCollector::AddTexture(const std::string & key, osg::Texture & data)
   {
      if( !CollectorUtil::AddNode(key, &data, mTextureNodeMap) )
      {
         LOG_WARNING("Can NOT add Texture with Duplicate Key: " + key);
      }
   }

  
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
   ///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a Materials State Attribute
   const osg::Material* StateAttributeCollector::GetMaterial(const std::string& objectName) const
   {
      return CollectorUtil::FindNodePointer(objectName, mMaterialNodeMap);
   }

   //Function that is used to request a CONST pointer to a Program State Attribute
   const osg::Program* StateAttributeCollector::GetProgram(const std::string& objectName) const
   {
      return CollectorUtil::FindNodePointer(objectName, mProgramNodeMap);
   }
   
   //Function that is used to request a CONST pointer to a Texture State Attribute
   const osg::Texture* StateAttributeCollector::GetTexture(const std::string& objectName) const
   {
      return CollectorUtil::FindNodePointer(objectName, mTextureNodeMap);
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
   ///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a Materials State Attribute
   osg::Material* StateAttributeCollector::GetMaterial(const std::string& objectName) 
   {
      return CollectorUtil::FindNodePointer(objectName, mMaterialNodeMap);
   }

   //Function that is used to request a pointer to a Program State Attribute
   osg::Program* StateAttributeCollector::GetProgram(const std::string& objectName) 
   {
      return CollectorUtil::FindNodePointer(objectName, mProgramNodeMap);
   }

   //Function that is used to request a pointer to a Texture State Attribute
   osg::Texture* StateAttributeCollector::GetTexture(const std::string& objectName) 
   {
      return CollectorUtil::FindNodePointer(objectName, mTextureNodeMap);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
   //////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST map populated with Material StateAttributes
   const StateAttributeCollector::MaterialNodeMap& StateAttributeCollector::GetMaterialMap() const
   {
      return mMaterialNodeMap;
   }

   //Function that returns a CONST map populated with Program StateAttributes
   const StateAttributeCollector::ProgramNodeMap& StateAttributeCollector::GetProgramMap() const
   {
      return mProgramNodeMap;
   }

   //Function that returns a CONST map populated with Texture StateAttributes
   const StateAttributeCollector::TextureNodeMap& StateAttributeCollector::GetTextureMap() const
   {
      return mTextureNodeMap;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
   //////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a map populated with Material StateAttributes
   StateAttributeCollector::MaterialNodeMap& StateAttributeCollector::GetMaterialMap() 
   {
      return mMaterialNodeMap;
   }

   //Function that returns a map populated with Program StateAttributes
   StateAttributeCollector::ProgramNodeMap& StateAttributeCollector::GetProgramMap() 
   {
      return mProgramNodeMap;
   }

   //Function that returns a map populated with Texture StateAttributes
   StateAttributeCollector::TextureNodeMap& StateAttributeCollector::GetTextureMap() 
   {
      return mTextureNodeMap;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Destructor
   StateAttributeCollector::~StateAttributeCollector()
   {

   }
}//namespace dtUtil
