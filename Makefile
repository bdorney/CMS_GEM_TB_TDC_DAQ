# 		=======================================================
# 		makefile for stand alone online monitoring applications
# 		=======================================================


CC := g++
LD := g++

OBJS     :=
OBJS_DIR := Linux
BINS_DIR := Linux
SRC_DIR  := src
INC_DIR  := include
TEST_DIR := test

CFLAGS   := -Dlong32="int" -Dlong64="long long" 
CFLAGS   := ${CFLAGS} -I${INC_DIR}
CFLAGS   := ${CFLAGS} -fPIC -O -O2 -O3 -Wall -I/home/isrdaq/software/root/include

LDFLAGS   := -pthread -lm -ldl -rdynamic -ggdc

LOADLIBES := -pthread -lm -ldl -rdynamic -ggdc

INCS := 	${INC_DIR}/IDaqVmeTypes.hh \
					${INC_DIR}/IDaqVmeInterface.hh \
					${INC_DIR}/IDaqVmeModule.hh \
					${INC_DIR}/IDaqV792.hh \
					${INC_DIR}/IDaqV775.hh \
					${INC_DIR}/IDaqV513.hh \
					${INC_DIR}/IDaqManager.hh

OBJS   :=	${OBJS_DIR}/IDaqVmeInterface.o \
					${OBJS_DIR}/IDaqVmeModule.o \
					${OBJS_DIR}/IDaqV792.o \
					${OBJS_DIR}/IDaqV775.o \
					${OBJS_DIR}/IDaqV513.o \
					${OBJS_DIR}/IDaqManager.o
					
LDFLAGS  := ${LDFLAGS} -L./lib -L${OBJS_DIR} -lCAENVME
TESTS := ${OBJS_DIR}/TestDAQ.o
				 
				 
APPS := ${BINS_DIR}/TestDAQ
				
toClean := $(sort ${OBJS_DIR} ${LIBS_DIR} ${BINS_DIR})

#############################################################################	

all : ${BINS_DIR}/TestDAQ

#${APPS} : ${OBJS} 
#	${LD} -o $@ ${TESTS} ${OBJS} ${LDFLAGS}

${BINS_DIR}/TestDAQ	: ${OBJS} ${TESTS}
	${LD} -o ${BINS_DIR}/TestDAQ ${TESTS} ${OBJS} ${LDFLAGS}

${TESTS} : ${TEST_DIR}/TestDAQ.c ${OBJS} ${INCS}
	${CC} ${CFLAGS} -c $< -o $@

${OBJS_DIR}/%.o 	: ${SRC_DIR}/%.cxx ${INC_DIR}/%.h Makefile
	${CC} ${CFLAGS} -c $< -o $@

${OBJS_DIR}/%.o 	: ${SRC_DIR}/%.cxx Makefile
	${CC} ${CFLAGS} -c $< -o $@



#${OBJS_DIR}/%.o : ${TEST_DIR}/%.c ${OBJS} ${INCS} 
#	${CC} ${CFLAGS} -c $< -o $@

clean 		:
	rm -f core */core *~ ${APPS} ${OBJS} ${TESTS}

distclean : clean
ifneq (${toClean},)
	for dir in ${toClean}; do \
	  if [ -d $${dir} ]; then \
	    find $${dir} \! -type d -exec rm -f {} \; ; \
	  fi; \
	done
endif
