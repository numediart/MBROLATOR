# MBROLA database creation tool

This guide describes how to create voice database for the [MBROLA](https://github.com/numediart/MBROLA)
speech synthesizer.

__General steps__

- [PHASE 0: compile tools](#phase-0-compile-tools)
- [PHASE 1: check the data](#phase-1-checking-the-data)
- [PHASE 2: generate analysis parameter files](#phase-2-generate-analysis-parameter-files)
- [PHASE 3: generate the makefile](#phase-3-generate-the-makefile)
- [PHASE 4: execute makefile](#phase-4-execute-makefile)
- [PHASE 5: test the result](#phase-5-test-the-result)
- [PHASE 6: debugging?](#phase-6-debugging)
- [ADDENDUM:](#addendum)
  - [ADDENDUM: 22Khz databases!!!!](#addendum-22khz-databases)
  - [ADDENDUM: running on windows....](#addendum-running-on-windows)

## PHASE 0: compile tools

1) Compiling MBE analysis
    1. in AnaMBE directory, type `make`
    1. you get some warnings about conversion from int to double,
       hopefully nothing lethal.
    1. resulting binaries Obj/anaf0 Obj/anambe must be copied with root
       privilege under /usr/local/bin (or any other searchable path).

2) Compile resynthesis and database_build
    1. in Resynthesis directory, type `make`
    1. copy resynth* and database_build binaries in /usr/local/bin
       (or any other searchable path).

3) Then you need to know about the diphone provider instruction
file... because here you're going to compile what the diphone provider
gives you.

## PHASE 1: check the data

you need audio file encoded as linear 16 bits, 16khz, little endian
(PC format), and a text segmentation file containing something that
looks like:

```
file name
|        diphone name
|       |        first sample
|       |        |     last sample
v       |        |     |      middle sample
        V        V     V      v
##a.d16	_ a	2000	3910	2929
ak.d16	a k	2000	4027	2939
i#.d16	i _	2000	4654	3228
ki.d16	k i	2000	3882	3150
```

First source of problems: names in the .seg file and file names do not
correspond well — uppercase and lowercase sometimes need to be
updated by yourself or by the provider.

Second source of problems: strange diphone names — for example,
silence better be "_", rather than "#" (which is Flush
character). Although it can be parametrized under mbrola it's looking
for trouble.

Third source of problems: people sometimes mess up segmentation
boundaries order, or they write bytes instead of samples — whatever, it
will generate an error later.

Fourth source of problems: if diphone **\_-\_** (silence silence) is
provided it *must* be the last diphone of the database in the seg
file. But generally it is advised to remove that phoneme as it is looking
for trouble (MBE analysis dies on numeric silence).

Fifth source of problems mentioned above: never never never introduce
any numeric silence (sequence of 0). First absolute 0 are not natural,
second anambe crashes badly trying to compute infinite decibel
spectral slices...

## PHASE 2: generate analysis parameter files

If we have audio files in a directory, say WAV directory for example, and
`br1w5.seg` segmentation file. Final database's name will be
`br1w5`. One has to build different parameter files for speech analysis:

```
generate_mbrola br1w5
```
will automatically build "default" files:

```
-rw-r--r--   1 user    program       44 Jul 25 11:24 br1w5.f0
-rw-r--r--   1 user    program       52 Jul 25 11:24 br1w5.mbe
-rw-r--r--   1 user    program       91 Jul 25 11:24 br1w5.syn
```

They resp. contain data for three different analysis:

1) f0 analysis (pitch, with anaf0 binary)  br1w5.f0
2) harmonic/noise analysis (MBE model with anambe binary) br1w5.mbe
3) mbrola format frame resynthesis (resynth binary) br1w5.syn

In a first approach keep default values, only mbe and syn need be
changed in the following. Let's do it now for mbe — listen to some of
your audio files to get an idea of speaker's average pitch, then
measure the length of 1 pitch period in samples (for example using
wavesurfer or any audio editor you like). E.g. on the brazilian voice
we find 144 samples (i.e. a 90Hz pitch), so edit .mbe file

```
Nfft=1024
Freq=16000
FrameLength=360
FrameShift=120
```

Replace FrameShift field with 144, and FrameLength field with 144*3
(it's empirical but it works!) that is 432.

## PHASE 3: generate the makefile

Three analysis (f0, mbe, resynthesis) are done on each audio file and
generate an s16 result. To avoid manual handling on updates of
segmentation use `make` or `pmake` (historical reason: in 1996 Mbrola
analysis was quite slow on 1 machine, hence `pmake` was used to
distribute analysis/resynthesis of a database on several Unix machines
while we were having a beer on the Grand Place of Mons).

```
generate_make.pl br1w5 WAV/ RES/ RES/
```
will generate `br1w5.mak` that's gonna use audio files in WAV/ (don't
forget the slash), and put result and intermediate files in RES/.

Example of a block in the mak file:

```
RES/0.f0 : WAV/1.d16 test.f0
        anaf0 'WAV/1.d16' RES/0.f0 2000 3810 test.f0
RES/0.mbe: RES/0.f0 test.mbe
        anambe WAV/1.d16 RES/0.f0 RES/0.mbe 2000 3810 test.mbe
RES/0.s16 : RES/0.mbe test.syn
         resynth WAV/1.d16 RES/0.mbe RES/0 test.syn
```

This block describes the construction of 0.f0 , 0.mbe and 0.s16
files from WAV/1.d16 and parameters in br1w5.f0 br1w5.mbe
br1w5.syn.

## PHASE 4: execute makefile

    make -f br1w5.mak

Some errors might occur — one of the major reason for errors is
insufficient audio context for a diphone. I usually ask for 800
samples for safety, but some smart guys sometimes provide shorter
context, as a result, even though segmentation boundaries are inside
of the wav file, due to the size of analysis windows, it's reading
outside.
Solution: either shorten the diphone a little bit, either send a note
to the provider for a greater audio context (and come on, audio
context doesn't mean adding *silence* on both sides, wicked idea...)

## PHASE 5: test the result

Usually make command ends with a line like:


    database_build br1w5 RES/ 2.069 16000 144 1


If no _-_ diphone was provided, database_build automatically includes
one. No worry.
If you have a br1w5.info text file in the directory, it is included at
the end of the database as provider's data (can be read with mbrola -i).

Yes, yes you can now test an audio output!

    mbrola br1w5 TEST/testw1.pho test.wav

Voila! It should sound ok. Now, one can do fine tuning.... one can set
voiced/unvoiced ratios in resynthesis .syn parameters:

```
Freq=16000
LastPhaseReset=10
VUVLimTra=0.55
VUVLimVoiced=1.2
ElimTrans=5e8
ElimVoiced=5e8
```

Mainly ElimTrans and ElimVoiced have the same value, and indicate
energy level below which we consider we have silence (if the database
dynamic is weak one has to lower this value).

VUVLimVoiced is the main parameter to tune voiced/unvoiced effect (if
the database is over-voiced, for example you may spot that /S/ is
voiced).

VUVLimTra tune smoothing level — the higher, the more mbrola's going
to smooth (even when it's harmfull, e.g. between two parts of a
/b/). This value corresponds to "transition threshold" — when we
declare that an MBE frame is a transition it means "no smoothing
beyond" (e.g. plosives, sonorant where formants move quickly).

When you modify .syn parameters, simply launch make again — you'll
notice everything goes faster because .f0 and .mbe files are not
recomputed (mbe analysis is by far the most greedy part of the
process).

If you modify the segmentation boundaries of a diphone, simply execute
`touch WAV/my_file.wav` on corresponding audio file to force make
rebuild of its parameters.

There's a special "resynth_debug" version of "resynth" binary that
generates .m and .mm matlab files. It displays under matlab f0 and mbe
coefficients which comes handy to set sound thresholds.

Another special version is "resynth_female", to process high female
pitches — edit .mbe param file to double analysis frame length (goes
1 harmonic higher), edit .mak file to use resynth_female instead of
resynth_male and launch make. What happens next? During resynthesis,
pitch is doubled (analysis frame was multiplied by 2, then pitch is
divided by 2). At the same time, when doubling the pitch, I associate
phases with their correct harmonic, and duplicate them for odd
harmonics (one has to invent phases for those harmonics).

## PHASE 6: debugging?

listen to your mbrola output to track any over/under smoothing on
unvoiced/voiced plosives or fricatives.

You're all set.

## ADDENDUM:

Downsampling a database... Edit .mak to use resynth_resample instead
of resynth (filter can be modified in Resynthesis/Src/COEF_FILTER
depending on target freq). Actually every .s16 file is downsampled
into a .s10 file.

You may also to modify the make file:

    database_build br1w5 RES/ 2.060 16000 144 1
becomes:

    database_build br1w5 RES/ 2.060 8000 77 1 16000

AVAILABLE RESAMPLING: 16khz to 8khz (resynth_resample16_8, works for
every 1/2 ratios, e.g. from 22050 to 11025), and 22050Hz to 8000Hz
with resynth_resample22_8

### 22Khz databases!!!!

It's the hack of the hack... *never* try to replace 16000 parameters
in your .mbe or .f0 or .syn files, it could make sense, but it's a
trap. All those programs were handcrafted a very long time ago by
Thierry Dutoit and *everything* has been designed for 16000Hz (some
band pass filters inside, some frequency dependent obscure constants).

The hack of the hack is: don't touch anything, pretend you work at
16000Hz on 22050Hz files (you get quite long periods generally but who
cares). Simply indicate real sample rate at the very last minute on
"database_build".

### running on windows....

Well, what does it take to run this on windows? Well either Cygwin
tools :-)

Or simply modify generate_make.pl so that instead of building a .mak
with dependencies between files, it builds a .bat with explicit
commands. Nowadays building a mbrola database is a cinch for any PC,
which renders usage of .mak somehow obsolete (it used to be elegant).
