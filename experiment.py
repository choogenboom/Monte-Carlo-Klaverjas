#!/usr/bin/python
import subprocess
import random
from math import *

num = 16*500

sp1 = '5'
sp2 = '2'
sp3 = '5'
sp4 = '2'

som1 = 0
som2 = 0

komtuit = 0

intmaxout = subprocess.check_output(['./klaverjas', '-im'])
intmax = int(str(intmaxout)[2:-3])
errors = 0

f = open('result.txt', 'w')

for i in range(num):
  seed = str(random.randint(0, intmax))
  fail = False

  try:
    rout = subprocess.check_output(['./klaverjas', str('-e'), sp1, sp2, sp3, sp4, seed, str(komtuit)], shell=False)
  except subprocess.CalledProcessError as grepexc:
    print("fail, opnieuw " + str(i))
    fail = True;
  if not fail:
    output = str(rout)[2:-3]

    team1 = int(output.split()[0])
    team2 = int(output.split()[1])

    som1 = som1 + team1
    som2 = som2 + team2

    f.write("1: " + str(team1) + ", 2: " + str(team2) + "\n")
    komtuit = (komtuit + 1) % 4
    print(str(round((i / num) * 100, 1)) + "%", end='\r')
  else:
    i = i - 1
    errors = errors + 1

f.write("\n\n1+3: " + str(som1) + ", 2+4: " + str(som2) + "\n" + "Errors: " + str(errors) + "\n")
f.close()