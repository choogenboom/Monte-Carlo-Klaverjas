#!/usr/bin/python
from subprocess import check_output
import random
from math import *

num = 16

sp1 = '1'
sp2 = '1'
sp3 = '1'
sp4 = '1'

som1 = 0
som2 = 0

komtuit = 0

intmaxout = check_output(['./klaverjas', '-im'])
intmax = int(str(intmaxout)[2:-3])

f = open('result.txt', 'w')

for i in range(num):
  seed = str(random.randint(0, intmax))
  rout = check_output(['./klaverjas', str('-e'), sp1, sp2, sp3, sp4, seed, str(komtuit)], shell=False)

  output = str(rout)[2:-3]

  team1 = int(output.split()[0])
  team2 = int(output.split()[1])

  som1 = som1 + team1
  som2 = som2 + team2

  f.write("1: " + str(team1) + ", 2: " + str(team2) + "\n")
  komtuit = (komtuit + 1) % 4

f.write("\n\n1+3: " + str(som1) + ", 2+4: " + str(som2) + "\n")
f.close()
