#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;
int main(int argc, char *argv[])
{
    int mat_size;

    ofstream file;
    file.open ("matrix_output.txt",ios::out | ios::app);

    double start_serial_time = omp_get_wtime();

    double** my_matrixA;
    double** my_matrixB;
    double** multiply_matrix;
    double** multiply_matrix_parallel;

    //Matrix INPUT Creation
    //cout<<"Enter size of matrix"<< endl;
    //cin>>mat_size;

    int input_thread_count = atoi(argv[1]); // thread count input from cmd line
    file<< "Thread count: "<< input_thread_count<<endl;
    cout<< "Thread count: "<< input_thread_count<<endl;

    mat_size = atoi(argv[2]); // size of matrix
    file<< "Matrix Size: " << mat_size<<endl;
    cout<< "Matrix Size: " << mat_size<<endl;

    if (argc < 3)
    {
        cerr << "enter thread count like: ./program <No. of Thread> <Matrix Size>" << endl;
        return -1;
    }


    my_matrixA = new double*[mat_size];
    my_matrixB = new double*[mat_size];
    multiply_matrix = new double*[mat_size];
    multiply_matrix_parallel = new double*[mat_size];

    for (int i=0; i<mat_size; ++i)
    {
        my_matrixA[i] = new double[mat_size];
        my_matrixB[i] = new double[mat_size];
        multiply_matrix[i] = new double[mat_size];
        multiply_matrix_parallel[i] = new double[mat_size];
    }

    // Populate Matrix
    //double mat_elementA = 3.0;
   // double mat_elementB = 2.0;
    double res_element = 0.0;


    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            // generate double randomly

            //double range = 100.0-1.0;
            double div = RAND_MAX / 100.0;
            double mat_elementA = (rand()/div) ;
            double mat_elementB = (rand()/div) ;

            my_matrixA[i][j] = mat_elementA;
            my_matrixB[i][j] = mat_elementB;
            multiply_matrix[i][j] = res_element;
            multiply_matrix_parallel[i][j] = res_element;
        }
    }

    double end_serial_time = omp_get_wtime();
    cout << "\nSerial Time Required: " << (end_serial_time - start_serial_time)<<endl;
    file << "\nSerial Time Required: " << (end_serial_time - start_serial_time)<<endl;

    // Print Input (debug purpose)
    cout<<"Input done"<<endl;

    /*
    cout<<"==========A Matrix==========="<<endl;
    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            cout << my_matrixA[i][j] << " ";
        }
        cout <<endl;
    }
    cout<<"==========B Matrix==========="<<endl;
    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            cout << my_matrixB[i][j] << " ";
        }
        cout <<endl;
    }
    */
/*
    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            for(int k=0; k<mat_size; k++)
            {
                multiply_matrix[i][j] += my_matrixA[i][k] * my_matrixB[k][j];
            }
        }
    }

    cout<<"Multiplication done (Serial)"<<endl;
    cout<<"==========Result Matrix==========="<<endl;
    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            cout << multiply_matrix[i][j] << " ";
        }
        cout <<endl;
    }
    cout<<endl;
*/


    //parallel region block
    int mat_part = ceil(mat_size / input_thread_count);
    double sTime = omp_get_wtime();
    #pragma omp parallel num_threads(input_thread_count)
    {
        int my_rank, num_threads;
        my_rank = omp_get_thread_num(); //current thread
        num_threads = omp_get_num_threads(); // total threads

        int start_index = my_rank * mat_part;
        int end_index = (start_index + mat_part);
        //cout << "Thread Number: " << my_rank << ", start index: " << start_index << ", end index: " << end_index << endl;

        for(int i=start_index; i<end_index; i++)
        {
            for(int j=0; j<mat_size;j++)
                {
                    for(int k=0; k<mat_size; k++)
                    {
                        multiply_matrix_parallel[i][j] += my_matrixA[i][k] * my_matrixB[k][j];
                    }
                }
        }
    }

    double eTime = omp_get_wtime();
    cout << "\nParallel Time Required: " << (eTime - sTime)<<endl;
    file << "\nParallel Time Required: " << (eTime - sTime)<<endl;
    file <<"===================================="<<endl;
    file.close();

    cout<<"Multiplication done (Parallel)"<<endl;


    cout<<"==========Result Matrix==========="<<endl;
    for(int i=0; i<mat_size; i++)
    {
        for(int j=0; j<mat_size;j++)
        {
            cout << multiply_matrix_parallel[i][j] << " ";
        }
        cout <<endl;
    }
    cout<<endl;

    for(int i = 0; i < mat_size; i++)
	{
	    free(my_matrixA[i]);
        free(my_matrixB[i]);
        free(multiply_matrix[i]);
        free(multiply_matrix_parallel[i]);
		//free(ptr[i]);
	}
	free(my_matrixA);
	free(my_matrixB);
	free(multiply_matrix);
	free(multiply_matrix_parallel);


}

