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

#ifndef DIRECTOR_NODE
#define DIRECTOR_NODE


#include <string>
#include <vector>

#include <dtDirector/export.h>
#include <dtDirector/nodetype.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>
#include <dtDirector/valuelink.h>

#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/mswinmacros.h>

#include <dtDAL/propertycontainer.h>

#include <osg/Vec2>

namespace dtDAL
{
   class BaseActorObject;
   class GameEvent;
}

namespace dtDirector
{
   class Director;
   class DirectorGraph;

   /**
    * This is the base class for all node objects.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT Node : public dtDAL::PropertyContainer
   {
   public:

      /**
       * Constructs the Node.
       */
      Node();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * Event handler called after the entire script has been loaded.
       */
      virtual void OnStart() {}

      /**
       * Creates a copy of this node and returns it.  The method uses the
       * node manager to create the clone and then iterates though the
       * current state of this node's property set and copies their values
       * to the newly created clone.
       *
       * @return The cloned node.
       */
      virtual dtCore::RefPtr<Node> Clone();

      /**
       * This method is called in init, which instructs the node
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();

      /**
       * Updates the node.
       * @note  Parent implementation will auto activate any trigger
       *        with the "Out" label by default.
       *
       * @param[in]  simDelta     The simulation time step.
       * @param[in]  delta        The real time step.
       * @param[in]  input        The index to the input that is active.
       * @param[in]  firstUpdate  True if this input was just activated,
       *
       * @return     True if the current node should remain active.
       */
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      /**
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(ValueLink* link, ValueNode* value);

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName) {}

      /**
       * Retrieves the type of this node.
       *
       * @return  The type of this node.
       */
      const NodeType& GetType();

      /**
       * Retrieves the ID of the node.
       *
       * @return  The unique ID.
       */
      const dtCore::UniqueId& GetID() const {return mID;}

      /**
       * Sets the ID of the node.
       *
       * @param[in]  id  The ID.
       */
      const void SetID(const dtCore::UniqueId& id) {mID = id;}

      /**
       * Retrieves the display name for the node.
       *
       * @return  The display name of the node.
       */
      virtual const std::string& GetName();

      /**
       * Accessors for the name of the input node.
       */
      virtual void SetName(const std::string& name);

      /**
       * Retrieves the description for the node.
       *
       * @return  The description of the node.
       */
      const std::string& GetTypeName();

      /**
       * Retrieves the description for the node.
       *
       * @return  The description of the node.
       */
      const std::string& GetDescription();

      /**
       * Retrieves the comment for the node.
       *
       * @return  The comment of the node.
       */
      const std::string& GetComment() const;

      /**
       * Sets the comment for the node.
       *
       * @param[in]  comment  The comment.
       */
      void SetComment(const std::string& comment);

      /**
       * Retrieves whether this node will output its comment
       * to the log window.
       *
       * @return  True if the node will output a log.
       */
      bool GetNodeLogging() const;

      /**
       * Sets whether this node will output its comment
       * to the log window.
       *
       * @param[in]  log  True to output a log.
       */
      void SetNodeLogging(bool log);

      /**
       * Retrieves the authors of this node.
       */
      const std::string& GetAuthors() {return mAuthors;}

      /**
       * Adds an author for this node.
       *
       * @param[in]  author  The author.
       */
      void AddAuthor(const std::string& author);

      /**
       * Retrieves the UI position of the node.
       *
       * @return  The Position.
       */
      osg::Vec2 GetPosition() const {return mPosition;}

      /**
       * Sets the UI position of the node.
       *
       * @param[in]  pos  The position.
       */
      void SetPosition(const osg::Vec2& pos) {mPosition = pos;}

      /**
       * Sets the enabled status of the node.
       *
       * @param[in]  enabled   True to enable.
       */
      void SetEnabled(bool enabled);

      /**
       * Retrieves whether this node is enabled.
       *
       * @return  True if enabled.
       */
      bool IsEnabled();

      /**
       * Retrieves the director.
       *
       * @return  The director.
       */
      Director* GetDirector() {return mDirector;}

      /**
       * Retrieves the graph.
       *
       * @return  The graph.
       */
      DirectorGraph* GetGraph() {return mGraph;}

      /**
       * Disconnects all links from this node.
       *
       * @return  True if disconnections were made.
       */
      virtual bool Disconnect();

