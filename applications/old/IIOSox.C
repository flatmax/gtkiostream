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

#include "OptionParser.H"

#include "IIO/IIOThreaded.H"
#include <values.h>

#define FP_TYPE unsigned short int ///< The type to be used with sox input for file output
#include "Sox.H"

#include <iomanip>

int printUsage(string name, int N, string chip, int chCnt, float T, float fs) {
    cout<<name<<" : An application to stream input from IIO devices to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] outFileName"<<endl;
    cout<<"\t -p : The number of samples to read each time from the IIO devices : (-p"<<N<<")"<<endl;
    cout<<"\t -C : The name of the chip to look for in the available IIO devices : (-C "<<chip<<")"<<endl;
    cout<<"\t -i : The number of channels to open, if the available number is less, then it is reduced to the available : (-i"<<chCnt<<")"<<endl;
    cout<<"\t -t : The duration to sample for : (-t"<<T<<")"<<endl;
    cout<<"\t -f : The sample rate to use : (-f"<<fixed<<setprecision(2)<<fs<<")"<<endl;
    cout<<resetiosflags(ios::showbase);

    Sox sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
    // defaults
    int N=2048; // the number of samples per read
    string chip("AD7476A"); // the chip to search for
    int chCnt=MAXINT;
    float T=40.; // seconds
    float fs=1.e6; // sample rate in Hz

    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, chip, chCnt, T, fs);
    if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
        return printUsage(argv[0], N, chip, chCnt, T, fs);
    if (argc<2)
        return printUsage(argv[0], N, chip, chCnt, T, fs);

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

    int M=T*1e6/N;

    IIOThreaded iio;
    iio.findDevicesByChipName(chip);

    iio.printInfo(); // print out detail about the devices which were found ...

    if (iio.getChCnt()<chCnt)
        chCnt=iio.getChCnt();

    cout<<"Number of samples p="<<N<<"\nNumber of channels available i="<<chCnt<<"\nChip name C="<<chip<<endl;
    cout<<"Duration t="<<T<<"\nSample rate f="<<fs<<endl;
    cout<<endl;

    cout<<"Reading "<<M<<" times "<<N<<" samples, resulting in a processing time of "<<M*N/1e6<<" or "<<M*N<<" samples per channel."<<endl;

    // open sox
    Sox sox;
    int ret=sox.openWrite(argv[argc-1], fs, chCnt/iio[0].getChCnt(), MAXSHORT);
    if (ret!=NO_ERROR)
        return ret;

    Eigen::Matrix<unsigned short, Eigen::Dynamic, Eigen::Dynamic> data;
    data.resize(M*N*2, 2);

    ret=iio.setSampleCountChannelCount(N, chCnt);
    if (ret!=NO_ERROR)
        return ret;
    int expectedWriteCnt=N*chCnt;

    if ((ret=iio.open())!=NO_ERROR) // try to open all devices
        return ret;

    if ((ret=iio.enable(true))!=NO_ERROR) // start the DMA
        return ret;

    if ((ret=iio.run())!=NO_ERROR) { // start the reading thread
        iio.enable(false); // stop the DMA
        return ret;
    }

//    if ((ret=iio.unLock())!=NO_ERROR) { // ensure started in an unlocked state
//        iio.enable(false); // stop the DMA
//        iio.stop(); // stop the reading thread
//        return ret;
//    }

    struct timespec start, stop, lockStart, lockStop;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        cout<<"clock start get time error"<<endl;
        exit(-1);
    }

//    if ((ret=iio.lock())!=NO_ERROR) { // lock so I can't read until unlocked.
//        iio.enable(false); // stop the DMA
//        iio.stop(); // stop the reading thread
//        return ret;
//    }

    // try to read from the devices.
//
//    // first generate a matrix big enough to hold the data we will request
//    Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> data;
//    ret=iio.getReadArray(N, data); // resize the array to be able to read enough memory
//    if (ret!=NO_ERROR)
//        ;
//    else {
//

//        cout<<" spent "<< (driver->engine->get_microseconds()-driver->debug_last_time)<<" us waiting for lock\n";
    double durations[M];
    for (int i=0; i<M; i++) {
        if( clock_gettime( CLOCK_REALTIME, &lockStart) == -1 ) {
            cout<<"clock lockStart get time error"<<endl;
            exit(-1);
        }
//
//        cout<<"mutex unlock 1\n";
//        iio.emptyRegionLock(false);
//            usleep(1000);
//        cout<<"mutex lock 1\n";
//        iio.lock(); // Wait until iio has finished reading this round
//        cout<<"mutex lock 1\n";
//        iio.lock(); // Wait until iio has finished reading this round
        //cout<<"mutex lock 2\n";
        //iio.lock(); // Wait until iio has finished reading this round
//        iio.emptyRegionLock(true);
//        iio.fillRegionLock();
//        iio.emptyRegionLock(false);


//        int written=sox.write(*iio.getFullBuffer());
//        if (written!=expectedWriteCnt) {
//            if (written>0)
//                cout<<"Attempted to write "<<N<<" samples (per channel) to the audio file, however only "<<written<<" samples were written. Exiting!"<<endl;
//            else {
//                cout<<SoxDebug().evaluateError(written)<<endl;
//                cout<<"Output matrix size (rows, cols) = ("<<iio.getFullBuffer()->rows()<<", "<<iio.getFullBuffer()->cols()<<")"<<endl;
//                cout<<"Error writing, exiting."<<endl;
//            }
//            break;
//        }

        iio.lock();
        while (!iio.bufFull)
            iio.wait();

        data.block(i*N*2, 0, N*2, 2)=*iio.getFullBuffer();

        iio.bufFull=false; // inidcate that the buffer has been emptied
        iio.unLock();

        //if (durations[i]<2000)
//            usleep(2000);

        if( clock_gettime( CLOCK_REALTIME, &lockStop) == -1 ) {
            cout<<"clock lockStop get time error"<<endl;
            exit(-1);
        }
        durations[i] = 1.e3*( lockStop.tv_sec - lockStart.tv_sec ) + (double)( lockStop.tv_nsec - lockStart.tv_nsec )/1.e6;
        cout<<"Lock duration "<<durations[i]<<" ms\n";

        //usleep(1000);

    }

    iio.enable(false); // stop the DMA
    iio.stop(); // stop the reading thread
    iio.close();

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        cout<<"clock stop get time error"<<endl;
        exit(-1);
    }

//    ofstream output("/tmp/data.txt");
//    output<<data;
//    output.close();
    int written=sox.write(data);

    ofstream outputd("/tmp/durations.txt");
    for (int i=0; i<M; i++)
        outputd<<durations[i]<<'\n';
    outputd.close();

    double duration = 1.e3*( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.e6;
    cout<<"Duration "<<duration<<" ms\n";

    double expected = (float)(N*M)/1.e6*1.e3;
    cout<<"Expected "<<expected<<" ms\n";

    cout<<"Difference "<<duration-expected<<" ms"<<endl;

//    }

    sox.closeWrite();

    return 0;
}

