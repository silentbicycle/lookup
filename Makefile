PROJECT =	lookup
OPTIMIZE =	-O3
WARN =		-Wall -Wextra -pedantic
#CDEFS +=
#CINCS +=
#CSTD +=	-std=c99
CFLAGS +=	${CSTD} -g ${WARN} ${CDEFS} ${CINCS} ${OPTIMIZE}
#LDFLAGS +=
TEST_LDFLAGS +=

all: ${PROJECT}

OBJS=

TEST_OBJS=

# Basic targets

${PROJECT}: main.o ${OBJS}
	${CC} -o $@ main.o ${OBJS} ${LDFLAGS}

lib${PROJECT}.a: ${OBJS}
	ar -rcs lib${PROJECT}.a ${OBJS}

test_${PROJECT}: test_${PROJECT}.o ${OBJS} ${TEST_OBJS}
	${CC} -o $@ test_${PROJECT}.o ${OBJS} ${TEST_OBJS} ${CFLAGS} ${LDFLAGS}

test: ./test_${PROJECT}
	./test_${PROJECT}

clean:
	rm -f ${PROJECT} test_${PROJECT} *.o *.a *.core

# Installation
PREFIX ?=	/usr/local
INSTALL ?=	install
RM ?=		rm

install: ${PROJECT}
	${INSTALL} -c ${PROJECT} ${PREFIX}/bin

uninstall:
	${RM} -f ${PREFIX}/bin/${PROJECT}
