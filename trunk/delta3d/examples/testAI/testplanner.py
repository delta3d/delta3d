from PyDtUtil import *
from PyDtCore import *
from PyDtAI import *

import sys


class RemainingCost(RemainingCostFunctor):
   
   def RemainingCost(self, ws):
      return 1


class DesiredState(DesiredStateFunctor):

   def IsDesiredState(self, ws):
      recipe = ws.GetState("Recipe")
      return recipe.HasRecipe()      


class Recipe(IStateVariable):

   def __init__(self):
      IStateVariable.__init__(self)
      self.mRecipe = 0

   def Copy(self):
      rec = Recipe()
      rec.HasRecipe(self.GetRecipe())
      return rec

   def HasRecipe(self, b):
      self.mRecipe = b

   def GetRecipe(self):
      return self.mRecipe
   

class HasRecipe(IConditional):

   def __init__(self):
      IConditional.__init__(self)      

   def GetName(self):
      return "Recipe"

   def Evaluate(self, ws):
      recipe = ws.GetState("Recipe")
      return recipe.GetRecipe()
      

class CallGrandma(ApplyOperatorFunctor):

   def Apply(self, wsFrom, wsTo):
      recipe = wsTo.GetState("Recipe")
      recipe.HasRecipe(1)
      wsTo.AddCost(1)

class PlannerNPC:

   def __init__(self):
      self.mHelper = PlannerHelper(RemainingCost(), DesiredState())

   def Init(self):
      self.mGrandmaFunctor = CallGrandma()
      self.mGrandmaOperator = NPCOperator("CallGrandma", self.mGrandmaFunctor)
      self.mHelper.AddOperator(self.mGrandmaOperator)

      self.mRecipe = Recipe()
      self.mHelper.GetCurrentState().AddState("Recipe", self.mRecipe)

   def GeneratePlan(self):
      pPlanner = Planner()
      pPlanner.Reset(self.mHelper)
      pPlanner.GeneratePlan()
      return pPlanner.GetPlan()
   
         
class TestPlanner:

   def Run(self) :
      pNPC = PlannerNPC()
      pNPC.Init()
      pNPC.GeneratePlan()
      plan = pNPC.GeneratePlan()
      #print plan[0]
       



plannerTest = TestPlanner()
plannerTest.Run()
