#include "testplannerutils.h"
#include <dtAI/planner.h>
#include <cassert>
#include <algorithm>

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
* @author Bradley Anderegg 06/29/2006
*/

namespace dtAI
{


//////////////////////////////////////////////////////////////////////////
//MyNPC
//////////////////////////////////////////////////////////////////////////

   MyNPC::MyNPC()
   {
     
   }

   MyNPC::~MyNPC()
   {

   }

   void MyNPC::Init()
   {
      WorldState ws(WorldState::RemainingCostFunctor(this, &MyNPC::RemainingCost), WorldState::DesiredStateFunctor(this, &MyNPC::IsDesiredState));
      ws.AddState("Recipe", new Recipe());
      ws.AddState("Groceries", new Groceries());
      ws.AddState("PreparedFood", new PreparedFood());
      ws.AddState("HungerMeter", new HungerMeter());
   
      mHelper.SetCurrentState(ws);
      mHelper.AddOperator(new CallGrandma());
      mHelper.AddOperator(new GoToStore());
      mHelper.AddOperator(new Cook());
      mHelper.AddOperator(new Eat());
   }

   std::list<const NPCOperator*> MyNPC::GetPlanToEat()
   {
      Planner pPlanner;
      pPlanner.Reset(0, &mHelper);
      pPlanner.GeneratePlan();
      return pPlanner.GetConfig().mResult;
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

      assert(0);
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

      assert(0);
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

      assert(0);
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
      assert(0);
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

   PreparedFood::PreparedFood(): mHaveFood(0)
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

   CallGrandma::CallGrandma(): NPCOperator("CallGrandma", NPCOperator::ApplyOperatorFunctor(this, &CallGrandma::FuncCallGrandma))
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

   GoToStore::GoToStore(): NPCOperator("GoToStore", NPCOperator::ApplyOperatorFunctor(this, &GoToStore::FuncGoToStore))
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

   Cook::Cook(): NPCOperator("Cook", NPCOperator::ApplyOperatorFunctor(this, &Cook::FuncCook))
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

   Eat::Eat(): NPCOperator("Eat", NPCOperator::ApplyOperatorFunctor(this, &Eat::FuncEat))
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
      assert(0);
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
