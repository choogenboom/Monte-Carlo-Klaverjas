#!/usr/bin/python
# -*- coding: utf-8 -*-
import re
filename = "kaarten"

f = open(filename, 'r')
f2 = open(filename + ".kvj", 'w')

strorig = f.read()
spelers = strorig[0:8]
str = strorig[8:]

str = str.replace("10 ", "4")
str = str.replace("♠7 ", "0")
str = str.replace("7 ", "0")
str = str.replace("8 ", "1")
str = str.replace("V ", "2")
str = str.replace("H ", "3")
str = str.replace("A ", "5")
str = str.replace("9 ", "6")
str = str.replace("B ", "7")

str = str.replace("♠", "")
str = str.replace("♥", "1")
str = str.replace("♣", "2")
str = str.replace("♦", "3")

str = str.replace("0: ", "")
str = str.replace("1: ", "")
str = str.replace("2: ", "")
str = str.replace("3: ", "")

kleur = input("Voer troefkleur in (-1 voor nog te bepalen): ")
speelt = input("Voer in wie speelt: ")
komtuit = input("Voer in wie uitkomt: ")

f2.write(spelers + str + "\n" + kleur + " " + speelt + " " + komtuit + "\n")