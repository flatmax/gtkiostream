/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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
#include <gtkInterface.H>
#include <Labels.H>
#include <Box.H>
#include <Frame.H>

int main(int argc, char *argv[]){

	gtk_init( &argc, &argv ); // init GTK

	GtkInterface topWindow; // Create the top box

	Labels labs; // Instantiate the Labels class and load three labels
	labs<<"Hello world !"
		 <<"You are programming me"
		 <<"thank you";

	VBox vBox; // Instantiate the vertical packing box class
	vBox<<labs; // Load the labels into the box
	vBox.show(); // Show the box

	Frame boxFrame("Computer says");
	boxFrame<<vBox;
	topWindow<<boxFrame.show();

	gtk_main(); // run GTK+
}
