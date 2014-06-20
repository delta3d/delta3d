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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirector/nodetype.h>
#include <dtDirectorNodes/nodelibraryregistry.h>
#include <dtDirector/colors.h>

// Events
#include <dtDirectorNodes/gameeventmessageevent.h>
#include <dtDirectorNodes/gamemessageevent.h>
#include <dtDirectorNodes/inputnode.h>
#include <dtDirectorNodes/keypressevent.h>
#include <dtDirectorNodes/mousepressevent.h>
#include <dtDirectorNodes/startevent.h>
#include <dtDirectorNodes/remoteevent.h>
#include <dtDirectorNodes/triggervolumeevent.h>

// Actions
#include <dtDirectorNodes/attachcameraaction.h>
#include <dtDirectorNodes/attachmotionmodelaction.h>
#include <dtDirectorNodes/calculatedistanceaction.h>
#include <dtDirectorNodes/callremoteeventaction.h>
#include <dtDirectorNodes/changemapaction.h>
#include <dtDirectorNodes/compareactoraction.h>
#include <dtDirectorNodes/compareboolaction.h>
#include <dtDirectorNodes/comparevalueaction.h>
#include <dtDirectorNodes/comparevectoraction.h>
#include <dtDirectorNodes/compareactorpropertyaction.h>
#include <dtDirectorNodes/compareactorfacingaction.h>
#include <dtDirectorNodes/createfpsmotionmodelaction.h>
#include <dtDirectorNodes/createvectoraction.h>
#include <dtDirectorNodes/delayaction.h>
#include <dtDirectorNodes/deleteactoraction.h>
#include <dtDirectorNodes/foraction.h>
#include <dtDirectorNodes/foreachaction.h>
#include <dtDirectorNodes/foreachactoraction.h>
#include <dtDirectorNodes/fpsmotionmodellisteneraction.h>
#include <dtDirectorNodes/getactoraction.h>
#include <dtDirectorNodes/getactororientationaction.h>
#include <dtDirectorNodes/getactorpropertyaction.h>
#include <dtDirectorNodes/getapplicationconfigpropertyaction.h>
#include <dtDirectorNodes/getarrayaction.h>
#include <dtDirectorNodes/getarraysizeaction.h>
#include <dtDirectorNodes/getparentactoraction.h>
#include <dtDirectorNodes/getrandomvalueaction.h>
#include <dtDirectorNodes/getvectorvaluesaction.h>
#include <dtDirectorNodes/lerpactorrotationaction.h>
#include <dtDirectorNodes/lerpactorscaleaction.h>
#include <dtDirectorNodes/lerpactortranslationaction.h>
#include <dtDirectorNodes/logaction.h>
#include <dtDirectorNodes/loopaction.h>
#include <dtDirectorNodes/normalizevectoraction.h>
#include <dtDirectorNodes/operationaction.h>
#include <dtDirectorNodes/outputnode.h>
#include <dtDirectorNodes/quitapplicationaction.h>
#include <dtDirectorNodes/referencescriptaction.h>
#include <dtDirectorNodes/removearrayaction.h>
#include <dtDirectorNodes/removearrayaction.h>
#include <dtDirectorNodes/scheduleraction.h>
#include <dtDirectorNodes/sendeventmessageaction.h>
#include <dtDirectorNodes/sendmessageaction.h>
#include <dtDirectorNodes/setactorpropertyaction.h>
#include <dtDirectorNodes/setarrayaction.h>
#include <dtDirectorNodes/setvalueaction.h>
#include <dtDirectorNodes/spawnactoraction.h>
#include <dtDirectorNodes/spawnprefabaction.h>
#include <dtDirectorNodes/stringmanipulatoraction.h>
#include <dtDirectorNodes/stringcomposeraction.h>
#include <dtDirectorNodes/switchaction.h>
#include <dtDirectorNodes/toggleaction.h>
#include <dtDirectorNodes/togglemotionmodelaction.h>
#include <dtDirectorNodes/togglemousecursoraction.h>

