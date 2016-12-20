CC=g++
RM=rm -f
# CPPFLAGS=-g
# LDFLAGS=-g $(shell root-config --ldflags)
# LDLIBS=$(shell root-config --libs)

SRCS=klaverjas.cc montecarlo.cc speelpas.cc spelers.cc

all: klaverjas

klaverjas: 
	$(CC) -O2 -o klaverjas $(SRCS)

debug: 
	$(CC) -g -o klaverjas klaverjas.cc montecarlo.cc speelpas.cc spelers.cc

clean: 
	rm -f klaverjas