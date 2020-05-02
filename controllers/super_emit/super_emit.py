"""my controller description."""

from controller import *
from controller import Emitter

msg = 'FFFF'

robot = Supervisor()

timestep = int(robot.getBasicTimeStep())


emit = robot.getEmitter('emitter')

print(emit)

step = 0

while (robot.step(timestep) != -1):

  step += 1
  if step % 30 == 0 :
      
      emit.send(msg)
