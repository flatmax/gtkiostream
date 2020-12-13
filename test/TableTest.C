/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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
#include "Window.H"
#include "Table.H"
#include "Buttons.H"
#include "Labels.H"
#include "Box.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

static void callback(void *wid, gpointer data){
    cout<<((int*)data)[0]<<endl;
}

int main(int argc, char *argv[]){

  gtk_init( &argc, &argv ); // init GTK

  Window topWindow; // Create the top box

  vector<int> datas;
  datas.push_back(1);datas.push_back(2);datas.push_back(3);datas.push_back(4);datas.push_back(5);datas.push_back(6);datas.push_back(7);datas.push_back(8);

  Buttons buttons; // create the buttons to show
  buttons<<BUTTONLABELSTRUCT((char*)"1", callback, &datas[0])
         <<BUTTONLABELSTRUCT((char*)"2", callback, &datas[2])
         <<BUTTONLABELSTRUCT((char*)"3", callback, &datas[3])
         <<BUTTONLABELSTRUCT((char*)"4", callback, &datas[4])
         <<BUTTONLABELSTRUCT((char*)"5", callback, &datas[5])
         <<BUTTONLABELSTRUCT((char*)"6", callback, &datas[6])
         <<BUTTONLABELSTRUCT((char*)"7", callback, &datas[7])
         <<BUTTONLABELSTRUCT((char*)"8", callback, &datas[8])
         <<BUTTONLABELSTRUCT((char*)"Quit", quit, NULL);

  // add some buttons for the first table
  Table table(2, 2); table.show();
  table<<buttons.grab(1)<<buttons.grab(2)<<buttons.grab(3)<<buttons.grab(4);

    // example of a manually placed table
  Table table2(2,2); table2.show();
  table2.setOptions((GtkAttachOptions)(GTK_FILL|GTK_EXPAND), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND));
//  table2<<(int[4]){0, 1, 1, 2}<<buttons.grab(5); // bottom left
//  table2<<(int[4]){1, 2, 0, 1}<<buttons.grab(6); // top right
//  table2<<(int[4]){0, 1, 0, 1}<<buttons.grab(7); // top left
//  table2<<(int[4]){1, 2, 1, 2}<<buttons.grab(8); // bottom right
  (table2<<0, 1, 1, 2)<<buttons.grab(5); // bottom left
  (table2<<1, 2, 0, 1)<<buttons.grab(6); // top right
  (table2<<0, 1, 0, 1)<<buttons.grab(7); // top left
  (table2<<1, 2, 1, 2)<<buttons.grab(8); // bottom right

  HBox hBox; // create an HBox

  // put the first table, then the quit button, then the second table into the HBox
  hBox<< BoxIS(true,true, true)<<table.getWidget()<< BoxIS(false,false, true)<<buttons.grab(buttons.getCount())<< BoxIS(true,true, true)<<table2.getWidget();

  hBox.show(); // show the HBox
  topWindow << hBox; // Load the HBox into the top window

  gtk_main(); // run GTK+
}
