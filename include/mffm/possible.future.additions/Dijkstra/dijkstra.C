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
// g++ dijkstra.C -o dijkstra

#include <iostream>
using namespace std;

#include <math.h>

#include "dijkstra.H"

int main(int argc, char *argv[]){

    int N=10;
                            // A B  C  D  E  F  G  H  I  J
    int roadLength[10][10]={{0, 2, 1, 0, 0, 0, 0, 0, 0, 0}, // A
                             {2, 0, 0, 5, 3, 0, 0, 0, 0, 0}, // B
                             {1, 0, 0, 4, 0, 0, 0, 0, 0, 0}, // C
                             {0, 5, 4, 0, 0, 7, 8, 0, 0, 0}, // D
                             {0, 3, 0, 0, 0, 6, 0, 0, 0, 0}, // E
                             {0, 0, 0, 7, 6, 0, 0, 10, 0, 0}, // F
                             {0, 0, 0, 8, 0, 0, 0, 9, 0, 0}, // G
                             {0, 0, 0, 0, 0, 10, 9, 0, 11, 13}, // H
                             {0, 0, 0, 0, 0, 0, 0, 11, 0, 12}, // I
                             {0, 0, 0, 0, 0, 0, 0, 13, 12, 0} // J
                            };

    int visited[10]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int pathLengths[N][N];
    int lastLoc[N][N];

    for (int i=0;i<N; i++)
        for (int j=0;j<N; j++){
            pathLengths[i][j]=99;
            lastLoc[i][j]=11;
            if (roadLength[i][j]==0)
                roadLength[i][j]=99;
            cout<<roadLength[i][j]<<endl;
        }

    Dijkstra dijkstra(N);
    dijkstra.setVisited((int *)visited);
       for (int i=0; i<N; i++)
            for (int j=0; j<N;j++)
                dijkstra.setPathLengths(pathLengths[i][j], i , j);
       for (int i=0; i<N; i++)
            for (int j=0; j<N;j++)
                dijkstra.setRoadLengths(roadLength[i][j], i, j);
       for (int i=0; i<N; i++)
            for (int j=0; j<N;j++)
                dijkstra.setLastLoc(lastLoc[i][j], i , j);

    dijkstra.findShortestPath();

    return 0;
}
