from PyDtCore import *

from math import *
from time import *

helo = Object.GetInstance("UH-1N")

transform = Transform()

angle = 0

def radians(v):
   return v * pi/180

while True:
   transform.Set(40*cos(radians(angle)),
                 100 + 40*sin(radians(angle)), 
                 0, 
                 angle, 0, -45,
                 1.0,1.0,1.0)
   helo.SetTransform(transform)
   sleep(0.01)
   angle += 0.45