// Mutators
#include <dtDirectorNodes/andmutator.h>
#include <dtDirectorNodes/addmutator.h>
#include <dtDirectorNodes/compareequalitymutator.h>
#include <dtDirectorNodes/comparelessmutator.h>
#include <dtDirectorNodes/comparegreatermutator.h>
#include <dtDirectorNodes/dividemutator.h>
#include <dtDirectorNodes/isnotmutator.h>
#include <dtDirectorNodes/multiplymutator.h>
#include <dtDirectorNodes/ormutator.h>
#include <dtDirectorNodes/subtractmutator.h>
#include <dtDirectorNodes/randommutator.h>

// Values
#include <dtDirectorNodes/actorarrayvalue.h>
#include <dtDirectorNodes/actorvalue.h>
#include <dtDirectorNodes/booleanvalue.h>
#include <dtDirectorNodes/doublevalue.h>
#include <dtDirectorNodes/externalvaluenode.h>
#include <dtDirectorNodes/floatvalue.h>
#include <dtDirectorNodes/intarrayvalue.h>
#include <dtDirectorNodes/intvalue.h>
#include <dtDirectorNodes/ownervalue.h>
#include <dtDirectorNodes/playervalue.h>
#include <dtDirectorNodes/randomfloatvalue.h>
#include <dtDirectorNodes/randomintvalue.h>
#include <dtDirectorNodes/referencevalue.h>
#include <dtDirectorNodes/staticmeshvalue.h>
#include <dtDirectorNodes/stringarrayvalue.h>
#include <dtDirectorNodes/stringvalue.h>
#include <dtDirectorNodes/vec2arrayvalue.h>
#include <dtDirectorNodes/vec2value.h>
#include <dtDirectorNodes/vec3arrayvalue.h>
#include <dtDirectorNodes/vec3value.h>
#include <dtDirectorNodes/vec4arrayvalue.h>
#include <dtDirectorNodes/vec4value.h>

// Misc
#include <dtDirector/groupnode.h>


using dtCore::RefPtr;

namespace dtDirector
{
   // Category naming convention:
   //  Core        - All Core nodes are nodes that are specifically referenced
   //                in Director and are special cases.
   //
   //  General     - General nodes provide general functionality that can be used
   //                in most, if not all, script types.
   //
   //  Value Ops   - Value Operation nodes are any nodes that perform an operation
   //                on values.
   //
   //  Conditional - Conditional nodes have multiple outputs that get triggered
   //                when a condition is met.
   //
   //  Cinematic   - Cinematic nodes are nodes that are auto-generated by the
   //                cinematic editor tool.

