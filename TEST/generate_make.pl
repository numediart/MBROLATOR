#!/usr/bin/perl -w
#
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
#
# Purpose: light weight system to construct a Unix makefile dedicated
#          to build a Mbrola diphone database

if (@ARGV < 4)
{
    die("usage: generate_make.pl database path_raw path_mbe path_res [sampling_rate]");
}

my $database_name = shift @ARGV || die("Waiting for database_name");
my $raw_path = shift @ARGV  || die("Waiting for raw_path");
my $mbe_path = shift @ARGV  || die("Waiting for mbe_path");
my $res_path = shift @ARGV  || die("Waiting for res_path");

my $real_sampling_rate = shift @ARGV || 16000 ; # may not be there

open(SEG_FILE,"<$database_name.seg") || die("can't read $database_name.seg");
open(MAK_FILE,">$database_name.mak") || die("can't write $database_name.mak");

# Read frame length param from .mbe file...
open(MBE_FILE,"<$database_name.mbe") || die("can't read $database_name.mbe");
my $mbe_frame_length;
my $mbe_frame_shift;
while (<MBE_FILE>)
{
    if ($_ =~ /FrameLength[]*=[ ]*([0-9]+)/)
    { $mbe_frame_length= $1; }
    if ($_ =~ /FrameShift[]*=[ ]*([0-9]+)/)
    { $mbe_frame_shift= $1; }
}
close(MBE_FILE);
($mbe_frame_length && $mbe_frame_shift) || die("FATAL: .mbe file doesn't define FrameLength and FrameShift\n");

# Test if the fr1.build file exist, if yes, defer the database_buid operation to this script */
if (-f "$database_name.build")
{
    print MAK_FILE "$database_name : diph_resynth\n";
    print MAK_FILE "\t$database_name.build\n";
}
else
{
    # older scheme: build command is inside the .mak file
    # database_build ce6 SYN/ 2.040 16000 90 1
    print MAK_FILE "$database_name : diph_resynth\n";
    print MAK_FILE "\tdatabase_build $database_name $res_path $res_path 2.069 $real_sampling_rate $mbe_frame_shift 1\n";
}

# Build the list of target diphones
my $target_diphones="";
my $line_counter=0;
while ($line=<SEG_FILE>)
{
    my ($wave_name, $left_name, $right_name, $begin, $end, $middle, $dummy) = split(/\s+/,$line,7);
    if ( !$wave_name || !$left_name || !$right_name || !$begin || !$end || !$middle )
    { print "not a seg $line\n"; }
    else
    {
        # First, some sanity checks....
        if (! open(WAVE_FILE,"<$raw_path$wave_name"))
        {
            print STDERR "***** FATAL ERROR\n$raw_path$wave_name does not exist\n";
            next;
        }
        seek(WAVE_FILE,0,2); # seek(0,SEEK_END)
        my $file_size= tell(WAVE_FILE);
        close(WAVE_FILE);

        $file_size = int($file_size / 2) ;       # number of samples

        # print "FILE SIZE= $raw_path$wave_name $file_size\n";
        if ($begin - $mbe_frame_length < 0)
        {
            print STDERR "BEGIN_IS_SHORT $wave_name $left_name $right_name $begin $end $middle\n";
            $begin = $mbe_frame_length + $mbe_frame_shift;
        }
        if ($end + $mbe_frame_length > $file_size)
        {
            print STDERR "END_IS_SHORT $wave_name $left_name $right_name $begin $end $middle\n";
            $end = $file_size - $mbe_frame_length - $mbe_frame_shift;
        }

        # The diphone is correct !?!?
        if ( ($begin < $end) && ($begin < $middle) && ($middle < $end))
        {
            # RULE TO GENERATE THE F0 FILE
            # RES/a-m.f0: RAW/cece.wav ce6.f0
            #            anaf0 RAW/cece.wav RES/a-m.f0 1500 3504 ce6.f0
            my $out_name= "$mbe_path$line_counter";
            my $source_name = "$raw_path$wave_name";
            print MAK_FILE "$out_name.f0 : $source_name $database_name.f0\n";
            print MAK_FILE "\tanaf0 '$source_name' $out_name.f0 $begin $end $database_name.f0\n";

            # RULE TO GENERATE THE MBE FILE
            # RES/a-m.mbe: RES/a-m.f0 ce6.mbe
            # anambe RAW/cece.wav RES/a-m.f0 RES/a-m.mbe 1500 3504 ce6.mbe
            print MAK_FILE "$out_name.mbe: $out_name.f0 $database_name.mbe\n";
            print MAK_FILE "\tanambe $source_name $out_name.f0 $out_name.mbe $begin $end $database_name.mbe\n";

            # RULE TO GENERATE THE .s16 and .pm RESYNTH FILES
            # S16/a-m.s16: RES/a-m.mbe ce6.syn
            # resynth RAW/cece.wav RES/a-m.mbe S16/a-m ce6.syn
            print MAK_FILE "$res_path$line_counter.s16 : $out_name.mbe $database_name.syn\n";
            print MAK_FILE "\t resynth $source_name $out_name.mbe $res_path$line_counter $database_name.syn\n";
            print MAK_FILE "\n";

            $target_diphones .= "$res_path$line_counter.s16 ";  # collect the list of targets
        }
        else
        {
            print STDERR "****** FATAL ERROR\nCOULD NOT CORRECT $wave_name $left_name $right_name $begin $end $middle\n";
        }
        $line_counter ++;
    }
}

print MAK_FILE "diph_resynth: $target_diphones\n";
close(MAK_FILE);

close(SEG_FILE);
