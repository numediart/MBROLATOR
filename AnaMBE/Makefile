# Copyright (c) 1995-2019 Faculté polytechnique de Mons (TCTS lab)
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC = g++

AR = ar

CFLAGS=
# CFLAGS += -g
# CFLAGS += -DALPHA

CFLAGS += -O6
#CFLAGS += -DLITTLE_ENDIAN
#CFLAGS += -DBIG_ENDIAN
#CFLAGS += -m32

ANAF0SRCS = anaf0.cpp soundfil.cpp pitchana.cpp mbefunc.cpp datatype.cpp func.cpp
MBESRCS = complex.cpp signalpr.cpp mbe.cpp soundfil.cpp pitchana.cpp mbefunc.cpp datatype.cpp func.cpp anambe.cpp

BINDIR = Obj

ANAF0OBJS = $(ANAF0SRCS:%.cpp=$(BINDIR)/%.o)
MBEOBJS = $(MBESRCS:%.cpp=$(BINDIR)/%.o)

PROJ = anaf0 anambe

all:  $(BINDIR) $(PROJ)

$(BINDIR):
	@if ! [ -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi

anaf0: $(ANAF0OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/anaf0 $(ANAF0OBJS) -lm

anambe : $(MBEOBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/anambe $(MBEOBJS) -lm

clean:
	rm -f $(ANAF0OBJS) $(MBEOBJS) $(PROJ) $(PROJ).a core TAGS

$(BINDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
