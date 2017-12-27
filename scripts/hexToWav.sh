#! /bin/bash
#   Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
#   This file is part of GTK+ IOStream class set
#
#   GTK+ IOStream is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   GTK+ IOStream is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You have received a copy of the GNU General Public License
#   along with GTK+ IOStream

# Author Matt Flax <flatmax@flatmax.org>

if [ ${#} -ne 3 ]; then
	echo usage:
	echo `basename $0` hexChars sampleRate secondsDuration
	exit 1
fi

# script to convert hex numbers to a wav file - currently assumes stereo 32 bit
if [ "${#1}" -ne 8 ]; then
	echo Input $1 is not 32 bits \(8 char\)
	exit 1
fi
hexStr=''
for (( i=0; i<${#1}; i=i+2 )); do
  hexStr=$hexStr"\\x${1:$i:2}"
done

hexStereoWord=$hexStr$hexStr
rawStereoWord=$1$1
N=`echo "$2*$3" | bc -l`
# floor N
N=${N%.*}

echo repeating $hexStereoWord $N times


fileData=`for i in $(seq 1 $N); do  echo -n $hexStereoWord; done`
#fileData=`for i in $(seq 1 $N); do  echo -n $rawStereoWord; done`

#echo ${#fileData}
#echo $fileData
rawFn=/tmp/$1.raw
echo -n -e $fileData > $rawFn
#echo $fileData | xxd -r $rawFn
#echo $fileData | dd of=$rawFn #conv=swab
echo writing to ${rawFn%.raw}.wav
sox -r $2 -e signed -b 32 -c 2 $rawFn ${rawFn%.raw}.wav
