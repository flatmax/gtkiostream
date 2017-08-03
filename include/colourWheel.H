/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
 */
#ifndef COLOURWHEEL_H_
#define COLOURWHEEL_H_

#include <iostream>
#include <gtk/gtk.h>
#include "../3daudio/SDevices.H"

//5/8 corresponds to -1,-1,0
#define MAXANGLE (5.0/8.0)*2.0*M_PI
//tan(-1/-1) corresponds to -1,-1,0
//#define MAXANGLE 1.557407725*2.0*M_PI
//#define MAXANGLE 2.0*M_PI-3.0/4.0*M_PI
#define DIFFUSIVITY 1.0

class ColourWheel {
  void allocColours(void){
    // Use a 2D speaker panner to derive colours ....
    // Set up your speakers ...
    spk.add(1.0, 0.0, 0.0); // RED
    spk.add(0.0, 1.0, 0.0); // GREEN
    spk.add(-1.0,-1.0,0.0); // BLUE

    // Set up a source to scan the colour wheel
    src.add(1.0, 0.0, 0.0);
    src.current()->setSharpness(DIFFUSIVITY);

    // Add relevant sources to the speakers sources cluster ...
    srcId=spk.addSources(&src);

    // Fill the colour map
    //    GdkColor tempC;
    for (double i=0.0; i<MAXANGLE; i+=factor){
      loc testLoc(-1.0, 0.0, 0.0), tempLoc;
      tempLoc=testLoc.findVectorFromAngle(i);
      *src.current()=source(tempLoc.x, tempLoc.y, tempLoc.z);
      colourAlloc();
    }
    //    std::cout<<"ColourMap size "<<rainbow->size<<std::endl;
  }

  void colourAlloc(){
    spk.activateNorm();
    colour.red  =(gushort)rint(spk.activations.current()[0]*65535.0);
    colour.green=(gushort)rint(spk.activations.current()[1]*65535.0);
    colour.blue =(gushort)rint(spk.activations.current()[2]*65535.0);
    int res=gdk_colormap_alloc_color(rainbow, &colour, TRUE, FALSE);
    if (res!=TRUE){
      std::cerr<<"ColourWheel:: Colour map colour alloc Failed"<<std::endl;
      exit(-1);
    }
  }

  speakers spk; // Colour plane in X-Y
  sources src;
  uint srcId;
  static double factor;

public:
  GdkColor colour; // Colour returned in colourFromHeight
  GdkColormap *rainbow;

  ColourWheel(void){
    //std::cout<<gdk_colormap_get_system_size()<<std::endl;
    rainbow=gdk_colormap_new(gdk_visual_get_best_with_type(GDK_VISUAL_TRUE_COLOR), TRUE);
    //    rainbow=gdk_colormap_new(gdk_visual_get_best(), TRUE);
    allocColours();

    //Mask sure black is there too ...
    colour.red  =(gushort)rint(0.0);
    colour.green=(gushort)rint(0.0);
    colour.blue =(gushort)rint(0.0);
    int res=gdk_colormap_alloc_color(rainbow, &colour, TRUE, FALSE);
    if (res!=TRUE){
      std::cerr<<"ColourWheel:: Colour map black colour alloc Failed"<<std::endl;
      exit(-1);
    }
  }

  ~ColourWheel(void){
    spk.remSources(srcId); //Clean up
    gdk_colormap_unref(rainbow);
  }

  GdkColor* colourFromHeight(double height, double maxHeight){
    double angle=-MAXANGLE*height/maxHeight;
    //std::cout<<" angle "<<angle<<'\n';
    loc testLoc(-1.0, -1.0, 0.0), tempLoc;
    tempLoc=testLoc.findVectorFromAngle(angle);
    //    std::cout<<tempLoc<<'\t';
    *src.current()=source(tempLoc.x, tempLoc.y, tempLoc.z);
    colourAlloc();
    //std::cout<<'\t'<<colour.red<<'\t'<<colour.green<<'\t'<<colour.blue<<std::endl;
    return &colour;
  }

  GdkColor* black(){
    //Mask sure black is there too ...
    colour.red  =(gushort)rint(0.0);
    colour.green=(gushort)rint(0.0);
    colour.blue =(gushort)rint(0.0);
    int res=gdk_colormap_alloc_color(rainbow, &colour, TRUE, FALSE);
    if (res!=TRUE){
      std::cerr<<"ColourWheel:: Colour map black colour alloc Failed"<<std::endl;
      exit(-1);
    }
    return &colour;
  }

  GdkColor* white(){
    //Mask sure black is there too ...
    colour.red  =(gushort)rint(65535.0);
    colour.green=(gushort)rint(65535.0);
    colour.blue =(gushort)rint(65535.0);
    int res=gdk_colormap_alloc_color(rainbow, &colour, TRUE, FALSE);
    if (res!=TRUE){
      std::cerr<<"ColourWheel:: Colour map white colour alloc Failed"<<std::endl;
      exit(-1);
    }
    return &colour;
  }
};
double ColourWheel::factor=MAXANGLE/65535.0;
#endif //COLOURWHEEL_H_






