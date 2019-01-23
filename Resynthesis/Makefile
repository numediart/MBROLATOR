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

CC = gcc
CFLAGS += -O5
# CFLAGS += -Wall
#CFLAGS += -DLITTLE_ENDIAN
#CFLAGS += -DBIG_ENDIAN

all: database_build resynth resynth_debug resynth_female resynth_resample16_8 resynth_resample16_11

resynth: Src/resynth.c
	$(CC) $(CFLAGS) -o resynth Src/resynth.c -lm

resynth_debug: Src/resynth.c
	$(CC) $(CFLAGS) -DDEBUG -o resynth_debug Src/resynth.c -lm

resynth_female: Src/resynth.c
	$(CC) $(CFLAGS) -DFEMALE -o resynth_female Src/resynth.c -lm

resynth_sybil: Src/resynth.c
	$(CC) $(CFLAGS) -DSYBIL -o resynth_sybil Src/resynth.c -lm

resynth_resample16_8: Src/resynth.c
	$(CC) $(CFLAGS) -c -DRESAMPLING16_8 -o Src/filter_file.o Src/filter_file.c
	$(CC) $(CFLAGS) -c -DRESAMPLING -DRESAMPLING16_8 -o Src/resynth.o Src/resynth.c
	$(CC) $(CFLAGS) -o  resynth_resample16_8 Src/resynth.o Src/filter_file.o -lm

resynth_resample16_11: Src/resynth.c
	$(CC) $(CFLAGS) -c -DRESAMPLING16_11 -o Src/filter_file.o Src/filter_file.c
	$(CC) $(CFLAGS) -c -DRESAMPLING -DRESAMPLING16_11 -o Src/resynth.o Src/resynth.c
	$(CC) $(CFLAGS) -o resynth_resample16_11 Src/resynth.o Src/filter_file.o -lm

database_build: Src/database_build.c
	$(CC) $(CFLAGS) -o database_build Src/database_build.c -lm

clean : 
	\rm -f Src/*.o resynth_resample resynth_female resynth_debug resynth database_build
