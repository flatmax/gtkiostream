#! /bin/bash
# author : Matt Flax <flatmax@>
# date : 2012.06.12

# filter for doxygen to be able to parse .ice files as if they were c++ files

#echo $1 $2 $3 $# > /tmp/temp.txt

cat $1 | sed 's/module/namespace/g' | sed 's/interface/class/g' | sed 's/sequence/vector/g' | sed 's/dictionary/map/g' | sed 's/extends/: public/g'

