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

// compile from the top directory (gtkiostream) with :
// g++ -g -I include -I /usr/include/eigen3 -I include/IIO src/IIO/IIO.C src/IIO/IIODevice.C test/IIOTest.C -o test/IIOTest

#include <sched.h>
#include "OptionParser.H"

#include "IIO/IIOThreadedQ.H"
#include <values.h>

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
    float T=40.; // seconds
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

    IIOThreadedQ iio;
    iio.findDevicesByChipName(chip);

    iio.printInfo(); // print out detail about the devices which were found ...

    iio.BlockBuffer::resize(periodCount); // resize to the correct number of periods

    if (iio.getChCnt()<chCnt)
        chCnt=iio.getChCnt();

    iio.sampleRate=fs;

    cout<<"Number of samples p="<<N<<"\nNumber of channels available i="<<chCnt<<"\nChip name C="<<chip<<endl;
    cout<<"Reading n="<<periodCount<<" period buffers of p samples each"<<endl;
    cout<<"Duration t="<<T<<"\nSample rate f="<<fs<<endl;
    cout<<endl;

    cout<<"Reading "<<M<<" times "<<N<<" samples, resulting in a processing time of "<<M*N/1e6<<" or "<<M*N<<" samples per channel."<<endl;

    int colCnt=(int)ceil((float)chCnt/(float)iio[0].getChCnt()); // check whether we require less then the available number of channels

    // open sox
    Sox<float> sox;
    int ret=sox.openWrite(argv[argc-1], fs, colCnt, MAXSHORT);
    if (ret!=NO_ERROR)
        return ret;

    Eigen::Matrix<unsigned short, Eigen::Dynamic, Eigen::Dynamic> data;
    data.resize(M*N*2, colCnt);

    ret=iio.setSampleCountChannelCount(N, chCnt);
    if (ret!=NO_ERROR)
        return ret;
    int expectedWriteCnt=N*chCnt;

    if ((ret=iio.open())!=NO_ERROR) // try to open all devices
        return ret;

    if ((ret=iio.run())!=NO_ERROR) { // start the reading thread
        iio.enable(false); // stop the DMA
        return ret;
    }


    if ((ret=iio.enable(true))!=NO_ERROR) // start the DMA
        return ret;

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
//    // first generate a matrix big e            cout<<"data.block x,y = "<<i*N*2<<","<<0<<" rows,cols = "<<N*2<<","<<2<<'\n';
//            cout<<"b rows,cols = "<<b->rows()<<","<<b->cols()<<'\n';

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

        iio.lock(); // wait for the reading thread to produce a new buffer.
        while (!iio.newbufReady)
            iio.wait();
        iio.newbufReady=false; // inidcate that the buffer has been emptied
        iio.unLock();

        Eigen::Array<short unsigned, Eigen::Dynamic, Eigen::Dynamic> *b=iio.getFullBuffer(); // get an full buffer

        if (!b){ // check whether there were any available buffers
                cout<<"main : Error : couldn't get a valid full buffer\n";
                usleep(1000); // sleep for a ms
        } else {
//            cout<<"data.block x,y = "<<i*N*2<<","<<0<<" rows,cols = "<<N*2<<","<<2<<'\n';
//            cout<<"b rows,cols = "<<b->rows()<<","<<b->cols()<<'\n';
            data.block(i*N*2, 0, N*2, b->cols())=*b;
            iio.putEmptyBuffer(b); // put the empty buffer back on the empty queue now that we have finished with it.
        }

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