      /**
       * Retrieves the total number of values linked to a property.
       *
       * @param[in]  name  The name of the property.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

      /**
       * Retrieves a property of the given name.  This is overloaded
       * to provide functionality of redirected properties (from the
       * use of ValueLink's).
       *
       * @param[in]  name     The name of the property.
       * @param[in]  index    The property index, in case of multiple linking.
       * @param[in]  outNode  If provided, will retrieve the value node that owns this property (if any).
       *
       * @return     A pointer to the property, NULL if none found.
       *
       * @note  All properties used within nodes should be retrieved
       *         via this method instead of directly to ensure that
       *         the desired property is being used.
       */
      virtual dtDAL::ActorProperty* GetProperty(const std::string& name, int index = 0, ValueNode** outNode = NULL);

      /**
       * Retrieves the type of the property of the given name.
       *
       * @param[in]  name     The name of the property.
       * @param[in]  index    The property index, in case of multiple linking.
       *
       * @return     The Data Type of the property.
       */
      dtDAL::DataType& GetPropertyType(const std::string& name, int index = 0);

      /**
      * Logs when a value is retrieved.
      *
      * @param[in]  valueNode  The value node that was retrieved.
      * @param[in]  prop       The property of that value node.
      */
      void LogValueRetrieved(ValueNode* valueNode, dtDAL::ActorProperty* prop);

      /**
      * Logs when a value is changed.
      *
      * @param[in]  valueNode  The value node that was retrieved.
      * @param[in]  prop       The property of that value node.
      */
      void LogValueChanged(ValueNode* valueNode, dtDAL::ActorProperty* prop, const std::string& oldVal);

      /**
       * This method is provided for ease of use, it will
       * retrieve a property's value and convert it to the type of
       * your choice.
       *
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *
       * @note  All properties used within nodes should be retrieved
       *         via these methods instead of directly to ensure that
       *         the desired property is being used.
       */
      template<typename T>
      T GetPropertyValue(const std::string& name = "Value", int index = 0)
      {
         T result = 0;
         ValueNode* node = NULL;
         dtDAL::ActorProperty* prop = GetProperty(name, index, &node);
         if (prop)
         {
            std::string val = prop->ToString();
            
            // Special case for boolean values.
            if (prop->GetDataType() == dtDAL::DataType::BOOLEAN)
            {
               if (val == "true") val = "1";
               else if (val == "false") val = "0";
            }

            result = dtUtil::ToType<T>(val);

            LogValueRetrieved(node, prop);
         }
         return result;
      }

      bool GetBoolean(const std::string& name = "Value", int index = 0);
      int GetInt(const std::string& name = "Value", int index = 0);
      float GetFloat(const std::string& name = "Value", int index = 0);
      double GetDouble(const std::string& name = "Value", int index = 0);
      std::string GetString(const std::string& name = "Value", int index = 0);
      osg::Vec4 GetVec(const std::string& name = "Value", int index = 0);
      dtCore::UniqueId GetActorID(const std::string& name = "Value", int index = 0);
      dtDAL::BaseActorObject* GetActor(const std::string& name = "Value", int index = 0);
      dtDAL::GameEvent* GetGameEvent(const std::string& name = "Value", int index = 0);

