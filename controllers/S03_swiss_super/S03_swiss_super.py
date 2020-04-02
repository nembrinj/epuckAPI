"""my controller description."""

from controller import *
import random

NB_BLOCKS = 20
NB_EPUCKS = 4

robot = Supervisor()

timestep = int(robot.getBasicTimeStep())

blocks = []
for i in range(NB_BLOCKS) :

    block = robot.getFromDef('BLOCK'+str(i+1)).getField("translation")
    blocks.append(block)

epucks = []
for i in range(NB_EPUCKS) :

    ep = robot.getFromDef('EP'+str(i+1)).getField("translation")
    epucks.append(ep)

def randomPosition() :
    return [random.uniform(-0.45,0.45),
            0.025,
            random.uniform(-0.45,0.45)]

def minDistance(pos):
    
    min= 100000
    
    for b in blocks :
        p = b.getSFVec3f()
        dist = abs(p[0]-pos[0])+abs(p[2]+pos[2])
        if dist<min:
          min = dist

      
    for ep in epucks :
        p = ep.getSFVec3f()
        dist = abs(p[0]-pos[0])+abs(p[2]+pos[2])
        if dist<min:
          min = dist
    return min


def findRandomPosition(p) :

    newpos = randomPosition()
    while minDistance(newpos) < 0.3 :
        newpos = randomPosition()
    
    return newpos


step = 0

while (robot.step(timestep) != -1):

  step += 1
  if step % 30 == 0 :
      for b in blocks :
          p = b.getSFVec3f() # vector in [x,z,y]1
          if (abs(p[0])>0.55) or (abs(p[2])>0.55):
              b.setSFVec3f(findRandomPosition(p))

          print([p[0],p[2]])
          
      for ep in epucks :
          p = ep.getSFVec3f()
          print([p[0],p[2]])
 
# Enter here exit cleanup code