   // Events
   RefPtr<NodeType> NodeLibraryRegistry::START_EVENT_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Start Event",               "General",     "Base",        "An event that is triggered immediately when the script is first updated.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::REMOTE_EVENT_NODE_TYPE(                 new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Remote Event",              "Core",        "Base",        "A remote event.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::TRIGGER_VOLUME_EVENT_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Trigger Volume Event",      "General",     "Base",        "A trigger volume event.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::GAME_MESSAGE_EVENT_NODE_TYPE(           new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Game Message Event",        "Messages",    "Messages",    "An event that responds to game messages.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::GAME_EVENT_MESSAGE_EVENT_NODE_TYPE(     new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Game Event Message Event",  "Messages",    "Messages",    "An event that responds to game event messages.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::KEY_PRESS_EVENT_NODE_TYPE(              new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Key Press Event",           "Input",       "Input",       "An event that responds to keyboard input.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::MOUSE_PRESS_EVENT_NODE_TYPE(            new dtDirector::NodeType(dtDirector::NodeType::EVENT_NODE,  "Mouse Press Event",         "Input",       "Input",       "An event that responds to mouse input.", NULL, Colors::BEIGE));

   // Actions
   RefPtr<NodeType> NodeLibraryRegistry::LOG_ACTION_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Log Message",               "General",     "Base",        "Writes out a message to the log.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::CALL_REMOTE_EVENT_ACTION_NODE_TYPE(     new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Call Remote Event",         "Core",        "Base",        "Calls all remote event nodes with a given name in parallel, and waits until their entire chains are finished before continuing.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::DELAY_ACTION_NODE_TYPE(                 new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Delay",                     "General",     "Base",        "Performs a time delay.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::LOOP_ACTION_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Loop",                      "General",     "Base",        "Continually fires its 'Cycle' output after a given time period while active.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SWITCH_ACTION_NODE_TYPE(                new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Switch",                    "General",     "Base",        "Fires outputs in sequence based on the number of inputs received.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::FPS_MOTION_MODEL_LISTENER_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "FPS Motion Model Listener", "Scene",       "Base",        "Listens for state changes on an FPS motion model.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::GET_APP_CONFIG_PROPERTY_ACTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Application Config Property", "General", "Access", "Retrieves a config property value from an application's config file.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_ACTOR_ACTION_NODE_TYPE(              new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Actor",                       "General", "Access", "Retrieves the actor with the given name from the GameManager.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_ACTOR_PROPERTY_ACTION_NODE_TYPE(     new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Actor Property",              "General", "Access", "Retrieves the value of a property from an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_ACTOR_ORIENTATION_ACTION(            new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Actor Orientation",           "General", "Access", "Retrieves the orientation vectors of an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_PARENT_ACTOR_ACTION_NODE_TYPE(       new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Parent Actor",                "General", "Access", "Retrieves the parent of an actor with the given name from the GameManager.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_VECTOR_VALUES_ACTION_NODE_TYPE(      new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Vector Values",               "General", "Access", "Extracts the values in a vector to individual parts.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_ARRAY_VALUE_ACTION_NODE_TYPE(        new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Array Value",                 "General", "Access", "Retrieves the value of an array index.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_ARRAY_SIZE_ACTION_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Array Size",                  "General", "Access", "Retrieves the total size of an array.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::FOR_ACTION_NODE_TYPE(                    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "For",                             "General", "Access", "Iterates from one index to another activating a chain every loop.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::FOR_EACH_ACTION_NODE_TYPE(               new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "For Each",                        "General", "Access", "Iterates through a list of given items and triggers the \"Each Item\" output link for each item found.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::FOR_EACH_ACTOR_ACTION_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "For Each Actor",                  "General", "Access", "Retrieves all actors with optional class and name filters and iterates through them individually via the \"Each Actor\" output link.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::GET_RANDOM_VALUE_NODE_TYPE(              new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Get Random Value",                "General", "Access", "Gets a random value between Min and Max (to use when Min and Max are variable).", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::OPERATION_ACTION_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Arithmetic Operation",      "General",     "Operations",  "Performs a simple operation between two values A and B and outputs to Result.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::STRING_MANIPULATOR_ACTION_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "String Manipulator",        "General",     "Operations",  "Performs a string operation.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::STRING_COMPOSER_ACTION_NODE_TYPE(       new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "String Composer",           "General",     "Operations",  "Allows string to be composed by filling in '%%' character sequences with values from attached value nodes.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::TOGGLE_ACTION_NODE_TYPE(                new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Toggle",                    "General",     "Operations",  "Sets a boolean value.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::TOGGLE_MOTION_MODEL_ACTION_NODE_TYPE(   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Toggle Motion Model",       "Scene",       "Operations",  "Toggle the enabled status of a motion model.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SET_VALUE_ACTION_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Set Value",                 "General",     "Operations",  "Copies the value of the Source value into the Destination value.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SET_VECTOR_ACTION_NODE_TYPE(            new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Set Vector",                "General",     "Operations",  "Creates a vector from constituent parts.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SET_ACTOR_PROPERTY_ACTION_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Set Actor Property",        "General",     "Operations",  "Sets the value of a property from an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SET_ARRAY_VALUE_ACTION_NODE_TYPE(       new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Set Array Value",           "General",     "Operations",  "Copies a value into an index of an array value.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::REMOVE_ARRAY_VALUE_ACTION_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Remove Array Value",        "General",     "Operations",  "Removes an index from an array.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::NORMALIZE_VECTOR_ACTION_NODE_TYPE(      new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Normalize Vector",          "General",     "Operations",  "Normalizes a vector.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::CALCULATE_DISTANCE_ACTION_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Calculate Distance",        "General",     "Operations",  "Calculates the distance between two vectors.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SPAWN_ACTOR_ACTION_NODE_TYPE(           new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Spawn Actor",               "Scene",       "Operations",  "Spawn a new actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::DELETE_ACTOR_ACTION_NODE_TYPE(          new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Delete Actor",              "Scene",       "Operations",  "Delete an existing actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SPAWN_PREFAB_ACTION_NODE_TYPE(          new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Spawn Prefab",              "Scene",       "Operations",  "Spawn a prefab actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::CREATE_FPS_MOTION_MODEL_ACTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,"Create FPS Motion Model",   "Scene",       "Operations",  "Creates an FPS motion model.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::ATTACH_CAMERA_ACTION_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Attach Camera",             "Scene",       "Operations",  "Attach the applications camera to an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::ATTACH_MOTION_MODEL_ACTION_NODE_TYPE(   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Attach Motion Model",       "Scene",       "Operations",  "Attach a motion model to an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::CHANGE_MAP_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Change Map",                "Scene",       "Operations",  "Change the map set currently loaded by the GameManager.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::QUIT_APPLICATION_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Quit Application",          "Scene",       "Operations",  "Tells the application to quit.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_BOOL_ACTION_NODE_TYPE(          new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Bool",              "General",     "Condition",   "Compares two bool values A and B.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_VALUE_ACTION_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Value",             "General",     "Condition",   "Compares two numerical values A and B.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_VECTOR_ACTION_NODE_TYPE(        new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Vector",            "General",     "Condition",   "Compares two vectors A and B.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_ACTOR_ACTION_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Actors",            "General",     "Condition",   "Compares two actor IDs A and B.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_ACTOR_PROPERTY_ACTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Actor Property",    "General",     "Condition",   "Compares an actor property with a value.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_ACTOR_FACING_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Compare Actor Facing",      "General",     "Condition",   "Compares an actor facing direction in relation to a target.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::SCHEDULER_ACTION_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Scheduler",                 "Cinematic",   "Cinematic",   "Schedules a sequence of timed events.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::LERP_ACTOR_ROTATION_ACTION_NODE_TYPE(   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Lerp Actor Rotation",       "Cinematic",   "Cinematic",   "Linearly interpolates the rotation of an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::LERP_ACTOR_SCALE_ACTION_NODE_TYPE(      new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Lerp Actor Scale",          "Cinematic",   "Cinematic",   "Linearly interpolates the scale of an actor.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::LERP_ACTOR_TRANSLATION_ACTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Lerp Actor Translation",    "Cinematic",   "Cinematic",   "Linearly interpolates the position of an actor.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::SEND_MESSAGE_ACTION_NODE_TYPE(          new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Send Message Action",       "Messages",    "Messages",    "Sends a Game Message.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::SEND_EVENT_MESSAGE_ACTION_NODE_TYPE(    new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Send Event Message Action", "Messages",    "Messages",    "Sends a Game Event Message.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::TOGGLE_MOUSE_CURSOR_ACTION_NODE_TYPE(   new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE, "Toggle Mouse Cursor",       "GUI",         "GUI",         "Toggle the visibility of the applications mouse cursor.", NULL, Colors::BLUE));

   RefPtr<NodeType> NodeLibraryRegistry::REFERENCE_SCRIPT_ACTION_NODE_TYPE(      new dtDirector::NodeType(dtDirector::NodeType::MACRO_NODE,  "Reference Script",          "Core",        "Reference",   "References an external Director script resource.", NULL, Colors::GREEN2));

   // Mutators
   RefPtr<NodeType> NodeLibraryRegistry::ADD_MUTATOR_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Add",                      "Mutator",     "Operations",  "Performs an addition of two values.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::SUBTRACT_MUTATOR_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Subtract",                 "Mutator",     "Operations",  "Performs a subtraction between two values.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::MULTIPLY_MUTATOR_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Multiply",                 "Mutator",     "Operations",  "Performs a multiplication of two values.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::DIVIDE_MUTATOR_NODE_TYPE(               new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Divide",                   "Mutator",     "Operations",  "Performs a division between two values.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::RANDOM_MUTATOR_NODE_TYPE(               new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Random",                   "Mutator",     "Operations",  "Retrieves a random value between two values.", NULL, Colors::BLUE3));

   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_EQUALITY_MUTATOR_NODE_TYPE(     new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Is Equal",                 "Mutator",     "Condition",   "Tests if two values are equal to each other.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_LESS_MUTATOR_NODE_TYPE(         new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Less Than",                "Mutator",     "Condition",   "Tests if A is less than B.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::COMPARE_GREATER_MUTATOR_NODE_TYPE(      new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Greater Than",             "Mutator",     "Condition",   "Tests if A is greater than B.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::IS_NOT_MUTATOR_NODE_TYPE(               new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Is Not",                   "Mutator",     "Condition",   "Flips the value received.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::AND_MUTATOR_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "And",                      "Mutator",     "Condition",   "Tests if all received values are true.", NULL, Colors::BLUE3));
   RefPtr<NodeType> NodeLibraryRegistry::OR_MUTATOR_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::MUTATOR_NODE, "Or",                       "Mutator",     "Condition",   "Tests if at least one received value is true.", NULL, Colors::BLUE3));

   // Values
   RefPtr<NodeType> NodeLibraryRegistry::REFERENCE_VALUE_NODE_TYPE(              new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Reference",                 "Core",        "Base",        "This will reference another value in the script.", NULL, Colors::VIOLET));
   RefPtr<NodeType> NodeLibraryRegistry::PLAYER_VALUE_NODE_TYPE(                 new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Player",                    "Core",        "Actors",      "The player actor.", NULL, Colors::MANGENTA2));
   RefPtr<NodeType> NodeLibraryRegistry::OWNER_VALUE_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Owner",                     "Core",        "Actors",      "The owner actor of this script.", NULL, Colors::MANGENTA2));
   RefPtr<NodeType> NodeLibraryRegistry::BOOLEAN_VALUE_NODE_TYPE(                new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Boolean",                   "General",     "Base",        "A boolean value.", NULL, Colors::RED, dtCore::DataType::BOOLEAN));
   RefPtr<NodeType> NodeLibraryRegistry::INT_VALUE_NODE_TYPE(                    new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Int",                       "General",     "Base",        "An integer value.", NULL, Colors::BLUE, dtCore::DataType::INT));
   RefPtr<NodeType> NodeLibraryRegistry::FLOAT_VALUE_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Float",                     "General",     "Base",        "A float value.", NULL, Colors::YELLOW, dtCore::DataType::FLOAT));
   RefPtr<NodeType> NodeLibraryRegistry::DOUBLE_VALUE_NODE_TYPE(                 new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Double",                    "General",     "Base",        "A double value.", NULL, Colors::ORANGE, dtCore::DataType::DOUBLE));
   RefPtr<NodeType> NodeLibraryRegistry::STRING_VALUE_NODE_TYPE(                 new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "String",                    "General",     "Base",        "A string value.", NULL, Colors::CYAN, dtCore::DataType::STRING));
   RefPtr<NodeType> NodeLibraryRegistry::ACTOR_VALUE_NODE_TYPE(                  new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Actor",                     "General",     "Actors",      "An actor value.", NULL, Colors::MANGENTA, dtCore::DataType::ACTOR));
   RefPtr<NodeType> NodeLibraryRegistry::STATIC_MESH_VALUE_NODE_TYPE(            new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Static Mesh",               "General",     "Resources",   "A Static Mesh resource value.", NULL, Colors::MANGENTA2, dtCore::DataType::STATIC_MESH));
   RefPtr<NodeType> NodeLibraryRegistry::VEC2_VALUE_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec2",                      "General",     "Base",        "An x,y vector value.", NULL, Colors::MANGENTA, dtCore::DataType::VEC2));
   RefPtr<NodeType> NodeLibraryRegistry::VEC3_VALUE_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec3",                      "General",     "Base",        "An x,y,z vector value.", NULL, Colors::MANGENTA, dtCore::DataType::VEC3));
   RefPtr<NodeType> NodeLibraryRegistry::VEC4_VALUE_NODE_TYPE(                   new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec4",                      "General",     "Base",        "An x,y,z,w vector value.", NULL, Colors::MANGENTA, dtCore::DataType::VEC4));

   RefPtr<NodeType> NodeLibraryRegistry::ACTOR_ARRAY_VALUE_NODE_TYPE(            new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Actor Array",               "General",     "Arrays",      "An array of actor values.", NULL, Colors::MANGENTA));
   RefPtr<NodeType> NodeLibraryRegistry::INT_ARRAY_VALUE_NODE_TYPE(              new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Int Array",                 "General",     "Arrays",      "An array of int values.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::STRING_ARRAY_VALUE_NODE_TYPE(           new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "String Array",              "General",     "Arrays",      "An array of string values.", NULL, Colors::CYAN));
   RefPtr<NodeType> NodeLibraryRegistry::VEC2_ARRAY_VALUE_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec2 Array",                "General",     "Arrays",      "An array of vec2 values.", NULL, Colors::MANGENTA));
   RefPtr<NodeType> NodeLibraryRegistry::VEC3_ARRAY_VALUE_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec3 Array",                "General",     "Arrays",      "An array of vec3 values.", NULL, Colors::MANGENTA));
   RefPtr<NodeType> NodeLibraryRegistry::VEC4_ARRAY_VALUE_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Vec4 Array",                "General",     "Arrays",      "An array of vec4 values.", NULL, Colors::MANGENTA));

   RefPtr<NodeType> NodeLibraryRegistry::RANDOM_INT_VALUE_NODE_TYPE(             new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Random Int",                "General",     "Random",      "A random int value.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::RANDOM_FLOAT_VALUE_NODE_TYPE(           new dtDirector::NodeType(dtDirector::NodeType::VALUE_NODE,  "Random Float",              "General",     "Random",      "A random float value.", NULL, Colors::YELLOW));

   // Links
   RefPtr<NodeType> NodeLibraryRegistry::INPUT_NODE_TYPE(                        new dtDirector::NodeType(dtDirector::NodeType::LINK_NODE,   "Input Link",                "Core",        "Base",        "This node creates an input link connector in its parent graph.", NULL, Colors::BEIGE));
   RefPtr<NodeType> NodeLibraryRegistry::OUTPUT_NODE_TYPE(                       new dtDirector::NodeType(dtDirector::NodeType::LINK_NODE,   "Output Link",               "Core",        "Base",        "This node creates an output link connector in its parent graph.", NULL, Colors::BLUE));
   RefPtr<NodeType> NodeLibraryRegistry::EXTERNAL_VALUE_NODE_TYPE(               new dtDirector::NodeType(dtDirector::NodeType::LINK_NODE,   "Value Link",                "Core",        "Base",        "This node creates a value link connector in its parent graph.", NULL, Colors::VIOLET));

   // Misc
   RefPtr<NodeType> NodeLibraryRegistry::GROUP_BOX_NODE_TYPE(                    new dtDirector::NodeType(dtDirector::NodeType::MISC_NODE,   "Group Box",                 "Core",        "Base",        "A group frame to help organize nodes together.", NULL, Colors::GRAY3));


   //////////////////////////////////////////////////////////////////////////
   extern "C" NODE_LIBRARY_EXPORT dtDirector::NodePluginRegistry* CreateNodePluginRegistry()
   {
      return new NodeLibraryRegistry;
   }

   //////////////////////////////////////////////////////////////////////////
   extern "C" NODE_LIBRARY_EXPORT void DestroyNodePluginRegistry(dtDirector::NodePluginRegistry* registry)
   {
      delete registry;
   }

   //////////////////////////////////////////////////////////////////////////
   NodeLibraryRegistry::NodeLibraryRegistry()
      : dtDirector::NodePluginRegistry("dtDirectorNodes", "Core set of nodes that can be used anywhere.")
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeLibraryRegistry::RegisterNodeTypes()
   {
      // Events
      mNodeFactory->RegisterType<StartEvent>(START_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<RemoteEvent>(REMOTE_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<TriggerVolumeEvent>(TRIGGER_VOLUME_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<GameMessageEvent>(GAME_MESSAGE_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<GameEventMessageEvent>(GAME_EVENT_MESSAGE_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<KeyPressEvent>(KEY_PRESS_EVENT_NODE_TYPE.get());
      mNodeFactory->RegisterType<MousePressEvent>(MOUSE_PRESS_EVENT_NODE_TYPE.get());

      // Actions
      mNodeFactory->RegisterType<LogAction>(LOG_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CallRemoteEventAction>(CALL_REMOTE_EVENT_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<DelayAction>(DELAY_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<LoopAction>(LOOP_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SwitchAction>(SWITCH_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<FPSMotionModelListenerAction>(FPS_MOTION_MODEL_LISTENER_NODE_TYPE.get());

      mNodeFactory->RegisterType<GetApplicationConfigPropertyAction>(GET_APP_CONFIG_PROPERTY_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetActorAction>(GET_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetActorPropertyAction>(GET_ACTOR_PROPERTY_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetActorOrientationAction>(GET_ACTOR_ORIENTATION_ACTION.get());
      mNodeFactory->RegisterType<GetParentActorAction>(GET_PARENT_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetVectorValuesAction>(GET_VECTOR_VALUES_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetArrayAction>(GET_ARRAY_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetArraySizeAction>(GET_ARRAY_SIZE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ForAction>(FOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ForEachAction>(FOR_EACH_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ForEachActorAction>(FOR_EACH_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<GetRandomValueAction>(GET_RANDOM_VALUE_NODE_TYPE.get());

      mNodeFactory->RegisterType<OperationAction>(OPERATION_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<StringManipulatorAction>(STRING_MANIPULATOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<StringComposerAction>(STRING_COMPOSER_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ToggleAction>(TOGGLE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ToggleMotionModelAction>(TOGGLE_MOTION_MODEL_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SetValueAction>(SET_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CreateVectorAction>(SET_VECTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SetActorPropertyAction>(SET_ACTOR_PROPERTY_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SetArrayAction>(SET_ARRAY_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<RemoveArrayAction>(REMOVE_ARRAY_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<NormalizeVectorAction>(NORMALIZE_VECTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CalculateDistanceAction>(CALCULATE_DISTANCE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SpawnActorAction>(SPAWN_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<DeleteActorAction>(DELETE_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SpawnPrefabAction>(SPAWN_PREFAB_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CreateFPSMotionModelAction>(CREATE_FPS_MOTION_MODEL_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<AttachCameraAction>(ATTACH_CAMERA_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<AttachMotionModelAction>(ATTACH_MOTION_MODEL_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<ChangeMapAction>(CHANGE_MAP_NODE_TYPE.get());
      mNodeFactory->RegisterType<QuitApplicationAction>(QUIT_APPLICATION_NODE_TYPE.get());

      mNodeFactory->RegisterType<CompareBoolAction>(COMPARE_BOOL_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareValueAction>(COMPARE_VALUE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareVectorAction>(COMPARE_VECTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareActorAction>(COMPARE_ACTOR_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareActorPropertyAction>(COMPARE_ACTOR_PROPERTY_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareActorFacingAction>(COMPARE_ACTOR_FACING_NODE_TYPE.get());

      mNodeFactory->RegisterType<SchedulerAction>(SCHEDULER_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<LerpActorRotationAction>(LERP_ACTOR_ROTATION_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<LerpActorScaleAction>(LERP_ACTOR_SCALE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<LerpActorTranslationAction>(LERP_ACTOR_TRANSLATION_ACTION_NODE_TYPE.get());

      mNodeFactory->RegisterType<SendMessageAction>(SEND_MESSAGE_ACTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SendEventMessageAction>(SEND_EVENT_MESSAGE_ACTION_NODE_TYPE.get());

      mNodeFactory->RegisterType<ToggleMouseCursorAction>(TOGGLE_MOUSE_CURSOR_ACTION_NODE_TYPE.get());

      mNodeFactory->RegisterType<ReferenceScriptAction>(REFERENCE_SCRIPT_ACTION_NODE_TYPE.get());

      // Mutators
      mNodeFactory->RegisterType<AddMutator>(ADD_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<SubtractMutator>(SUBTRACT_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<MultiplyMutator>(MULTIPLY_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<DivideMutator>(DIVIDE_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<RandomMutator>(RANDOM_MUTATOR_NODE_TYPE.get());

      mNodeFactory->RegisterType<CompareEqualityMutator>(COMPARE_EQUALITY_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareLessMutator>(COMPARE_LESS_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<CompareGreaterMutator>(COMPARE_GREATER_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<IsNotMutator>(IS_NOT_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<AndMutator>(AND_MUTATOR_NODE_TYPE.get());
      mNodeFactory->RegisterType<OrMutator>(OR_MUTATOR_NODE_TYPE.get());

      // Values
      mNodeFactory->RegisterType<ReferenceValue>(REFERENCE_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<PlayerValue>(PLAYER_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<OwnerValue>(OWNER_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<BooleanValue>(BOOLEAN_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<IntValue>(INT_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<FloatValue>(FLOAT_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<DoubleValue>(DOUBLE_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<StringValue>(STRING_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<ActorValue>(ACTOR_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<StaticMeshValue>(STATIC_MESH_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec2Value>(VEC2_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec3Value>(VEC3_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec4Value>(VEC4_VALUE_NODE_TYPE.get());

      mNodeFactory->RegisterType<ActorArrayValue>(ACTOR_ARRAY_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<IntArrayValue>(INT_ARRAY_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<StringArrayValue>(STRING_ARRAY_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec2ArrayValue>(VEC2_ARRAY_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec3ArrayValue>(VEC3_ARRAY_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<Vec4ArrayValue>(VEC4_ARRAY_VALUE_NODE_TYPE.get());

      mNodeFactory->RegisterType<RandomIntValue>(RANDOM_INT_VALUE_NODE_TYPE.get());
      mNodeFactory->RegisterType<RandomFloatValue>(RANDOM_FLOAT_VALUE_NODE_TYPE.get());

      // Links
      mNodeFactory->RegisterType<InputNode>(INPUT_NODE_TYPE.get());
      mNodeFactory->RegisterType<OutputNode>(OUTPUT_NODE_TYPE.get());
      mNodeFactory->RegisterType<ExternalValueNode>(EXTERNAL_VALUE_NODE_TYPE.get());

      // Misc
      mNodeFactory->RegisterType<GroupNode>(GROUP_BOX_NODE_TYPE.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeLibraryRegistry::GetReplacementNodeTypes(NodeLibraryRegistry::NodeTypeReplacements &replacements) const
   {
      {
         NodeLibraryRegistry::NodeReplacementData nodeData;
         nodeData.library = "dtDirectorAnimNodes";
         nodeData.newName = "Animate Actor";
         nodeData.newCategory = "Cinematic";

         replacements.push_back(std::make_pair("Cinematic.Animate Actor", nodeData));
      }

      {
         NodeLibraryRegistry::NodeReplacementData nodeData;
         nodeData.library = "dtDirectorNodes";
         nodeData.newName = "Arithmetic Operation";
         nodeData.newCategory = "General";

         replacements.push_back(std::make_pair("General.Binary Operation", nodeData));
      }

      {
         NodeLibraryRegistry::NodeReplacementData nodeData;
         nodeData.library = "dtDirectorNodes";
         nodeData.newName = "Vec3";
         nodeData.newCategory = "General";

         replacements.push_back(std::make_pair("General.Vector", nodeData));
      }
   }
}
