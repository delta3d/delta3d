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

#include <dtDirector/node.h>
#include <dtDirector/eventnode.h>
#include <dtDirector/actionnode.h>
#include <dtDirector/latentactionnode.h>
#include <dtDirector/valuenode.h>
#include <dtDirector/colors.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/resourceactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/directortypefactory.h>

#include <dtUtil/stringutils.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Node::Node()
      : mLogNode(false)
      , mEnabled(true)
      , mIsEnabled(true)
      , mIsReadOnly(false)
      , mDirector(NULL)
      , mGraph(NULL)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node::Node(const Node& rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node::~Node()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node& Node::operator=(const Node& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Node::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      mGraph = graph;

      if (graph)
      {
         graph->AddNode(this);
         mDirector = graph->GetDirector();
      }

      mType = &nodeType;
      SetColorRGB(nodeType.GetColor());

      BuildPropertyMap();
      InitDefaults();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Node> Node::Clone(DirectorGraph* graph)
   {
      if (!graph)
      {
         return NULL;
      }

      // First tell the node manager to create a new node using this
      // nodes type.
      dtCore::RefPtr<Node> copy;

      try
      {
         copy = NodeManager::GetInstance().CreateNode(*mType, graph).get();
      }
      catch(const dtUtil::Exception &e)
      {
         std::ostringstream error;
         error << "Clone of Node: " << mType->GetName() << " failed. Reason was: " << e.What();
         LOG_ERROR(error.str());
         return NULL;
      }

      copy->SetID(mID);
      copy->CopyPropertiesFrom(*this);

      // Input Links.
      std::vector<InputLink>& inputs = GetInputLinks();
      int count = (int)inputs.size();
      //int saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         InputLink& input = inputs[index];
         InputLink& copyInput = copy->GetInputLinks()[index];
         copyInput.SetVisible(input.GetVisible());
      }

      // Output Links.
      std::vector<OutputLink>& outputs = GetOutputLinks();
      count = (int)outputs.size();
      //saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         OutputLink& output = outputs[index];
         OutputLink& copyOutput = copy->GetOutputLinks()[index];
         copyOutput.SetVisible(output.GetVisible());
      }

      // Value Links.
      std::vector<ValueLink>& values = GetValueLinks();
      count = (int)values.size();
      //saveCount = 0;
      for (int index = 0; index < count; index++)
      {
         ValueLink& value = values[index];
         ValueLink& copyValue = copy->GetValueLinks()[index];

         copyValue.SetExposed(value.GetExposed());
         copyValue.SetVisible(value.GetVisible());
      }

      return copy;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::BuildPropertyMap()
   {
      dtCore::BooleanActorProperty* enabledProp = new dtCore::BooleanActorProperty(
         "Enabled", "Enabled",
         dtCore::BooleanActorProperty::SetFuncType(this, &Node::SetEnabled),
         dtCore::BooleanActorProperty::GetFuncType(this, &Node::GetEnabled),
         "Enabled status of this Node (Disabled nodes will not run during graph execution).");
      AddProperty(enabledProp);

      AddProperty(new dtCore::StringActorProperty(
         "Comment", "Comment",
         dtCore::StringActorProperty::SetFuncType(this, &Node::SetComment),
         dtCore::StringActorProperty::GetFuncType(this, &Node::GetComment),
         "Generic text field used to describe why this node is here."));

      AddProperty(new dtCore::BooleanActorProperty(
         "LogNode", "Log Node",
         dtCore::BooleanActorProperty::SetFuncType(this, &Node::SetNodeLogging),
         dtCore::BooleanActorProperty::GetFuncType(this, &Node::GetNodeLogging),
         "Prints a log message when this node is executed."));

      dtCore::StringActorProperty* authorProp =
         new dtCore::StringActorProperty("Authors", "Node Author(s)",
         dtCore::StringActorProperty::SetFuncType(),
         dtCore::StringActorProperty::GetFuncType(this, &Node::GetAuthors),
         "The author(s) of this node, as well as all inherited nodes.", "Info");
      authorProp->SetReadOnly(true);
      AddProperty(authorProp);

      dtCore::StringActorProperty* typeProp = new dtCore::StringActorProperty(
         "Type", "Type",
         dtCore::StringActorProperty::SetFuncType(),
         dtCore::StringActorProperty::GetFuncType(this, &Node::GetTypeName),
         "The nodes type.", "Info");
      typeProp->SetReadOnly(true);
      AddProperty(typeProp);

      dtCore::StringActorProperty* descProp = new dtCore::StringActorProperty(
         "Description", "Description",
         dtCore::StringActorProperty::SetFuncType(),
         dtCore::StringActorProperty::GetFuncType(this, &Node::GetDescription),
         "Generic text field used to describe the basic functionality of this node.", "Info");
      descProp->SetReadOnly(true);
      AddProperty(descProp);

      AddProperty(new dtCore::Vec2ActorProperty(
         "Position", "Position",
         dtCore::Vec2ActorProperty::SetFuncType(this, &Node::SetPosition),
         dtCore::Vec2ActorProperty::GetFuncType(this, &Node::GetPosition),
         "The UI Position of the Node.", "Info"));

      mValues.push_back(ValueLink(this, enabledProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // Trigger the default out link.
      ActivateOutput();
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::Update(float simDelta, float delta, int input, bool firstUpdate, void*& data)
   {
      return Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (!link || !value) return false;

      if (GetDirector()->IsLoading())
      {
         return true;
      }

      if (link->IsTypeChecking())
      {
         if (!value->CanBeType(link->GetPropertyType()))
         {
            return false;
         }
         else if (link->GetPropertyType() == dtCore::DataType::ACTOR)
         {
            std::string desiredClass;
            dtCore::BaseActorObject* proxyValue = NULL;

            // Handle both: ActorIDActorProperty and ActorActorProperty (do nothing if redirected)
            if (IS_A(link->GetProperty(), dtCore::ActorIDActorProperty*))
               desiredClass = static_cast<dtCore::ActorIDActorProperty*>(link->GetProperty())->GetDesiredActorClass();
            else if (IS_A(value->GetProperty(), dtCore::ActorActorProperty*))
               desiredClass = static_cast<dtCore::ActorActorProperty*>(link->GetProperty())->GetDesiredActorClass();

            if (IS_A(value->GetProperty(), dtCore::ActorIDActorProperty*) || IS_A(value->GetProperty(), dtCore::ActorActorProperty*))
               proxyValue = value->GetActor(value->GetProperty()->GetName());

            if (proxyValue && !desiredClass.empty() && !proxyValue->IsInstanceOf(desiredClass))
               return false;
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::OnLinkValueChanged(const std::string& linkName)
   {
      // As a special case to help optimize a very highly used property
      // that can also be exposed as a value link.  Any time a value is
      // connected to the Enabled flag value link, it will change the
      // internal value stored within this node.  This reduces the need
      // to check for connected values whenever we need to check if the
      // node is enabled.
      if (linkName == "Enabled")
      {
         mIsEnabled = GetBoolean("Enabled");
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const NodeType& Node::GetType() const
   {
      return *mType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::ObjectType& Node::GetObjectType() const { return  *mType; }


   ////////////////////////////////////////////////////////////////////////////////
   bool Node::SetID(const ID& id)
   {
      bool result = true;
      if (id.index != mID.index)
      {
         result = SetIDIndex(id.index);
      }

      SetID(id.id);
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::SetIDIndex(int index)
   {
      mDirector->MasterListRemoveNode(this);

      return mDirector->MasterListAddNode(this, index);
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Node::GetName()
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Node::GetTypeName() const
   {
      return GetType().GetName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Node::GetDescription()
   {
      return GetType().GetDescription();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Node::GetComment() const
   {
      return mComment;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetComment(const std::string& comment)
   {
      mComment = comment;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::GetNodeLogging() const
   {
      return mLogNode;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetNodeLogging(bool log)
   {
      mLogNode = log;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::AddAuthor(const std::string& author)
   {
      if (author.empty()) return;

      // Check if this author already exists.
      int count = (int)mAuthorList.size();
      for (int index = 0; index < count; index++)
      {
         if (mAuthorList[index] == author)
         {
            // Remove the author from the list,
            // so it will be re-added to the end.
            mAuthorList.erase(mAuthorList.begin() + index);
            break;
         }
      }

      // Push the new author to the list.
      mAuthorList.push_back(author);

      // Now set the author string.  We do last to first order
      // because more recent authors are appended to the end.
      mAuthors.clear();
      count = (int)mAuthorList.size();
      for (int index = count - 1; index >= 0; index--)
      {
         if (index < count - 1) mAuthors += ", ";
         mAuthors += mAuthorList[index];
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::GetEnabled() const
   {
      return mEnabled;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetEnabled(bool enabled)
   {
      mEnabled = enabled;
      mIsEnabled = GetBoolean("Enabled");
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsEnabled()
   {
      // We no longer check for link redirections when checking this
      // flag because we now copy any linked value to the internal
      // enabled flag stored in this node.  This is for optimization
      // purposes.
      ////return GetBoolean("Enabled");

      return mIsEnabled && GetDirector()->IsEnabled();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Director* Node::GetTopDirector() const
   {
      Director* result = mDirector;

      while (result && result->GetParent())
      {
         result = result->GetParent();
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::Disconnect()
   {
      bool result = false;

      // Disconnect inputs.
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         result |= mInputs[index].Disconnect();
      }

      // Disconnect outputs.
      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         result |= mOutputs[index].Disconnect();
      }

      // Disconnect values.
      count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         result |= mValues[index].Disconnect();
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Node::GetValueNodeCount(const std::string& name)
   {
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtCore::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return (int)mValues[valueIndex].GetLinks().size();
         }
      }

      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueNode* Node::GetValueNode(const std::string& name, int index)
   {
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtCore::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            if (index < (int)mValues[valueIndex].GetLinks().size())
            {
               return mValues[valueIndex].GetLinks()[index];
            }

            return NULL;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   int Node::GetPropertyCount(const std::string& name)
   {
      int propertyCount = 0;

      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtCore::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            propertyCount = mValues[valueIndex].GetPropertyCount();
            break;
         }
      }

      // Did not find any overrides, so return the default.
      if (propertyCount == 0 && dtCore::PropertyContainer::GetProperty(name))
      {
         return 1;
      }

      return propertyCount;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* Node::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtCore::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetProperty(index, outNode);
         }
      }

      // Did not find any overrides, so return the default.
      if (index == 0)
      {
         return dtCore::PropertyContainer::GetProperty(name);
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DataType& Node::GetPropertyType(const std::string& name, int index)
   {
      ValueNode* node = NULL;

      dtCore::ActorProperty* prop = GetProperty(name, index, &node);
      if (node != NULL)
      {
         return node->GetPropertyType();
      }
      else if (prop)
      {
         return prop->GetDataType();
      }

      return dtCore::DataType::UNKNOWN;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::LogValueRetrieved(ValueNode* valueNode, dtCore::ActorProperty* prop)
   {
      // Log the comment for this value.
      if (GetDirector()->GetNodeLogging() && valueNode && valueNode->GetNodeLogging())
      {
         std::string message = "Value Node \'" + valueNode->GetName();
         if (!valueNode->GetComment().empty())
         {
            message += " - " + valueNode->GetComment();
         }
         message += "\' retrieved " + prop->GetValueString();
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, message);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::LogValueChanged(ValueNode* valueNode, dtCore::ActorProperty* prop, const std::string& oldVal)
   {
      // Log the comment for this value.
      if (GetDirector()->GetNodeLogging() && valueNode && valueNode->GetNodeLogging())
      {
         std::string message = "Value Node \'" + valueNode->GetName();
         if (!valueNode->GetComment().empty())
         {
            message += " - " + valueNode->GetComment();
         }
         message += "\' was changed from " + oldVal + " to " + prop->GetValueString();
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__, message);
      }

      if (valueNode)
      {
         valueNode->OnValueChanged();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Node::GetValueNodeValue(const std::string& name, int index)
   {
      ValueNode* valueNode = GetValueNode(name, index);
      if (valueNode)
      {
         return valueNode->GetFormattedValue();
      }
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::GetBoolean(const std::string& name, int index)
   {
      return GetPropertyValue<bool>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   int Node::GetInt(const std::string& name, int index)
   {
      return GetPropertyValue<int>(name, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int Node::GetUInt(const std::string& name, int index)
   {
      return GetPropertyValue<unsigned int>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   float Node::GetFloat(const std::string& name, int index)
   {
      return GetPropertyValue<float>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   double Node::GetDouble(const std::string& name, int index)
   {
      return GetPropertyValue<double>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string Node::GetString(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop) return prop->ToString();
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 Node::GetVec2(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop)
      {
         std::string str = prop->ToString();

         osg::Vec2 newValue;
         dtUtil::ParseVec<osg::Vec2>(str, newValue, 2);
         return newValue;
      }

      return osg::Vec2();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Node::GetVec3(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop)
      {
         std::string str = prop->ToString();

         osg::Vec3 newValue;
         dtUtil::ParseVec<osg::Vec3>(str, newValue, 3);
         return newValue;
      }

      return osg::Vec3();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Node::GetVec4(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop)
      {
         std::string str = prop->ToString();

         osg::Vec4 newValue;
         dtUtil::ParseVec<osg::Vec4>(str, newValue, 4);
         return newValue;
      }

      return osg::Vec4();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId Node::GetActorID(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop) return dtCore::UniqueId(prop->ToString());

      dtCore::UniqueId emptyID;
      emptyID = "";
      return emptyID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* Node::GetActor(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (!prop) return NULL;

      dtCore::ActorIDActorProperty* actorIdProp = dynamic_cast<dtCore::ActorIDActorProperty*>(prop);
      if (actorIdProp)
      {
         dtCore::BaseActorObject* actor = NULL;

         dtCore::UniqueId id = actorIdProp->GetValue();

         if (GetDirector()->GetGameManager())
         {
            GetDirector()->GetGameManager()->FindActorById(id, actor);
         }

         if (actor == NULL)
         {
            actor = actorIdProp->GetActor();
         }

         return actor;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::GameEvent* Node::GetGameEvent(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (prop == NULL) return NULL;

      dtCore::GameEventActorProperty* eventProp = dynamic_cast<dtCore::GameEventActorProperty*>(prop);
      if (eventProp)
      {
         return eventProp->GetValue();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor Node::GetResource(const std::string& name, int index)
   {
      dtCore::ActorProperty* prop = GetProperty(name, index);
      if (!prop) return dtCore::ResourceDescriptor::NULL_RESOURCE;

      dtCore::ResourceActorProperty* resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(prop);
      if (resourceProp)
      {
         return resourceProp->GetValue();
      }


      std::string resourceIdentifier = prop->ToString();
      if (!resourceIdentifier.empty())
      {
         return dtCore::ResourceDescriptor(resourceIdentifier);
      }

      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetValueNodeValue(const std::string& value, const std::string& name, int index)
   {
      if (index < 0)
      {
         int count = GetValueNodeCount(name);
         for (index = 0; index < count; ++index)
         {
            ValueNode* valueNode = GetValueNode(name, index);
            if (valueNode)
            {
               valueNode->SetFormattedValue(value);
            }
         }
      }
      else
      {
         ValueNode* valueNode = GetValueNode(name, index);
         if (valueNode)
         {
            valueNode->SetFormattedValue(value);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetBoolean(bool value, const std::string& name, int index)
   {
      SetPropertyValue<bool>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetInt(int value, const std::string& name, int index)
   {
      SetPropertyValue<int>(value, name, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetUInt(unsigned int value, const std::string& name, int index)
   {
      SetPropertyValue<unsigned int>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetFloat(float value, const std::string& name, int index)
   {
      SetPropertyValue<float>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetDouble(double value, const std::string& name, int index)
   {
      SetPropertyValue<double>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetString(const std::string& value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               prop->FromString(value);
            }
         }
      }
      else
      {
         dtCore::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            prop->FromString(value);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetVec2(osg::Vec2 value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               std::ostringstream stream;
               stream.precision(2 * sizeof(float) + 1);
               stream << value;

               prop->FromString(stream.str());
            }
         }
      }
      else
      {
         dtCore::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            std::ostringstream stream;
            stream.precision(2 * sizeof(float) + 1);
            stream << value;

            prop->FromString(stream.str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetVec3(osg::Vec3 value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               std::ostringstream stream;
               stream.precision(2 * sizeof(float) + 1);
               stream << value;

               prop->FromString(stream.str());
            }
         }
      }
      else
      {
         dtCore::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            std::ostringstream stream;
            stream.precision(2 * sizeof(float) + 1);
            stream << value;

            prop->FromString(stream.str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetVec4(osg::Vec4 value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               std::ostringstream stream;
               stream.precision(2 * sizeof(float) + 1);
               stream << value;

               prop->FromString(stream.str());
            }
         }
      }
      else
      {
         dtCore::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            std::ostringstream stream;
            stream.precision(2 * sizeof(float) + 1);
            stream << value;

            prop->FromString(stream.str());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetActorID(const dtCore::UniqueId& value, const std::string& name, int index)
   {
      SetString(value.ToString(), name, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetGameEvent(dtCore::GameEvent* value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = GetProperty(name, index);
            dtCore::GameEventActorProperty* eventProp = dynamic_cast<dtCore::GameEventActorProperty*>(prop);
            if (eventProp)
            {
               eventProp->SetValue(value);
            }
         }
      }
      else
      {
         dtCore::ActorProperty* prop = GetProperty(name, index);
         dtCore::GameEventActorProperty* eventProp = dynamic_cast<dtCore::GameEventActorProperty*>(prop);
         if (eventProp)
         {
            eventProp->SetValue(value);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetResource(const dtCore::ResourceDescriptor& value, const std::string& name, int index)
   {
      SetString(value.GetResourceIdentifier(), name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   InputLink* Node::GetInputLink(const std::string& name)
   {
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mInputs[index].GetName() == name)
         {
            return &mInputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   OutputLink* Node::GetOutputLink(const std::string& name)
   {
      int count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mOutputs[index].GetName() == name)
         {
            return &mOutputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const ValueLink* Node::GetValueLink(const std::string& name) const
   {
      int count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         if (mValues[index].GetName() == name)
         {
            return &mValues[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const InputLink* Node::GetInputLink(const std::string& name) const
   {
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mInputs[index].GetName() == name)
         {
            return &mInputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const OutputLink* Node::GetOutputLink(const std::string& name) const
   {
      int count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mOutputs[index].GetName() == name)
         {
            return &mOutputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   ValueLink* Node::GetValueLink(const std::string& name)
   {
      int count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         if (mValues[index].GetName() == name)
         {
            return &mValues[index];
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<OutputLink> Node::GetRemovedImportedInputLinkConnections(const std::string& inputName) const
   {
      std::vector<OutputLink> links;

      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const InputLink* myInput = GetInputLink(inputName);
            const InputLink* oInput = originalNode->GetInputLink(inputName);
            if (myInput && oInput)
            {
               const std::vector<OutputLink*>& myLinks = myInput->GetLinks();
               const std::vector<OutputLink*>& oLinks = oInput->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  OutputLink* oOutput = oLinks[oLinkIndex];
                  if (oOutput)
                  {
                     bool found = false;
                     for (int myLinkIndex = 0; myLinkIndex < (int)myLinks.size(); ++myLinkIndex)
                     {
                        OutputLink* myOutput = myLinks[myLinkIndex];
                        if (myOutput &&
                           oOutput->GetOwner()->GetID() == myOutput->GetOwner()->GetID() &&
                           oOutput->GetName() == myOutput->GetName())
                        {
                           found = true;
                           break;
                        }
                     }

                     if (!found)
                     {
                        OutputLink removedLink(oOutput->GetOwner(), oOutput->GetName());
                        links.push_back(removedLink);
                     }
                  }
               }
            }
         }
      }

      return links;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<InputLink> Node::GetRemovedImportedOutputLinkConnections(const std::string& outputName) const
   {
      std::vector<InputLink> links;

      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const OutputLink* myOutput = GetOutputLink(outputName);
            const OutputLink* oOutput = originalNode->GetOutputLink(outputName);
            if (myOutput && oOutput)
            {
               const std::vector<InputLink*>& myLinks = myOutput->GetLinks();
               const std::vector<InputLink*>& oLinks = oOutput->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  InputLink* oInput = oLinks[oLinkIndex];
                  if (oInput)
                  {
                     bool found = false;
                     for (int myLinkIndex = 0; myLinkIndex < (int)myLinks.size(); ++myLinkIndex)
                     {
                        InputLink* myInput = myLinks[myLinkIndex];
                        if (myInput &&
                           oInput->GetOwner()->GetID() == myInput->GetOwner()->GetID() &&
                           oInput->GetName() == myInput->GetName())
                        {
                           found = true;
                           break;
                        }
                     }

                     if (!found)
                     {
                        InputLink removedLink(oInput->GetOwner(), oInput->GetName());
                        links.push_back(removedLink);
                     }
                  }
               }
            }
         }
      }

      return links;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<ValueNode*> Node::GetRemovedImportedValueLinkConnections(const std::string& valueName) const
   {
      std::vector<ValueNode*> links;

      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const ValueLink* myValue = GetValueLink(valueName);
            const ValueLink* oValue = originalNode->GetValueLink(valueName);
            if (myValue && oValue)
            {
               const std::vector<ValueNode*>& myLinks = myValue->GetLinks();
               const std::vector<ValueNode*>& oLinks = oValue->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  ValueNode* oValueNode = oLinks[oLinkIndex];
                  if (oValueNode)
                  {
                     bool found = false;
                     for (int myLinkIndex = 0; myLinkIndex < (int)myLinks.size(); ++myLinkIndex)
                     {
                        ValueNode* myValueNode = myLinks[myLinkIndex];
                        if (myValueNode &&
                           oValueNode->GetID() == myValueNode->GetID())
                        {
                           found = true;
                           break;
                        }
                     }

                     if (!found)
                     {
                        links.push_back(oValueNode);
                     }
                  }
               }
            }
         }
      }

      return links;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsInputLinkImported(const std::string& inputName, const ID& targetID, const std::string& outputName) const
   {
      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const InputLink* myInput = GetInputLink(inputName);
            const InputLink* oInput = originalNode->GetInputLink(inputName);
            if (myInput && oInput)
            {
               const std::vector<OutputLink*>& oLinks = oInput->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  OutputLink* oOutput = oLinks[oLinkIndex];
                  if (oOutput &&
                     oOutput->GetOwner()->GetID() == targetID &&
                     oOutput->GetName() == outputName)
                  {
                     return true;
                  }
               }
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsOutputLinkImported(const std::string& outputName, const ID& targetID, const std::string& inputName) const
   {
      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const OutputLink* myOutput = GetOutputLink(outputName);
            const OutputLink* oOutput = originalNode->GetOutputLink(outputName);
            if (myOutput && oOutput)
            {
               const std::vector<InputLink*>& oLinks = oOutput->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  InputLink* oInput = oLinks[oLinkIndex];
                  if (oInput &&
                     oInput->GetOwner()->GetID() == targetID &&
                     oInput->GetName() == inputName)
                  {
                     return true;
                  }
               }
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsValueLinkImported(const std::string& valueName, const ID& targetID) const
   {
      if (IsImported())
      {
         const Node* originalNode = GetOriginalImportedNode();
         if (originalNode)
         {
            const ValueLink* myValue = GetValueLink(valueName);
            const ValueLink* oValue = originalNode->GetValueLink(valueName);
            if (myValue && oValue)
            {
               const std::vector<ValueNode*>& oLinks = oValue->GetLinks();

               for (int oLinkIndex = 0; oLinkIndex < (int)oLinks.size(); ++oLinkIndex)
               {
                  const ValueNode* oValueNode = oLinks[oLinkIndex];
                  if (oValueNode && oValueNode->GetID() == targetID)
                  {
                     return true;
                  }
               }
            }
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::ActivateOutput(const std::string& name)
   {
      OutputLink* link = GetOutputLink(name);
      if (link)
      {
         link->Activate();
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::InputsExposed()
   {
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::OutputsExposed()
   {
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::ValuesExposed()
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventNode* Node::AsEventNode()
   {
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActionNode* Node::AsActionNode()
   {
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   LatentActionNode* Node::AsLatentNode()
   {
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueNode* Node::AsValueNode()
   {
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsImported() const
   {
      if (mDirector)
      {
         return mDirector->IsImported();
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsReadOnly() const
   {
      return mIsReadOnly | IsImported();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Node* Node::GetOriginalImportedNode() const
   {
      if (IsImported())
      {
         Director* topDirector = GetTopDirector();

         // Get all our imported scripts and find their original cached equivalent for comparison.
         const std::vector<dtCore::RefPtr<Director> >& importedScripts = topDirector->GetImportedScriptList();

         DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
         std::vector<const Director*> cachedScripts;
         if (factory)
         {
            int count = (int)importedScripts.size();
            for (int index = 0; index < count; ++index)
            {
               const Director* cachedScript = factory->GetCachedScript(importedScripts[index]->GetScriptName());
               cachedScripts.push_back(cachedScript);
            }
         }

         int cacheCount = (int)cachedScripts.size();
         for (int cacheIndex = 0; cacheIndex < cacheCount; ++cacheIndex)
         {
            const Director* cachedScript = cachedScripts[cacheIndex];
            if (cachedScript)
            {
               return cachedScript->GetNode(GetID(), true);
            }
         }
      }

      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
