#include <iostream>
#include <mpi.h>
#include <cstring>
#include <cstdlib>
#include "mpi_bcast.h"
#include <cmath>
#include <ctime>

using namespace std;
const int SIZE = 10000;

int main (int argc, char *argv[])
{
    int rootP;
    double sTime, eTime, totalTime, localTime;

    double libSTime, libETime, libTotalTime, libLocalTime;
    //mat_size = atoi(argv[1]);
    // size of matrix
    rootP = atoi(argv[1]);
    cout<<"Root Process: "<< rootP<<endl;
    cout<< "Array Size: " << SIZE<<endl;
    double* my_arr = new double[SIZE];
    double* lib_arr = new double[SIZE];

    MPI_Init(&argc,&argv); // start MPI initialization

    int comm_sz;
    int my_rank;

    MPI_Comm comm;
    //MPI_Status status;

    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    // file<< "Number of Process: " << comm_sz<<endl;
    cout<< "Number of Process: " << comm_sz<<endl;

    /* ------------------Using My Defined Boradcast Function ---------------------*/
    MPI_Barrier(comm);
    //sTime =  MPI_Wtime();

    sTime = MPI_Wtime();
    if(my_rank == rootP)
    {
        //double arr_element = 3.0;
        // generate double randomly

        //double range = 100.0-1.0;

        for(int i=0; i<SIZE; i++)
        {
            double div = RAND_MAX / 100.0;
            double arr_element = (rand()/div) ;
            my_arr[i] = arr_element;
        }

        /*------------------ Print Input (debug purpose)----------------------*/
/*
        cout<<"Input done"<<endl;
        cout<<"==========Array==========="<<endl;
        for(int i=0; i<SIZE; i++)
        {
            cout << my_arr[i] << " ";
        }
        cout<<endl;
        */


    }

    //sTime =  MPI_Wtime();
    my_Bcast(my_arr,SIZE,MPI_DOUBLE,rootP,comm);
    MPI_Barrier(comm);

    eTime = MPI_Wtime();

    totalTime = eTime - sTime;

    /*-------------DEDUG purpose-------------------------*/

    if(my_rank != rootP)
    {
        cout<<"Process: "<< my_rank << " received array from root process: "<< rootP<<endl;
        for(int i=0; i<10; i++)
        {
            cout << my_arr[i] << " ";
        }
        cout<<endl;
    }

    //eTime =  MPI_Wtime();
    //localTime = eTime-sTime;
    //MPI_Reduce(&localTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, comm);


    /* ------------------Using Library MPI_Bcast Function ---------------------*/
    MPI_Barrier(comm);
    //libSTime =  MPI_Wtime();

    libSTime = MPI_Wtime();
    if(my_rank == rootP)
    {
        //double arr_element = 3.0;
        // generate double randomly

        //double range = 100.0-1.0;

        for(int i=0; i<SIZE; i++)
        {
            double div = RAND_MAX / 100.0;
            double lib_arr_element = (rand()/div) ;
            lib_arr[i] = lib_arr_element;
        }

        /*------------------ Print Input (debug purpose)----------------------*/
/*
        cout<<"Input done"<<endl;
        cout<<"==========Array==========="<<endl;
        for(int i=0; i<SIZE; i++)
        {
            cout << my_arr[i] << " ";
        }
        cout<<endl;
        */


    }
    MPI_Bcast(lib_arr,SIZE,MPI_DOUBLE,rootP,comm);
    MPI_Barrier(comm);
    libETime = MPI_Wtime();

    libTotalTime = libETime - libSTime ;
    /*-------------DEDUG purpose-------------------------*/
    /*
    if(my_rank != rootP)
    {
        cout<<"Process: "<< my_rank << " received array from root process: "<< rootP<<endl;
        for(int i=0; i<SIZE; i++)
        {
            cout << my_arr[i] << " ";
        }
        cout<<endl;
    }
*/
    //libETime =  MPI_Wtime();
   // libLocalTime = libETime-libSTime;
    //MPI_Reduce(&libLocalTime, &libTotalTime, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    /* ------------ Execution Time (seconds) --------------*/

    if(my_rank==rootP)
    {
        cout << "Nodes: 2 "<<endl;
        cout<<"========================================================"<<endl;
        cout << "My Bcast Method Time Required: " << totalTime<<endl;
        cout<<"---------------------------------------------------------"<<endl;
        cout << "Library MPI_Bcast Function Time Required: " << libTotalTime<<endl;
        cout<<"========================================================"<<endl;
        //file << "\nParallel Time Required: " << totalTime<<endl;
    }


    /* free memory */
    delete[] my_arr;
    delete[] lib_arr;
    MPI_Finalize();


    //file <<"=============================================="<<endl;
    //file.close();
}
