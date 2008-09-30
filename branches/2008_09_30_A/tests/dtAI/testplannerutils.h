/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2004-2008, MOVES Institute
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
* @author Bradley Anderegg 06/29/2006
*/

#ifndef __DELTA_PLANNERTESTUTILS_H__
#define __DELTA_PLANNERTESTUTILS_H__

#include <dtAI/conditional.h>
#include <dtAI/statevariable.h>
#include <dtAI/operator.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/basenpc.h>
#include <list>

namespace dtAI
{

class MyNPC: public BaseNPC
{
  public:
  
     MyNPC();
     ~MyNPC();

     void OnInit();

     void MakeHungry();
     
     void RegisterActions();

     bool ActionCallGrandma(double dt, WorldState* pWS);
     bool ActionGoToStore(double dt, WorldState* pWS);
     bool ActionCook(double dt, WorldState* pWS);
     bool ActionEat(double dt, WorldState* pWS);

     float RemainingCost(const WorldState* pWS) const;
     bool IsDesiredState(const WorldState* pWS) const;


};


class HasPizzaRecipe: public IConditional
{
  public:
     HasPizzaRecipe();

     const std::string& GetName() const;
     bool Evaluate(const WorldState* pWS);
  protected:
     ~HasPizzaRecipe();
};


class HasPizzaGroceries: public IConditional
{
public:
   HasPizzaGroceries();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);
protected:
   ~HasPizzaGroceries();
};


class HasFood: public IConditional
{
public:
   HasFood();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);

protected:
   ~HasFood();
};

class AmHungry: public IConditional
{
public:
   AmHungry();

   const std::string& GetName() const;
   bool Evaluate(const WorldState* pWS);

protected:
   ~AmHungry();
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
     
     virtual const std::string ToString() const { return GetName(); }

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

   virtual const std::string ToString() const { return GetName(); }

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

   virtual const std::string ToString() const { return GetName(); }

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

   virtual const std::string ToString() const { return GetName(); }

private:

   float mHungerLevel;

};



class CallGrandma: public Operator
{
public:

   CallGrandma();

   bool FuncCallGrandma(const Operator*, WorldState* pWSIn) const;

};

class GoToStore: public Operator
{
public:

   GoToStore();

   bool FuncGoToStore(const Operator*, WorldState* pWSIn) const;

};


class Cook: public Operator
{
public:

   Cook();

   bool FuncCook(const Operator*, WorldState* pWSIn) const;

};


class Eat: public Operator
{
public:

   Eat();

   bool FuncEat(const Operator*, WorldState* pWSIn) const;

};


//utility functions
bool StringInList(const std::string& ing, const std::list<std::string>& pList);



}//namespace dtAI


#endif//__DELTA_PLANNERTESTUTILS_H__
