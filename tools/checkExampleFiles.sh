#! /bin/bash

files=test/*.C

for f in $files; do
	basename $f
	grep `basename $f` include/Examples.H
	echo ''
done
