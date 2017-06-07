/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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

// compile from the top directory (gtkiostream) with :
// g++ -g -I include -I /usr/include/eigen3 -I include/IIO src/IIO/IIO.C src/IIO/IIODevice.C test/IIOTest.C -o test/IIOTest

#include "IIO/IIO.H"
#include "OptionParser.H"
#include <values.h>

#include <sched.h>
#include "Sox.H"

int printUsage(string name, int N, float T) {
    cout<<name<<" : An application to stream input from IIO devices to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] outFileName"<<endl;
    cout<<"\t -N : The number of samples to read each time from the IIO devices : (-N"<<N<<")"<<endl;
    cout<<"\t -t : The duration to sample for : (-t"<<T<<")"<<endl;

    return 0;
}

int main(int argc, char *argv[]) {

    int N=2048;
    int targetTime=40; // seconds

    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, targetTime);
    if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, targetTime);
    if (argc<2)
        return printUsage(argv[0], N, targetTime);

    if (op.getArg<int>("N", argc, argv, N, i=0)!=0)
        ;

    if (op.getArg<int>("t", argc, argv, targetTime, i=0)!=0)
        ;


    struct sched_param param;
    param.sched_priority = 96;
    if (sched_setscheduler(0, SCHED_FIFO, & param) == -1) {
        perror("sched_setscheduler");
        return -1;
    }

    int M=targetTime*1e6/N;
    cout<<"N="<<N<<", M="<<M<<" resulting in a processing time of "<<M*N/1e6<<endl;

    string chipName("AD7476A"); // we are looking for devices on the IIO system with the "AD7476A" chip
    IIO iio;
    iio.findDevicesByChipName(chipName);

    iio.printInfo(); // print out detail about the devices which were found ...

    int ret=iio.open(); // try to open all devices
    if (ret!=NO_ERROR)
        exit(ret);

    Eigen::Matrix<unsigned short, Eigen::Dynamic, Eigen::Dynamic> dataStore;
    dataStore.resize(M*N*2, 2);

    // first generate a matrix big enough to hold the data we will request
    Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> data;
    ret=iio.getReadArray(N, data); // resize the array to be able to read enough memory
    if (ret!=NO_ERROR)
        exit(ret);

//    iio.setChannelBufferCnt(data.rows()*2*2); // ensure we are double buffering (so to speak)

    ret=iio.enable(true); // try to enable all devices
    if (ret!=NO_ERROR)
        exit(ret);

    // try to read from the devices.


    struct timespec start, stop, readStart, readStop, cycleDuration, outReadStart, outReadStop;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        cout<<"clock start get time error"<<endl;
        exit(-1);
    }
    cycleDuration.tv_sec = start.tv_sec;
    cycleDuration.tv_nsec = start.tv_nsec;


    for (int i=0; i<M; i++) {
        usleep(3000);
        if( clock_gettime( CLOCK_REALTIME, &readStart) == -1 ) {
            cout<<"clock start get time error"<<endl;
            exit(-1);
        }
        double duration = 1.e3*( readStart.tv_sec - readStop.tv_sec ) + (double)( readStart.tv_nsec - readStop.tv_nsec )/1.e6;
        cout<<"Outside read duration "<<duration<<" ms\n";

        ret=iio.read(N, data);
        if (ret!=NO_ERROR) {
            cout<<"error with i="<<i<<" out of M="<<M<<" loops."<<endl;
            break;
        }
        if( clock_gettime( CLOCK_REALTIME, &readStop) == -1 ) {
            cout<<"clock start get time error"<<endl;
            exit(-1);
        }

        duration = 1.e3*( readStop.tv_sec - readStart.tv_sec ) + (double)( readStop.tv_nsec - readStart.tv_nsec )/1.e6;
        cout<<"Read duration "<<duration<<" ms\n";

        duration = 1.e3*( readStop.tv_sec - cycleDuration.tv_sec ) + (double)( readStop.tv_nsec - cycleDuration.tv_nsec )/1.e6;
        cout<<"\t\t\t\t\tCycle duration "<<duration<<" ms\n";
        cycleDuration.tv_sec = readStop.tv_sec;
        cycleDuration.tv_nsec = readStop.tv_nsec;

        dataStore.block(i*N*2, 0, N*2, 2)=data;
        if( clock_gettime( CLOCK_REALTIME, &readStop) == -1 ) {
            cout<<"clock start get time error"<<endl;
            exit(-1);
        }

    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        cout<<"clock stop get time error"<<endl;
        exit(-1);
    }

    double duration = 1.e3*( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.e6;
    cout<<"Duration "<<duration<<" ms\n";

    double expected = (float)(N*M)/1.e6*1.e3;
    cout<<"Expected "<<expected<<" ms\n";

    cout<<"Difference "<<duration-expected<<" ms\n";

    iio.close();

    // open sox
    float fs=1.e6; // sample rate in Hz
    Sox<float> sox;
    ret=sox.openWrite(argv[argc-1], fs, dataStore.cols(), MAXSHORT);
    if (ret!=NO_ERROR)
        return ret;
    int written=sox.write(dataStore);
    sox.closeWrite();

    cout<<"output written to "<<argv[argc-1]<<endl;
//    ofstream output("/tmp/data.txt");
//    output<<dataStore;
//    output.close();

    return 0;
}
