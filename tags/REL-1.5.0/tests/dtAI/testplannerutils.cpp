/* Delta3D Open Source Game and Simulation Engine
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
 * Bradley Anderegg 06/29/2006
 */
#include <prefix/dtgameprefix-src.h>
#include "testplannerutils.h"
#include <dtAI/planner.h>
#include <cassert>
#include <iostream>
#include <algorithm>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #pragma warning(disable : 4355) // 'this' used in initializer list
#endif

namespace dtAI
{


//////////////////////////////////////////////////////////////////////////
//MyNPC
//////////////////////////////////////////////////////////////////////////

   MyNPC::MyNPC()
     : BaseNPC("MyNPC")
   {
     
   }

   MyNPC::~MyNPC()
   {

   }

   void MyNPC::OnInit()
   {

      mHelper.SetDesiredStateFunc(PlannerHelper::DesiredStateFunctor(this, &MyNPC::IsDesiredState));
      mHelper.SetRemainingCostFunc(PlannerHelper::RemainingCostFunctor(this, &MyNPC::RemainingCost));

      AddOperator(new CallGrandma());
      AddOperator(new GoToStore());
      AddOperator(new Cook());
      AddOperator(new Eat());

      
      mWSTemplate.AddState("Recipe", new Recipe());
      mWSTemplate.AddState("Groceries", new Groceries());
      mWSTemplate.AddState("PreparedFood", new PreparedFood());
      mWSTemplate.AddState("HungerMeter", new HungerMeter());  
   }

   void MyNPC::MakeHungry()
   {
      IStateVariable* pState = mHelper.GetCurrentState()->GetState("HungerMeter");
      if(pState)
      {
         HungerMeter* hm = dynamic_cast<HungerMeter*>(pState);
         assert(hm);
         hm->SetHungerLevel(0.0f);
      }
      
      //std::cout << "Force Hungry" << std::endl;
   }

   float MyNPC::RemainingCost(const WorldState* pWS) const
   {
      static HasPizzaRecipe pHPR;
      static HasPizzaGroceries pHPG;
      static HasFood pHF;
      static AmHungry pAH;
      
      if(!pAH.Evaluate(pWS))
      {
         return 0;
      }

      if(pHF.Evaluate(pWS))
      {
         return 1;
      }

      if(pHPG.Evaluate(pWS))
      {
         return 2;
      }

      if(pHPR.Evaluate(pWS))
      {
         return 3;
      }         

      return 4;
   }

   bool MyNPC::IsDesiredState(const WorldState* pWS) const
   {
      const IStateVariable* pState = pWS->GetState("HungerMeter");
      if(pState)
      {
         const HungerMeter* hm = dynamic_cast<const HungerMeter*>(pState);
         assert(hm);
         bool pHungry = (hm->GetHungerLevel() > 0.75);
         return pHungry;
      }
      return false;
   }

   void MyNPC::RegisterActions()
   {
      RegisterAction("CallGrandma", BaseNPC::Action(this, &MyNPC::ActionCallGrandma));
      RegisterAction("GoToStore", BaseNPC::Action(this, &MyNPC::ActionGoToStore));
      RegisterAction("Cook", BaseNPC::Action(this, &MyNPC::ActionCook));
      RegisterAction("Eat", BaseNPC::Action(this, &MyNPC::ActionEat));
   }

   bool MyNPC::ActionCallGrandma(double dt, WorldState* pWS)
   {
      mHelper.GetOperator("CallGrandma")->Apply(pWS, pWS);
      //std::cout << "Action Call Grandma" << std::endl;
      return true;
   }

   bool MyNPC::ActionGoToStore(double dt, WorldState* pWS)
   {
      mHelper.GetOperator("GoToStore")->Apply(pWS, pWS);
      //std::cout << "Action Go To Store" << std::endl;
      return true;
   }

   bool MyNPC::ActionCook(double dt, WorldState* pWS)
   {
      mHelper.GetOperator("Cook")->Apply(pWS, pWS);
      //std::cout << "Action Cook" << std::endl;
      return true;
   }

   bool MyNPC::ActionEat(double dt, WorldState* pWS)
   {
      mHelper.GetOperator("Eat")->Apply(pWS, pWS);
      //std::cout << "Action Eat" << std::endl;
      return true;
   }


//////////////////////////////////////////////////////////////////////////
//HasPizzaRecipe
//////////////////////////////////////////////////////////////////////////

   HasPizzaRecipe::HasPizzaRecipe()
   {

   }

   HasPizzaRecipe::~HasPizzaRecipe()
   {

   }

   const std::string& HasPizzaRecipe::GetName() const
   {
      static std::string name("HasPizzaRecipe");
      return name;
   }

   bool HasPizzaRecipe::Evaluate(const WorldState* pWS)
   {
      const IStateVariable* pState = pWS->GetState("Recipe");
      if(pState)
      {
         const Recipe* rec = dynamic_cast<const Recipe*>(pState);
         assert(rec);
         return StringInList("Pizza", rec->GetRecipes());
      }

      assert(false);
      return false;
   }


//////////////////////////////////////////////////////////////////////////
//HasPizzaGroceries
//////////////////////////////////////////////////////////////////////////
   HasPizzaGroceries::HasPizzaGroceries()
   {

   }

