CC = g++
RM = rm -f
OBJS = klaverjas.o montecarlo.o speelpas.o spelers.o
DEBUG = -g
CFLAGS = -Wall -c
LFLAGS = -Wall

all: opt

opt: CFLAGS += -O2
opt: LFLAGS += -O2
opt: klaverjas

debug: CFLAGS += -g
debug: LFLAGS += -g
debug: klaverjas

klaverjas: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o klaverjas

klaverjas.o: klaverjas.cc klaverjas.h montecarlo.h speelpas.h
	$(CC) $(CFLAGS) klaverjas.cc

montecarlo.o: montecarlo.cc montecarlo.h spelers.h
	$(CC) $(CFLAGS) montecarlo.cc

speelpas.o: speelpas.cc speelpas.h
	$(CC) $(CFLAGS) speelpas.cc

spelers.o: spelers.cc spelers.h
	$(CC) $(CFLAGS) spelers.cc

clean:
	\rm *.o klaverjas