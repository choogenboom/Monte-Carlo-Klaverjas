#!/usr/bin/python3
import subprocess
import random
from math import *
from multiprocessing import Pool

T1 = 1
T2 = 5

boompjes = 100
num = boompjes * 4

filename = "vs/" + str(T1) + "|" + str(T2) + ".txt"
with open(filename, "a") as f:
  f.write("T1=" + str(T1) + ", T2=" + str(T2) + "\n\n")

if T1 == 1:
  sp1 = '100'
  sp3 = '100'
elif T1 == 2:
  sp1 = '4'
  sp3 = '4'
elif T1 == 3:
  sp1 = '3'
  sp3 = '3'
elif T1 == 4:
  sp1 = '2'
  sp3 = '2'
elif T1 == 5:
  sp1 = '5'
  sp3 = '5'
else:
  print("Foute teamnaam")

if T2 == 1:
  sp2 = '100'
  sp4 = '100'
elif T2 == 2:
  sp2 = '4'
  sp4 = '4'
elif T2 == 3:
  sp2 = '3'
  sp4 = '3'
elif T2 == 4:
  sp2 = '2'
  sp4 = '2'
elif T2 == 5:
  sp2 = '5'
  sp4 = '5'
else:
  print("Foute teamnaam")

results = []
team1 = []
team2 = []

intmaxout = subprocess.check_output(['./klaverjas', '-im'])
intmax = int(str(intmaxout)[2:-3])

def run(komtuit): 
  seed = str(random.randint(0, intmax))
  fail = False
  try:
    rout = subprocess.check_output(['./klaverjas', str('-e'), sp1, sp2, sp3, sp4, seed, str(komtuit)], shell=False)
  except subprocess.CalledProcessError as grepexc:
    fail = True;
  if not fail:
    output = str(rout)[2:-3]

    score1 = int(output.split()[0])
    score2 = int(output.split()[1])

    result = [score1, score2]

    return result
  else:
    errors = errors + 1
    print("Error...\n")
    run(komtuit)

for i in range(num):
  if __name__ == '__main__':
    print(str(round((i / num) * 100, 1)) + "%", end='\r')
    pool = Pool(processes=4)
    results = pool.map(run, range(4))
    pool.close()
    pool.join()

    for k in range(4):
      team1.append(results[k][0])
      team2.append(results[k][1])

som1 = sum(team1)
som2 = sum(team2)

with open(filename, "a") as f:
  f.write("1+3: " + str(som1) + ", 2+4: " + str(som2) + "\n")
  f.write("1+3: " + str(som1 / (num * 4)) + ", 2+4: " + str(som2 / (num * 4)) + "\n")