   HasPizzaGroceries::~HasPizzaGroceries()
   {

   }

   const std::string& HasPizzaGroceries::GetName() const
   {
      static std::string name("HasPizzaGroceries");
      return name;
   }

   bool HasPizzaGroceries::Evaluate(const WorldState* pWS)
   {
      const IStateVariable* pState = pWS->GetState("Groceries");
      if(pState)
      {
         const Groceries* groc = dynamic_cast<const Groceries*>(pState);
         assert(groc);
         std::list<std::string> pGroc = groc->GetGroceries();
         
         return (StringInList("Dough", pGroc) && StringInList("Cheese", pGroc) && StringInList("Tomato Sauce", pGroc));
      }

      assert(false);
      return false;
   }


//////////////////////////////////////////////////////////////////////////
//HasFood
//////////////////////////////////////////////////////////////////////////

   HasFood::HasFood()
   {

   }

   HasFood::~HasFood()
   {
   
   }

   const std::string& HasFood::GetName() const
   {
      static std::string name("HasFood");
      return name;
   }

   bool HasFood::Evaluate(const WorldState* pWS)
   {
      const IStateVariable* pState = pWS->GetState("PreparedFood");
      if(pState)
      {
         const PreparedFood* pf = dynamic_cast<const PreparedFood*>(pState);
         assert(pf);
         return pf->HaveFood();
      }

      assert(false);
      return false;
   }


//////////////////////////////////////////////////////////////////////////
//AmHungry
//////////////////////////////////////////////////////////////////////////
   
   AmHungry::AmHungry()
   {

   }

   AmHungry::~AmHungry()
   {

   }

   const std::string& AmHungry::GetName() const
   {
      static std::string name("AmHungry");
      return name;
   }

   bool AmHungry::Evaluate(const WorldState* pWS)
   {
      const IStateVariable* pState = pWS->GetState("HungerMeter");
      if(pState)
      {
         const HungerMeter* hm = dynamic_cast<const HungerMeter*>(pState);
         assert(hm);
         return (hm->GetHungerLevel() < 0.5);
      }
      //we shouldnt get here
      assert(false);
      return false;
   }


//////////////////////////////////////////////////////////////////////////
//Recipe
//////////////////////////////////////////////////////////////////////////

   Recipe::Recipe(): mRecipes()
   {

   }

   Recipe::~Recipe()
   {

   }

   IStateVariable* Recipe::Copy() const
   {
      Recipe* pRec = new Recipe();

      std::list<std::string>::const_iterator iter = mRecipes.begin();
      std::list<std::string>::const_iterator endOfList = mRecipes.end();
      while(iter != endOfList)
      {
         pRec->AddRecipe(*iter);   
         ++iter;
      }
      return pRec;
   }


   const std::string& Recipe::GetName() const
   {
      static std::string name("Recipe");
      return name;
   }

   const std::list<std::string>& Recipe::GetRecipes() const
   {
      return mRecipes;
   }

   void Recipe::AddRecipe(const std::string& pRecipe)
   {
      mRecipes.push_back(pRecipe);
   }


//////////////////////////////////////////////////////////////////////////
//Groceries
//////////////////////////////////////////////////////////////////////////

   Groceries::Groceries(): mGroceries()
   {

   }

   Groceries::~Groceries()
   {
      mGroceries.clear();
   }

   IStateVariable* Groceries::Copy() const
   {
      Groceries* pGroc = new Groceries();
      
      std::list<std::string>::const_iterator iter = mGroceries.begin();
      std::list<std::string>::const_iterator endOfList = mGroceries.end();
      while(iter != endOfList)
      {
         pGroc->AddItem(*iter);   
         ++iter;
      }
      return pGroc;
   }

   const std::string& Groceries::GetName() const
   {
      static std::string name("Groceries");
      return name;
   }

   const std::list<std::string>& Groceries::GetGroceries() const
   {
      return mGroceries;
   }

   void Groceries::AddItem(const std::string& pGrocery)
   {
      mGroceries.push_back(pGrocery);
   }

   void Groceries::RemoveItem(const std::string& pGrocery)
   {
      mGroceries.remove(pGrocery);
   }


///////////////////////////////////////////////////////////////////////////
//PreparedFood
//////////////////////////////////////////////////////////////////////////

   PreparedFood::PreparedFood(): mHaveFood(false)
   {

   }

   PreparedFood::~PreparedFood()
   {

   }

   IStateVariable* PreparedFood::Copy() const
   {
      PreparedFood* pf = new PreparedFood();
      pf->SetHaveFood(mHaveFood);
      return pf;
   }

   const std::string& PreparedFood::GetName() const
   {
      static std::string name("PreparedFood");
      return name;
   }

   bool PreparedFood::HaveFood() const
   {
      return mHaveFood;
   }

