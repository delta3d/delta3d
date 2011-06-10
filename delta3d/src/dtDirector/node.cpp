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
#include <dtDirector/valuenode.h>
#include <dtDirector/colors.h>

#include <dtDAL/actoractorproperty.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/resourceactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/nodemanager.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Node::Node()
      : mLogNode(false)
      , mEnabled(true)
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

      SetType(nodeType);
      SetColorRGB(nodeType.GetColor());

      BuildPropertyMap();
      InitDefaults();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Node> Node::Clone()
   {
      std::ostringstream error;

      // First tell the node manager to create a new node using this
      // nodes type.
      dtCore::RefPtr<Node> copy;

      try
      {
         copy = NodeManager::GetInstance().CreateNode(*mType, mGraph).get();
      }
      catch(const dtUtil::Exception &e)
      {
         error << "Clone of Commander Node: " << mType->GetName() << " failed. Reason was: " << e.What();
         LOG_ERROR(error.str());
         return NULL;
      }

      copy->CopyPropertiesFrom(*this);

      return copy;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::BuildPropertyMap()
   {
      dtDAL::BooleanActorProperty* enabledProp = new dtDAL::BooleanActorProperty(
         "Enabled", "Enabled",
         dtDAL::BooleanActorProperty::SetFuncType(this, &Node::SetEnabled),
         dtDAL::BooleanActorProperty::GetFuncType(this, &Node::GetEnabled),
         "Enabled status of this Node (Disabled nodes will not run during graph execution).");
      AddProperty(enabledProp);

      AddProperty(new dtDAL::StringActorProperty(
         "Comment", "Comment",
         dtDAL::StringActorProperty::SetFuncType(this, &Node::SetComment),
         dtDAL::StringActorProperty::GetFuncType(this, &Node::GetComment),
         "Generic text field used to describe why this node is here."));

      AddProperty(new dtDAL::BooleanActorProperty(
         "LogNode", "Log Node",
         dtDAL::BooleanActorProperty::SetFuncType(this, &Node::SetNodeLogging),
         dtDAL::BooleanActorProperty::GetFuncType(this, &Node::GetNodeLogging),
         "Prints a log message when this node is executed."));

      dtDAL::StringActorProperty* authorProp =
         new dtDAL::StringActorProperty("Authors", "Node Author(s)",
         dtDAL::StringActorProperty::SetFuncType(),
         dtDAL::StringActorProperty::GetFuncType(this, &Node::GetAuthors),
         "The author(s) of this node, as well as all inherited nodes.", "Info");
      authorProp->SetReadOnly(true);
      AddProperty(authorProp);

      dtDAL::StringActorProperty* typeProp = new dtDAL::StringActorProperty(
         "Type", "Type",
         dtDAL::StringActorProperty::SetFuncType(),
         dtDAL::StringActorProperty::GetFuncType(this, &Node::GetTypeName),
         "The nodes type.", "Info");
      typeProp->SetReadOnly(true);
      AddProperty(typeProp);

      dtDAL::StringActorProperty* descProp = new dtDAL::StringActorProperty(
         "Description", "Description",
         dtDAL::StringActorProperty::SetFuncType(),
         dtDAL::StringActorProperty::GetFuncType(this, &Node::GetDescription),
         "Generic text field used to describe the basic functionality of this node.", "Info");
      descProp->SetReadOnly(true);
      AddProperty(descProp);

      AddProperty(new dtDAL::Vec2ActorProperty(
         "Position", "Position",
         dtDAL::Vec2ActorProperty::SetFuncType(this, &Node::SetPosition),
         dtDAL::Vec2ActorProperty::GetFuncType(this, &Node::GetPosition),
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

   //////////////////////////////////////////////////////////////////////////
   bool Node::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (!link || !value) return false;

      if (link->IsTypeChecking())
      {
         if (!value->CanBeType(link->GetPropertyType()))
         {
            return false;
         }
         else if (link->GetPropertyType() == dtDAL::DataType::ACTOR)
         {
            std::string desiredClass;
            dtDAL::BaseActorObject* proxyValue = NULL;

            // Handle both: ActorIDActorProperty and ActorActorProperty (do nothing if redirected)
            if (IS_A(link->GetProperty(), dtDAL::ActorIDActorProperty*))
               desiredClass = static_cast<dtDAL::ActorIDActorProperty*>(link->GetProperty())->GetDesiredActorClass();
            else if (IS_A(value->GetProperty(), dtDAL::ActorActorProperty*))
               desiredClass = static_cast<dtDAL::ActorActorProperty*>(link->GetProperty())->GetDesiredActorClass();

            if (IS_A(value->GetProperty(), dtDAL::ActorIDActorProperty*) || IS_A(value->GetProperty(), dtDAL::ActorActorProperty*))
               proxyValue = value->GetActor(value->GetProperty()->GetName());

            if (proxyValue && !desiredClass.empty() && !proxyValue->IsInstanceOf(desiredClass))
               return false;
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Node::GetDefaultPropertyKey() const
   {
      return std::string("Director Node: ") + mType->GetFullName();
   }

   //////////////////////////////////////////////////////////////////////////
   const NodeType& Node::GetType()
   {
      return *mType;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetType(const NodeType& type)
   {
      mType = &type;
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
   const std::string& Node::GetTypeName()
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
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Node::IsEnabled()
   {
      return GetBoolean("Enabled");
   }

   //////////////////////////////////////////////////////////////////////////
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
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
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
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
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
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            propertyCount = mValues[valueIndex].GetPropertyCount();
         }
      }

      // Did not find any overrides, so return the default.
      if (propertyCount == 0 && dtDAL::PropertyContainer::GetProperty(name))
      {
         return 1;
      }

      return propertyCount;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* Node::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetProperty(index, outNode);
         }
      }

      // Did not find any overrides, so return the default.
      if (index == 0)
      {
         return dtDAL::PropertyContainer::GetProperty(name);
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& Node::GetPropertyType(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (prop)
      {
         return prop->GetDataType();
      }

      return dtDAL::DataType::UNKNOWN;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::LogValueRetrieved(ValueNode* valueNode, dtDAL::ActorProperty* prop)
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
   void Node::LogValueChanged(ValueNode* valueNode, dtDAL::ActorProperty* prop, const std::string& oldVal)
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
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (prop) return prop->ToString();
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 Node::GetVec2(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      dtDAL::Vec2ActorProperty* vecProp = dynamic_cast<dtDAL::Vec2ActorProperty*>(prop);
      if (vecProp)
      {
         return vecProp->GetValue();
      }

      return osg::Vec2();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Node::GetVec3(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      dtDAL::Vec3ActorProperty* vecProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);
      if (vecProp)
      {
         return vecProp->GetValue();
      }

      return osg::Vec3();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Node::GetVec4(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      dtDAL::Vec4ActorProperty* vecProp = dynamic_cast<dtDAL::Vec4ActorProperty*>(prop);
      if (vecProp)
      {
         return vecProp->GetValue();
      }

      return osg::Vec4();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId Node::GetActorID(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (prop) return dtCore::UniqueId(prop->ToString());

      dtCore::UniqueId emptyID;
      emptyID = "";
      return emptyID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::BaseActorObject* Node::GetActor(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (!prop) return NULL;

      dtDAL::ActorIDActorProperty* actorIdProp = dynamic_cast<dtDAL::ActorIDActorProperty*>(prop);
      if (actorIdProp)
      {
         dtDAL::BaseActorObject* proxy = NULL;

         dtCore::UniqueId id = actorIdProp->GetValue();

         if (GetDirector()->GetGameManager())
         {
            GetDirector()->GetGameManager()->FindActorById(id, proxy);
         }

         else //if (!proxy)
         {
            proxy = actorIdProp->GetActorProxy();
         }

         return proxy;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::GameEvent* Node::GetGameEvent(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (!prop) return NULL;

      dtDAL::GameEventActorProperty* eventProp = dynamic_cast<dtDAL::GameEventActorProperty*>(prop);
      if (eventProp)
      {
         return eventProp->GetValue();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor Node::GetResource(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (!prop) return dtDAL::ResourceDescriptor::NULL_RESOURCE;

      dtDAL::ResourceActorProperty* resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(prop);
      if (resourceProp)
      {
         return resourceProp->GetValue();
      }

      std::string resourceIdentifier = prop->ToString();
      if (!resourceIdentifier.empty())
      {
         return dtDAL::ResourceDescriptor(resourceIdentifier);
      }

      return dtDAL::ResourceDescriptor::NULL_RESOURCE;
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
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               prop->FromString(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
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
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            dtDAL::Vec2ActorProperty* vecProp = dynamic_cast<dtDAL::Vec2ActorProperty*>(prop);
            if (vecProp)
            {
               vecProp->SetValue(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         dtDAL::Vec2ActorProperty* vecProp = dynamic_cast<dtDAL::Vec2ActorProperty*>(prop);
         if (vecProp)
         {
            vecProp->SetValue(value);
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
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            dtDAL::Vec3ActorProperty* vecProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);
            if (vecProp)
            {
               vecProp->SetValue(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         dtDAL::Vec3ActorProperty* vecProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);
         if (vecProp)
         {
            vecProp->SetValue(value);
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
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            dtDAL::Vec4ActorProperty* vecProp = dynamic_cast<dtDAL::Vec4ActorProperty*>(prop);
            if (vecProp)
            {
               vecProp->SetValue(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         dtDAL::Vec4ActorProperty* vecProp = dynamic_cast<dtDAL::Vec4ActorProperty*>(prop);
         if (vecProp)
         {
            vecProp->SetValue(value);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetActorID(const dtCore::UniqueId& value, const std::string& name, int index)
   {
      SetString(value.ToString(), name, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetGameEvent(dtDAL::GameEvent* value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            dtDAL::GameEventActorProperty* eventProp = dynamic_cast<dtDAL::GameEventActorProperty*>(prop);
            if (eventProp)
            {
               eventProp->SetValue(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         dtDAL::GameEventActorProperty* eventProp = dynamic_cast<dtDAL::GameEventActorProperty*>(prop);
         if (eventProp)
         {
            eventProp->SetValue(value);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::SetResource(const dtDAL::ResourceDescriptor& value, const std::string& name, int index)
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
   ValueNode* Node::AsValueNode()
   {
      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
