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

//#define DEBUG_OUTPUT // define this to print out Debugger information

#include <sched.h>
#include "OptionParser.H"

#include "IIO/IIOMMap.H"
#include <values.h>

//#define FP_TYPE unsigned short int ///< The type to be used with sox input for file output
#include "Sox.H"

#include <iomanip>

int printUsage(string name, int N, string chip, int chCnt, float T, float fs, int periodCount) {
    cout<<name<<" : An application to stream input from IIO devices to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] outFileName"<<endl;
    cout<<"\t -p : The number of samples to read each time from the IIO devices : (-p "<<N<<")"<<endl;
    cout<<"\t -C : The name of the chip to look for in the available IIO devices : (-C "<<chip<<")"<<endl;
    cout<<"\t -i : The number of channels to open, if the available number is less, then it is reduced to the available : (-i "<<chCnt<<")"<<endl;
    cout<<"\t -t : The duration to sample for : (-t "<<T<<")"<<endl;
    cout<<"\t -f : The sample rate to use : (-f "<<fixed<<setprecision(2)<<fs<<")"<<endl;
    cout<<"\t -n : The number of periods : (-n "<<periodCount<<")"<<endl;
    cout<<resetiosflags(ios::showbase);
    Sox<float> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
    struct sched_param param;
    param.sched_priority = 96;
    if (sched_setscheduler(0, SCHED_FIFO, & param) == -1) {
        perror("sched_setscheduler");
        return -1;
    }

    // defaults
    int N=2048; // the number of samples per read
    string chip("AD7476A"); // the chip to search for
    int chCnt=MAXINT;
    float T=1.; // seconds
    float fs=1.e6; // sample rate in Hz
    int periodCount=3;

    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, chip, chCnt, T, fs, periodCount);
    if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, chip, chCnt, T, fs, periodCount);
    if (argc<2)
        return printUsage(argv[0], N, chip, chCnt, T, fs, periodCount);

    if (op.getArg<int>("p", argc, argv, N, i=0)!=0)
        ;

    if (op.getArg<string>("C", argc, argv, chip, i=0)!=0)
        ;

    if (op.getArg<int>("i", argc, argv, chCnt, i=0)!=0)
        ;

    if (op.getArg<float>("t", argc, argv, T, i=0)!=0)
        ;

    if (op.getArg<float>("f", argc, argv, fs, i=0)!=0)
        ;

    if (op.getArg<int>("n", argc, argv, periodCount, i=0)!=0)
        ;

    int M=T*1e6/N;

    IIOMMap iio;
    iio.findDevicesByChipName(chip);

    iio.printInfo(); // print out detail about the devices which were found ...

    if (iio.getChCnt()<chCnt)
        chCnt=iio.getChCnt();

    //iio.sampleRate=fs;

    cout<<"Number of samples p="<<N<<"\nNumber of channels available i="<<chCnt<<"\nChip name C="<<chip<<endl;
    cout<<"Reading n="<<periodCount<<" period buffers of p samples each"<<endl;
    cout<<"Duration t="<<T<<"\nSample rate f="<<fs<<endl;
    cout<<endl;

    cout<<"Reading "<<M<<" times "<<N<<" samples, resulting in a processing time of "<<M*N/1e6<<" or "<<M*N<<" samples per channel."<<endl;

    int colCnt=(int)ceil((float)chCnt/(float)iio[0].getChCnt()); // check whether we require less then the available number of channels

// use this if there are problems writing to file gradually
//    // Collect all samples in memory first using the data matrix.
//    Eigen::Matrix<unsigned short, Eigen::Dynamic, Eigen::Dynamic> dataStore;
//    dataStore.resize(M*N*iio[0].getChCnt(), colCnt);

    int expectedWriteCnt=N*chCnt;
//    Debugger << "opening"<<endl;
    int ret;
    if ((ret=iio.open(periodCount, N))!=NO_ERROR) // try to open all devices
        return ret;

  //  Debugger << "get read array"<<endl;
    Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> data;
    ret=iio.getReadArray(N, data); // resize the array to be able to read enough memory
    if (ret!=NO_ERROR)
        return ret;
    if (data.cols()>colCnt) // resize the data columns to match the specified number of columns
        data.resize(data.rows(), colCnt);

    float maxDelay=iio.getMaxDelay(fs);
    cout<<"maxDelay = "<<maxDelay<<endl;

    // open sox
    Sox<float> sox;
    ret=sox.openWrite(argv[argc-1], fs, colCnt, MAXSHORT);
    if (ret!=NO_ERROR)
        return ret;

    struct timespec start, stop, lockStart, lockStop;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        cout<<"clock start get time error"<<endl;
        exit(-1);
    }

    double durations[M];
    for (int i=0; i<M; i++) {
    //    Debugger << "read loop i="<<i<<endl;

        if( clock_gettime( CLOCK_REALTIME, &lockStart) == -1 ) {
            cout<<"clock lockStart get time error"<<endl;
            exit(-1);
        }

        if (i==0){
        //    Debugger << "enabling"<<endl;
            if ((ret=iio.enable(true))!=NO_ERROR) // start the DMA
                return ret;
        }

        ret=iio.read(N, data);
        if (ret!=NO_ERROR) {
            cout<<"error with i="<<i<<" out of M="<<M<<" loops."<<endl;
            break;
        }

        //dataStore.block(i*N*iio[0].getChCnt(), 0, N*iio[0].getChCnt(), data.cols())=data; // use this if there is problems writing to file gradually.
        int written=sox.write(data);
        if (written!=expectedWriteCnt) {
            if (written>0)
                cout<<"Attempted to write "<<N<<" samples (per channel) to the audio file, however only "<<written<<" samples were written. Exiting!"<<endl;
            else {
                cout<<SoxDebug().evaluateError(written)<<endl;
                cout<<"Output matrix size (rows, cols) = ("<<data.rows()<<", "<<data.cols()<<")"<<endl;
                cout<<"Error writing, exiting."<<endl;
            }
            break;
        }

        if( clock_gettime( CLOCK_REALTIME, &lockStop) == -1 ) {
            cout<<"clock lockStop get time error"<<endl;
            exit(-1);
        }
        durations[i] = 1.e3*( lockStop.tv_sec - lockStart.tv_sec ) + (double)( lockStop.tv_nsec - lockStart.tv_nsec )/1.e6;
        if (durations[i]>maxDelay*1.e3)
            cout<<"Safe lock duration of "<<maxDelay<<" exceeded with a lag of "<<durations[i]<<" ms, possible sample drop.\n";
    }

    iio.enable(false); // stop the DMA
    cout<<"closing the devices"<<endl;
    iio.close();

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        cout<<"clock stop get time error"<<endl;
        exit(-1);
    }


    sox.closeWrite();

    ofstream outputd("/tmp/durations.txt");
    for (int i=0; i<M; i++)
        outputd<<durations[i]<<'\n';
    outputd.close();

    double duration = 1.e3*( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.e6;
    cout<<"Duration "<<duration<<" ms\n";

    double expected = (float)(N*M)/1.e6*1.e3;
    cout<<"Expected "<<expected<<" ms\n";

    cout<<"Difference "<<duration-expected<<" ms"<<endl;


    return 0;
}

