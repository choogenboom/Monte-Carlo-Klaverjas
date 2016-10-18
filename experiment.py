#!/usr/bin/python
from subprocess import check_output
import random
	
num = 16

sp1 = '1'
sp2 = '2'
sp3 = '1'
sp4 = '2'

som1 = 0
som2 = 0

intmax = 2147483647

f = open('result.txt', 'w')

for i in range(num):
  seed = str(random.randint(0,2147483647))
  print(seed)
  rout = check_output(['./klaverjas', '-e', sp1, sp2, sp3, sp4, seed])
  output = str(rout)[2:-3]

  team1 = int(output.split()[0])
  team2 = int(output.split()[1])

  som1 = som1 + team1
  som2 = som2 + team2

  f.write("1: " + str(team1) + ", 2: " + str(team2) + "\n")

f.write("\n\n1+3: " + str(som1) + ", 2+4: " + str(som2) + "\n")
f.close()
