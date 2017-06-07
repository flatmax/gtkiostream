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
