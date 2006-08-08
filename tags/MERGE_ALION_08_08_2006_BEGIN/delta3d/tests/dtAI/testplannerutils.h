/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
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
* @author Bradley Anderegg 06/29/2006
*/

#ifndef __DELTA_PLANNERTESTUTILS_H__
#define __DELTA_PLANNERTESTUTILS_H__

#include <dtAI/conditional.h>
#include <dtAI/statevariable.h>
#include <dtAI/npcoperator.h>
#include <dtAI/plannerhelper.h>

#include <list>

namespace dtAI
{

class MyNPC
{
  public:
  
     MyNPC();
     ~MyNPC();

     void Init();

     std::list<const NPCOperator*> GetPlanToEat();
     

     float RemainingCost(const WorldState* pWS) const;
     bool IsDesiredState(const WorldState* pWS) const;


  private:

     PlannerHelper mHelper;

};


class HasPizzaRecipe: public IConditional
{
  public:
     HasPizzaRecipe();
     ~HasPizzaRecipe();

     const std::string& GetName() const;
     bool Evaluate(const WorldState* pWS);

};


class HasPizzaGroceries: public IConditional
{
public:
   HasPizzaGroceries();
   ~HasPizzaGroceries();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);

};


class HasFood: public IConditional
{
public:
   HasFood();
   ~HasFood();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);

};

class AmHungry: public IConditional
{
public:
   AmHungry();
   ~AmHungry();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);

};

class Recipe: public IStateVariable
{
  public:

     Recipe();
     ~Recipe();

     IStateVariable* Copy() const;

     const std::string& GetName() const;
     const std::list<std::string>& GetRecipes() const;

     void AddRecipe(const std::string& pRecipe);

private:

   std::list<std::string> mRecipes;
};


class Groceries: public IStateVariable
{
public:

   Groceries();
   ~Groceries();

   IStateVariable* Copy() const;

   const std::string& GetName() const;
   const std::list<std::string>& GetGroceries() const;

   void AddItem(const std::string& pGrocery);
   void RemoveItem(const std::string& pGrocery);

private:

   std::list<std::string> mGroceries;

};


class PreparedFood: public IStateVariable
{
public:

   PreparedFood();
   ~PreparedFood();

   IStateVariable* Copy() const;

   const std::string& GetName() const;
   bool HaveFood() const;
   
   void SetHaveFood(bool b);

private:

   bool mHaveFood;

};


class HungerMeter: public IStateVariable
{
public:

   HungerMeter();
   ~HungerMeter();

   IStateVariable* Copy() const;

   const std::string& GetName() const;
   
   float GetHungerLevel() const;
   void SetHungerLevel(float pHunger);

private:

   float mHungerLevel;

};



class CallGrandma: public NPCOperator
{
public:

   CallGrandma();

   bool FuncCallGrandma(const WorldState* pCurrent, WorldState* pWSIn) const;

};

class GoToStore: public NPCOperator
{
public:

   GoToStore();

   bool FuncGoToStore(const WorldState* pCurrent, WorldState* pWSIn) const;

};


class Cook: public NPCOperator
{
public:

   Cook();

   bool FuncCook(const WorldState* pCurrent, WorldState* pWSIn) const;

};


class Eat: public NPCOperator
{
public:

   Eat();

   bool FuncEat(const WorldState* pCurrent, WorldState* pWSIn) const;

};


//utility functions
bool StringInList(const std::string& ing, const std::list<std::string>& pList);



}//namespace dtAI


#endif//__DELTA_PLANNERTESTUTILS_H__
