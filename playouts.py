#!/usr/bin/python
import subprocess
import random
import timeit
from math import *
from multiprocessing import Pool
from functools import partial

boompjes = 100
# keer 4 omdat het later nog keer 4 gedaan wordt 
num = boompjes * 4

filename = "playouts.txt"

sp1 = '3'
sp2 = '3'
sp3 = '3'
sp4 = '3'

intmaxout = subprocess.check_output(['./klaverjas', '-im'])
intmax = int(str(intmaxout)[2:-3])

def run(j, komtuit): 
  seed = str(random.randint(0, intmax))
  fail = False
  try:
    rout = subprocess.check_output(['./klaverjas', str('-en'), sp1, sp2, sp3, sp4, seed, str(komtuit), str(j), "1000", str(j), "1000"], shell=False)
  except subprocess.CalledProcessError as grepexc:
    fail = True;
    print("Fail")
  if not fail:
    output = str(rout)[2:-3]

    score1 = int(output.split()[0])
    score2 = int(output.split()[1])
    result = [score1, score2]

    return result
  else:
    run(j, komtuit)

for j in range(500, 10001, 500):
  results = []
  team1 = []
  team2 = []
  tijd = 0

  for i in range(num):
    if __name__ == '__main__':
      print(str(j / 100) + "% - " + str(round((i / num) * 100, 1)) + "%", end='\r')
      func = partial(run, j)

      start_time = timeit.default_timer()
      pool = Pool(processes=4)
      results = pool.map(func, range(4))
      pool.close()
      pool.join()
      ntijd = timeit.default_timer() - start_time

      for k in range(4):
        team1.append(results[k][0])
        team2.append(results[k][1])
        tijd = tijd + ntijd

  som1 = sum(team1)
  som2 = sum(team2)
  tijd = tijd / (num)
  
  with open(filename, "a") as f:
    f.write(str(j) + ": 1+3=" + str(som1) + ", 2+4=" + str(som2) + "tijd=" + str(tijd) + "\n")