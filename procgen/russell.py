import gym
import procgen

env = gym.make('procgen-miner-v0')

for i in range(249):
    env.step(1)

import code
code.interact(local=locals())
