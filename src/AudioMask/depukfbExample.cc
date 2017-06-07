/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2010  Dr Matthew Raphael Flax

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define FASTDEPUKFB

#ifdef FASTDEPUKFB
#include "fastDepukfb.H"
#else
#include "depukfb.H"
#endif

#include <fstream>

#define FS 8192

void main(void){
  int binCount=4096;
#ifdef FASTDEPUKFB
  FastDepUKFB depUKFB(FS, 20);
#else
  DepUKFB depUKFB(FS);
#endif
  ofstream output("w");
  for (int i=0;i<depUKFB.filterCount();i++){
    for (int j=0;j<binCount;j++)
      output<<depUKFB(i,j,binCount)<<'\t';
    output<<endl;
  }
  output.close();
}
