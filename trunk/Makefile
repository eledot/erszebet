#
#  Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 3 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General
#  Public License along with this library; if not, write to the
#  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301 USA

TARGET?= erszebet

CC?=  gcc

SRCS=  ${wildcard *.c}
SRCS+= ${wildcard 3rd/lua/*.c}
SRCS+= ${wildcard 3rd/chipmunk/*.c} ${wildcard 3rd/chipmunk/constraints/*.c}
OBJS=  ${SRCS:%.c=%.o}
WARN=  -W -Wall
INCS=  -I/usr/include ${shell sdl-config --cflags} ${shell pkg-config --cflags flac}
# stupid vorbis.pc
INCS+= ${shell pkg-config --cflags vorbis} -I/usr/include/vorbis -I/usr/local/include/vorbis
LIBS=  -lm -lpthread

ifeq "${RELEASE}" "1"
CFLAGS+= -s -Os -g0 -DRELEASE
STRIP=   strip -s ${TARGET}
else
CFLAGS+= -ggdb -DDEBUG
STRIP=
endif

CFLAGS+=  ${DEFS} ${INCS} ${WARN} -MD -std=c99 -fno-strict-aliasing -D"assert(a)"=
LDFLAGS+= ${LIBS}

${TARGET}: ${OBJS}
	${CC} -o $@ $^ ${LIBS}
	${STRIP}

all: ${TARGET}

clean:
	-@rm -f *.o 3rd/*/*.o *~ 3rd/*/*~ *.d 3rd/*/*.d .*~ 3rd/*/.*~

cleanall: clean
	-@rm -f ${TARGET}

-include *.d
