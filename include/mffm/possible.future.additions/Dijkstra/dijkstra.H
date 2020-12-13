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
#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

class Dijkstra {
    int *visited;
    int **pathLengths;
    int **roadLengths;
    int **lastLoc;
    int N;

    void dumpArrays(int **roadLengths, int Nin) {
        N=Nin;
        cout<<"Visited = [";
        for (int i=0; i<N; i++)
            cout<<visited[i]<<", ";
        cout<<"]"<<endl;
        cout<<"a\tb\tc\td\te\tf\tg\th\ti\tj";
        cout<<"\t || ";
        cout<<"a\tb\tc\td\te\tf\tg\th\ti\tj"<<endl;
        cout<<"1\t2\t3\t4\t5\t6\t7\t8\t9\t10";
        cout<<"\t || ";
        cout<<"1\t2\t3\t4\t5\t6\t7\t8\t9\t10"<<endl;
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++)
                cout<<pathLengths[i][j]<<'\t';
            cout<<" || ";
            for (int j=0; j<N; j++)
                cout<<lastLoc[i][j]<<'\t';
            cout<<'\n';
        }

        cout<<'\n'<<endl;
    }

public:

    Dijkstra(int Nin) {
        pathLengths=NULL;
        roadLengths=NULL;
        lastLoc=NULL;
        visited=NULL;
        N=Nin;
        cout<<"N "<<N<<endl;
        pathLengths = new int*[N];
        if (!pathLengths) {
            cerr<<"couldn't create path lengths"<<endl;
            return;
        }
        for (int i=0; i<N; i++)
            pathLengths[i]=NULL;

        for (int i=0; i<N; i++) {
            pathLengths[i] = new int[N];
            if (!pathLengths[i]) {
                cerr<<"couldn't create path lengths"<<endl;
                return;
            }
        }
        roadLengths = new int *[N];
        if (!roadLengths) {
            cerr<<"couldn't create road lengths"<<endl;
            return;
        }
        for (int i=0; i<N; i++)
            roadLengths[i]=NULL;

        for (int i=0; i<N; i++) {
            roadLengths[i] = new int[N];
            if (!roadLengths[i]) {
                cerr<<"couldn't create road lengths"<<endl;
                return;
            }
        }

        lastLoc = new int *[N];
        if (!lastLoc) {
            cerr<<"couldn't create last lengths"<<endl;
            return;
        }
        for (int i=0; i<N; i++)
            lastLoc[i]=NULL;

        for (int i=0; i<N; i++) {
            lastLoc[i] = new int[N];
            if (!lastLoc[i]) {
                cerr<<"couldn't create last lengths"<<endl;
                return;
            }
        }

        visited = new int [N];
        if (!visited) {
            cerr<<"couldn't create visited"<<endl;
            return;
        }
    }

    ~Dijkstra(void) {
        if (pathLengths)
            for (int i=0; i<N; i++) {
                if (pathLengths[i]) {
                    delete [] pathLengths[i];
                    pathLengths[i]=NULL;
                }
            }
        if (pathLengths)
            delete [] pathLengths;
        pathLengths=NULL;


        if (roadLengths)
            for (int i=0; i<N; i++) {
                if (roadLengths[i]) {
                    delete [] roadLengths[i];
                    roadLengths[i]=NULL;
                }
            }
        if (roadLengths)
            delete [] roadLengths;
        roadLengths=NULL;

        if (lastLoc)
            for (int i=0; i<N; i++) {
                if (lastLoc[i]) {
                    delete [] lastLoc[i];
                    lastLoc[i]=NULL;
                }
            }
        if (lastLoc)
            delete [] lastLoc;
        lastLoc=NULL;

        if (visited)
            delete [] visited;
        visited=NULL;
    }

    void setVisited(int *vis) {
        for (int i=0; i<N; i++)
            visited[i] = vis[i];
    }
    void setPathLengths(int pl, int i, int j) {
        pathLengths[i][j]=pl;
    }
    void setRoadLengths(int pl, int i, int j) {
        roadLengths[i][j]=pl;
    }
    void setLastLoc(int pl, int i, int j) {
        lastLoc[i][j]=pl;
    }

    void findShortestPath() {
        // breadth first search
        unsigned int lastLength;
        for (int j=0; j<N; j++) // initialise the first path lengths
            pathLengths[0][j]=roadLengths[0][j];
        visited[0]=1;
        for (int i=1; i<N; i++) {
            for (int j=0; j<N; j++) // initialise the current path lengths
                pathLengths[i][j]=pathLengths[i-1][j];
            visited[i]=1;
            lastLength=pathLengths[i-1][i];
            for (int j=0; j<N; j++)
                if (!visited[j] & i!=j) {
                    if (lastLength+roadLengths[i][j]<pathLengths[i-1][j]) {
                        pathLengths[i][j]=lastLength+roadLengths[i][j];
                        lastLoc[i][j]=i;
                    }
                } else
                    visited[i] =0;
            dumpArrays(roadLengths, N);
        }
    }
};
#endif // DIJKSTRA_H_
