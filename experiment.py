#!/usr/bin/python
import subprocess
import random
from math import *
from multiprocessing import Pool

boompjes = 5
num = boompjes * 4

filename = "result.txt"

sp1 = '5'
sp2 = '2'
sp3 = '5'
sp4 = '2'

results = []
team1 = []
team2 = []

intmaxout = subprocess.check_output(['./klaverjas', '-im'])
intmax = int(str(intmaxout)[2:-3])
errors = 0

def som(input):
  output = 0
  for i in input:
    print(str(input[i]) + " " + str(output))
    output = output + input[i]

  return output

def run(komtuit): 
  seed = str(random.randint(0, intmax))
  fail = False
  # print("Run met komtuit=" + str(komtuit))
  try:
    rout = subprocess.check_output(['./klaverjas', str('-e'), sp1, sp2, sp3, sp4, seed, str(komtuit)], shell=False)
  except subprocess.CalledProcessError as grepexc:
    fail = True;
  if not fail:
    output = str(rout)[2:-3]

    score1 = int(output.split()[0])
    score2 = int(output.split()[1])

    result = [score1, score2]

    with open(filename, "a") as f:
      f.write(str(komtuit) + " - 1: " + str(score1) + ", 2: " + str(score2) + "\n")
    
    # print(str(komtuit) + " - 1: " + str(score1) + ", 2: " + str(score2))
    return result
  else:
    errors = errors + 1
    print("Error...\n")
    run(komtuit)

for i in range(num):
  if __name__ == '__main__':
    print(str(round((i / num) * 100, 1)) + "%", end='\r')
    # Python 3.5:
    # with Pool(processes=4) as pool:
    pool = Pool(processes=4)
    results = pool.map(run, range(4))
    pool.close()
    pool.join()

    for k in range(4):
      team1.append(results[k][0])
      team2.append(results[k][1])

som1 = sum(team1)
som2 = sum(team2)
# print(str(len(results)))
# print(team1)
# for j in team1:
#   som1 = som1 + team1[j]
#   som2 = som2 + team2[j]

with open(filename, "a") as f:
  f.write("\n\n1+3: " + str(som1) + ", 2+4: " + str(som2) + "\n" + "Errors: " + str(errors) + "\n")