      /**
       * This method is provided for ease of use, it will
       * allow you to set the value of a property.
       *
       * @param[in]  value  The new value to set.
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *                     Use -1 to set all linked properties.
       *
       * @note  All properties used within nodes should be retrieved
       *         via these methods instead of directly to ensure that
       *         the desired property is being used.
       */
      template<typename T>
      void SetPropertyValue(T value, const std::string& name = "Value", int index = -1)
      {
         if (index == -1)
         {
            int count = GetPropertyCount(name);
            for (index = 0; index < count; index++)
            {
               ValueNode* node = NULL;
               dtDAL::ActorProperty* prop = GetProperty(name, index, &node);
               if (prop)
               {
                  std::string oldVal = prop->GetValueString();

                  std::string val = dtUtil::ToString(value);
                  prop->FromString(val);

                  LogValueChanged(node, prop, oldVal);
               }
            }
         }
         else
         {
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               std::string val = dtUtil::ToString(value);
               prop->FromString(val);
            }
         }
      }

      void SetBoolean(bool value, const std::string& name = "Value", int index = -1);
      void SetInt(int value, const std::string& name = "Value", int index = -1);
      void SetFloat(float value, const std::string& name = "Value", int index = -1);
      void SetDouble(double value, const std::string& name = "Value", int index = -1);
      void SetVec(osg::Vec4 value, const std::string& name = "Value", int index = -1);
      void SetString(const std::string& value, const std::string& name = "Value", int index = -1);
      void SetActorID(const dtCore::UniqueId& value, const std::string& name = "Value", int index = -1);
      void SetGameEvent(dtDAL::GameEvent* value, const std::string& name = "Value", int index = -1);

      /**
       * Retrieves the input list.
       */
      std::vector<InputLink>& GetInputLinks() {return mInputs;}

      /**
       * Retrieves the output list.
       */
      std::vector<OutputLink>& GetOutputLinks() {return mOutputs;}

      /**
       * Retrieves the value list.
       */
      std::vector<ValueLink>& GetValueLinks() {return mValues;}

      /**
       * Retrieves a link of a given name.
       *
       * @param[in]  name  The name of the link to find.
       *
       * @return     A Pointer to the link, or NULL if not found.
       */
      InputLink* GetInputLink(const std::string& name);
      OutputLink* GetOutputLink(const std::string& name);
      ValueLink* GetValueLink(const std::string& name);

      /**
       * Retrieves whether the UI should expose input links
       * assigned to this node.
       *
       * @return  True to expose inputs.
       */
      virtual bool InputsExposed();

      /**
       * Retrieves whether the UI should expose output links
       * assigned to this node.
       *
       * @return  True to expose outputs.
       */
      virtual bool OutputsExposed();

      /**
       * Retrieves whether the UI should expose the value links
       * assigned to this node.
       *
       * @return  True to expose values.
       */
      virtual bool ValuesExposed();


      /**
       * Registration function for registering message types this
       * node will receive.
       * Note: This should be rewritten in inherited classes.
       */
      virtual void RegisterMessages() {}
      virtual void UnRegisterMessages() {}

      /**
       * Retrieves the UI color of the node.
       *
       * @return  The Color.
       */
      osg::Vec4 GetColor() const {return mColor;}

      /**
       * Sets the UI color of the node.
       *
       * @param[in]  color  The color.
       */
      void SetColor(const osg::Vec4& color) { mColor = color; }
      void SetColorRGB(const osg::Vec3& color) { SetColor(osg::Vec4(color, 225)/255.0f); }
      void SetColorRGBA(int r, int g, int b, int a = 225) { SetColor(osg::Vec4(r, g, b, a)/255.0f); }


      /**
       * Allow access to the NodePluginRegistry.
       */
      friend class NodePluginRegistry;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Node();

      /// Links
      std::vector<InputLink>  mInputs;
      std::vector<OutputLink> mOutputs;
      std::vector<ValueLink>  mValues;

      osg::Vec4      mColor;
      std::string    mName;
     
   private:
      /**
       * Hidden Copy Constructor.
       *
       * @param[in]  Node  The node to copy from.
       */
      Node(const Node&);

      /**
       *	Hidden Assignment operator.
       *
       * @param[in]  left   This.
       * @param[in]  right  The node to copy from.
       */
      Node& operator=(const Node&);

      /**
       * Retrieves the enabled status of the node.
       *
       * @return  True if the node is enabled.
       */
      bool GetEnabled() const;

      /**
       *	Sets the types of the node.
       * This can only be called from the NodeManager.
       *
       * @param[in]  type  The type of the node.
       */
      void SetType(const NodeType& type);


      // Properties.
      dtCore::UniqueId   mID;
      std::string        mComment;
      bool               mLogNode;
      std::string        mAuthors;
      std::vector<std::string> mAuthorList;

      osg::Vec2          mPosition;

      bool               mEnabled;

      Director*          mDirector;
      DirectorGraph*     mGraph;

#if defined DELTA_WIN32
#pragma warning (push)
#pragma warning (disable:4251)
#endif //DELTA_WIN32
      dtCore::RefPtr<const NodeType>   mType;
#if defined DELTA_WIN32
#pragma warning (pop)
#endif //DELTA_WIN32
   };
}

#endif
