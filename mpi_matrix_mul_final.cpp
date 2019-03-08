#include <iostream>
#include <cstdlib>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

int main (int argc, char *argv[])
{
   // ofstream file;
    //file.open ("mpi_matrix_output.txt",ios::out | ios::app);
    double sTime, eTime, totalTime, localTime;

    int mat_size;
    mat_size = atoi(argv[1]);
     // size of matrix
    //file<< "Matrix Size: " << mat_size<<endl;
    cout<< "Matrix Size: " << mat_size<<endl;

    MPI_Init(&argc,&argv); // start MPI initialization

    double* A = new double[mat_size*mat_size];
    double* B = new double[mat_size*mat_size];
    double* C = new double[mat_size*mat_size];

    int comm_sz;
    int my_rank;

    MPI_Comm comm;
    //MPI_Status status;

    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    int mat_part = (mat_size*mat_size) / comm_sz;

   // file<< "Number of Process: " << comm_sz<<endl;
    cout<< "Number of Process: " << comm_sz<<endl;
    //file<< "Chunk Size: " << mat_part<<endl;
    cout<< "Chunk Size: " << mat_part<<endl;

    sTime =  MPI_Wtime();
    if(my_rank == 0)
    {
/*
        double mat_elementA = 3.0;
        double mat_elementB = 2.0;
        double res_element = 0.0;
*/
        // generate double randomly

        //double range = 100.0-1.0;
        double div = RAND_MAX / 100.0;
        double mat_elementA = (rand()/div) ;
        double mat_elementB = (rand()/div) ;
        double res_element = 0.0;


        for(int i=0; i<(mat_size * mat_size); i++)
        {
            A[i] = mat_elementA;
            B[i] = mat_elementB;
            C[i] = res_element;
        }

        // Print Input (debug purpose)
/*
        cout<<"Input done"<<endl;

        cout<<"==========A Matrix==========="<<endl;
        for(int i=0; i<mat_size * mat_size; i++)
        {
              cout << A[i] << " ";
        }
        cout<<endl;
        cout<<"==========B Matrix==========="<<endl;
        for(int i=0; i<mat_size * mat_size; i++)
        {
              cout << B[i] << " ";
        }
        cout<<endl;
        */

        /*--------------- Send B to all the Process ---------*/

        for(int i=1; i<comm_sz; i++)
        {
            MPI_Send(B, (mat_size * mat_size), MPI_DOUBLE, i,0,comm);
        }

        /*--------------- Distribute A with row partition to all the Process ---------*/
        for(int i=1; i<comm_sz; i++)
        {
            int index = i*mat_part;
            MPI_Send(&A[index], mat_part, MPI_DOUBLE, i,0,comm);
        }
    }
    if(my_rank !=0)
    {
        /*------------- Collect B as a whole to all the process -------------------*/
        MPI_Recv(B, (mat_size * mat_size), MPI_DOUBLE, 0,0,comm,NULL);

        /*------------- Collect distributed part/chunk of A to all the process -------------------*/

        MPI_Recv(A, mat_part, MPI_DOUBLE, 0,0,comm,NULL);
/*
        cout<<"[DEBUG]Process no. "<<my_rank<<endl;
        for(int i=0; i<mat_part; i++)
        {
            cout << A[i] << " ";
        }
        cout<<endl;
*/
    }

    /* ---------------- Matrix A * B = C -------------------------*/
    for(int i=0; i<(mat_part/mat_size); i++) /* (chunk/dimension) */
    {
        for(int j=0; j<mat_size; j++)
        {
            for(int k=0; k<mat_size; k++)
            {
                C[i * mat_size + j] += A[i * mat_size + k] * B[k * mat_size + j];
                //cout << "C[" << i << "*" << mat_size << "+" << j << "] += A[" << i << "*" << mat_size << "+" << k << "] * B[" << k <<"*"<< mat_size <<"+"<< j<<"]" << endl;
            }
        }
    }

    /*-----------Send calculated result from each process TO Master (Process 0)------------- */
    if(my_rank !=0)
    {
        MPI_Send(C, mat_part, MPI_DOUBLE,0,my_rank,MPI_COMM_WORLD);
    }

    /*-----------Collect calculated result from each process INTO Master (Process 0)------------- */
    if(my_rank == 0)
    {
        for (int i=1; i<comm_sz; i++)
        {
            MPI_Recv(&C[i*mat_part], mat_part, MPI_DOUBLE, i,i,MPI_COMM_WORLD,NULL);
        }
/*
        cout<<"[DEBUG]==========Result Matrix==========="<<endl;
        for(int i=0; i<mat_size*mat_size; i++)
        {
            cout << C[i] << " ";
        }
        cout<<endl;
*/
    }
    eTime =  MPI_Wtime();
    localTime = eTime-sTime;
    MPI_Reduce(&localTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    /* ------------ Execution Time (seconds) --------------*/

    if(my_rank==0)
    {
        cout << "\nParallel Time Required: " << totalTime<<endl;
        //file << "\nParallel Time Required: " << totalTime<<endl;
    }

    /* free memory */
    delete[] A;
    delete[] B;
    delete[] C;

    MPI_Finalize();


    //file <<"=============================================="<<endl;
    //file.close();
}