   void PreparedFood::SetHaveFood(bool b)
   {
      mHaveFood = b;
   }


//////////////////////////////////////////////////////////////////////////
//HungerMeter
//////////////////////////////////////////////////////////////////////////

   HungerMeter::HungerMeter(): mHungerLevel(0.0f)
   {

   }

   HungerMeter::~HungerMeter()
   {

   }

   IStateVariable* HungerMeter::Copy() const
   {
      HungerMeter* pNewHM = new HungerMeter();
      pNewHM->SetHungerLevel(mHungerLevel);
      return pNewHM;
   }

   const std::string& HungerMeter::GetName() const
   {
      static std::string name("HungerMeter");
      return name;
   }

   float HungerMeter::GetHungerLevel() const
   {
      return mHungerLevel;
   }

   void HungerMeter::SetHungerLevel(float pHunger)
   {
      mHungerLevel = pHunger;
   }

//////////////////////////////////////////////////////////////////////////
//CallGrandma
//////////////////////////////////////////////////////////////////////////

   CallGrandma::CallGrandma(): Operator("CallGrandma", Operator::ApplyOperatorFunctor(this, &CallGrandma::FuncCallGrandma))
   {      
   }

   bool CallGrandma::FuncCallGrandma(const WorldState* pCurrent, WorldState* pWSIn) const
   { 
      IStateVariable* pStateRecipe = pWSIn->GetState("Recipe");
      assert(pStateRecipe);
      Recipe* pRecipe = dynamic_cast<Recipe*>(pStateRecipe);
      assert(pRecipe);
      pRecipe->AddRecipe("Pizza");
      pWSIn->AddCost(1.0f);
      return true;
   }
//////////////////////////////////////////////////////////////////////////
//GoToStore
//////////////////////////////////////////////////////////////////////////

   GoToStore::GoToStore(): Operator("GoToStore", Operator::ApplyOperatorFunctor(this, &GoToStore::FuncGoToStore))
   {
      AddPreCondition(new HasPizzaRecipe());
   }

   bool GoToStore::FuncGoToStore(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      IStateVariable* pStateGroc = pWSIn->GetState("Groceries");
      assert(pStateGroc);
      Groceries* pGroc = dynamic_cast<Groceries*>(pStateGroc);
      assert(pGroc);

      pGroc->AddItem("Dough");
      pGroc->AddItem("Cheese");
      pGroc->AddItem("Tomato Sauce");

      pWSIn->AddCost(1.0f);
      return true;
   }


//////////////////////////////////////////////////////////////////////////
//Cook
//////////////////////////////////////////////////////////////////////////

   Cook::Cook(): Operator("Cook", Operator::ApplyOperatorFunctor(this, &Cook::FuncCook))
   {
      AddPreCondition(new HasPizzaRecipe());
      AddPreCondition(new HasPizzaGroceries());
   }

   bool Cook::FuncCook(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      IStateVariable* pStateGrco = pWSIn->GetState("Groceries");
      IStateVariable* pStatePFood = pWSIn->GetState("PreparedFood");
      assert(pStateGrco && pStatePFood);

      Groceries* pGroc = dynamic_cast<Groceries*>(pStateGrco);
      PreparedFood* pFood = dynamic_cast<PreparedFood*>(pStatePFood);
      assert(pGroc && pFood);

      std::list<std::string> pGroceryList = pGroc->GetGroceries();
      
      if(StringInList("Dough", pGroceryList) && StringInList("Cheese", pGroceryList) && StringInList("Tomato Sauce", pGroceryList))
      {
         pGroc->RemoveItem("Dough");
         pGroc->RemoveItem("Cheese");
         pGroc->RemoveItem("Tomato Sauce");

         pFood->SetHaveFood(true);
      }

      pWSIn->AddCost(1.0f);

      return false;
   }

//////////////////////////////////////////////////////////////////////////
//Eat
//////////////////////////////////////////////////////////////////////////

   Eat::Eat(): Operator("Eat", Operator::ApplyOperatorFunctor(this, &Eat::FuncEat))
   {
      AddPreCondition(new HasFood());
   }

   bool Eat::FuncEat(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      IStateVariable* pState = pWSIn->GetState("PreparedFood");
      IStateVariable* pHungerState = pWSIn->GetState("HungerMeter");      

      assert(pState && pHungerState);

      PreparedFood* pf = dynamic_cast<PreparedFood*>(pState);
      assert(pf);

      HungerMeter* hm = dynamic_cast<HungerMeter*>(pHungerState);
      assert(hm);


      if(!pf->HaveFood())
      {
         return false;
      }
      else
      {
         pf->SetHaveFood(false);            
         hm->SetHungerLevel(1.0f);
         pWSIn->AddCost(1.0f);
         return true;
      }
      assert(false);
      return false;
   }


//////////////////////////////////////////////////////////////////////////
//Utility Functions
//////////////////////////////////////////////////////////////////////////
   bool StringInList(const std::string& ing, const std::list<std::string>& pList) 
   {
      return std::find(pList.begin(), pList.end(), ing) != pList.end();
   }


//////////////////////////////////////////////////////////////////////////
}//namespace dtAI