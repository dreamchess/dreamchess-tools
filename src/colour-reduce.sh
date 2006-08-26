#!/bin/bash

# colour-reduce.sh
# Copyright (C) 2006  The DreamChess project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

function usage {
	echo "Usage: colour-reduce.sh <infile> <maptype> <outfile>"
	echo
	echo "Supported colour map types: rgb565 rgba5551 rgba4444"
	exit
}

if [ "$#" -ne "3" ]
then
	usage
fi

which convert &>/dev/null

if [ "$?" -ne "0" ]
then
	echo "ImageMagick not found"
	exit 1
fi

tempdir=${TMPDIR-/tmp}/colour-reduce.$$
tempfile=$tempdir/colourmap.png

(umask 077 && mkdir $tempdir) || {
	echo "Could not create temporary directory."
	exit 1
}

# Avoid the &, | and : operators that are broken pre-6.2.9
if [ "$2" = "rgba4444" ]
then
	convert xc:[256x256] -channel R -fx '(i%16)/15' -channel G -fx '(i%64-i%16)/240+(j%64-j%16)/60' -channel B -fx '(j%16)/15)' -channel A -fx '(i-i%64)/960+(j-j%64)/240' -depth 8 -quality 95 $tempfile
elif [ "$2" = "rgba5551" ]
then
	convert xc:[256x256] -channel R -fx '(i%32)/31' -channel G -fx '(i-i%32)/992+(j%128-j%32)/124' -channel B -fx '(j%32)/31)' -channel A -fx '(j-j%128)/128' -depth 8 -quality 95 $tempfile
elif [ "$2" = "rgb565" ]
then
	convert xc:[256x256] -channel R -fx '(i%32)/31' -channel G -fx '(i-i%32)/2016+(j-j%32)/252' -channel B -fx '(j%32)/31)' -depth 8 -quality 95 $tempfile
else
	usage
fi

convert $1 -map $tempfile -quality 95 $3

rm -rf $tempdir
