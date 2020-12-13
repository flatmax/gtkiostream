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
/* Compile with ...
g++ -ggdb `gtk-config --cflags` ButtonsTest.C -o ButtonsTest `gtk-config --libs`
`pkg-config --cflags --libs gtk+-2.0`
*/
#include "Window.H"
#include "Buttons.H"
#include "MessageDialog.H"
#include "Box.H"

static void run(void *wid, gpointer data) {
    GtkWidget *win=(GtkWidget*)data;
{MessageInfo messageInfo(win, (char *)"info");
messageInfo.title((char *)"this is the title");
messageInfo.run();}
{MessageError messageError(win, (char *)"Error");
messageError.run();}
{MessageQuestion messageQuestion(win, (char *)"Question");
messageQuestion.run();}
{
string txt("Warning");
MessageWarning messageWarning(win, txt);
messageWarning.run();}
    gtk_main_quit();
}

static void runPango(void *wid, gpointer data) {
    GtkWidget *win=(GtkWidget*)data;
    Pango pango; pango<< Font((char *)"Sans 12") << ColourLineSpec((char *)"r") <<(char *) " Sans font size 12 text"<<Font((char *)"")<<(char *)" and this is  not formatted";
MessageInfo messageInfo(win, pango);
messageInfo.title((char *)"this is the title");
messageInfo.run();
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv ); // init GTK

    Window topWindow; // Create the top box

    Buttons buttons;
    HBox hBox;

    //Pango pango; pango<<"this is the message";

    //messageDialog<<pango;
    //MessageDialog messageDialog("text");
//    messageDialog.show();

//cout<<"here"<<endl;
//    topWindow<< messageDialog;
buttons<<BUTTONLABELSTRUCT((char *)"Run", run, topWindow.getWidget())<<BUTTONLABELSTRUCT((char *)"Run with Pango", runPango, topWindow.getWidget());
    hBox<<buttons;
    hBox.show();
    topWindow<<hBox;
    gtk_main();
}
