#include <iostream>
#include <cstring>
#include <cstdlib>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */
#include <omp.h>
#include <fstream>

using namespace std;


int main (int argc, char *argv[])
{
    int num_row, num_col, num_generation, thread_count, chunks;

    double sTime=0, eTime=0, totalTime=0;


    //input_str = argv[1];

    int comm_sz;
    int my_rank;

    MPI_Init(&argc,&argv);
    MPI_Comm comm;
    //MPI_Status status;

    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    cout<< "Total Processes: "<< comm_sz <<endl;




    int* grid_input;
    //int* grid_generation;
    int* my_chunk;


    thread_count = atoi(argv[1]);  // how many threads?
    num_generation = atoi(argv[2]); // how many iteration?

    cout<< "Total Threads: "<<thread_count<<endl;

    ifstream input_file(argv[3]);
    ofstream output_file(argv[4]);

    //input_file.open ("input_GoL.txt");

    input_file>> num_row >> num_col;
    //output_file.open("output_GoL.txt",std::ios_base::app);

    cout<< "Rows: "<< num_row << " " << "Columns: "<<num_col<<endl;
    output_file << num_row << " " << num_col<<endl;

    chunks = num_row/comm_sz; // each process get how much blocks or chunk of main grid

    int start_idx = my_rank * chunks;
    int end_idx;
    if(my_rank == comm_sz -1)
    {
        end_idx = num_row;
    }
    else
    {
        end_idx = start_idx + chunks;
    }

    if(my_rank !=0)
    {
        start_idx -= 1;
    }
    if(my_rank != comm_sz -1)
    {
        end_idx +=1;
    }

    int chunk_sz = end_idx - start_idx;

    if(my_rank == 0 || my_rank == comm_sz -1)
    {
        chunk_sz += 1;
    }
    grid_input = new int [num_row * num_row];
    my_chunk = new int [chunk_sz * num_row];

    sTime = MPI_Wtime();

    /*------------------Process 0 creating the Grid------------------*/
    if(my_rank ==0)
    {

        //grid_generation = new int [num_row * num_row];


        for (int i=0; i<num_row; i++)
        {
            for (int j=0; j<num_col; j++)
            {
                input_file >> grid_input[i * num_row + j]; // to make the 2d input into 1d array by jumping segments of column (j)
            }
        }

        cout<<"Input Done"<<endl;
        output_file << "Input Done"<<endl;

        output_file <<"========Input=======" <<endl;
        cout<<"Input Done"<<endl;
        /*
        for (int i=0; i<num_row; i++)
        {
            for (int j=0; j<num_col; j++)
            {
                cout<< grid_input[i * num_row + j] << " ";
                output_file << grid_input[i * num_row + j] << " "; // to make the 2d input into 1d array by jumping segments of column (j)
            }
            cout<< endl;
            output_file << endl;
        }
*/
        /*------------------- Divide the Total grid into Chunks-------------*/
        for(int i=0; i < chunk_sz * num_row; i++)
        {
            if(i-num_row < 0)
            {
                my_chunk[i] = grid_input[(i-num_row)+(num_row*num_row)];
            }
            else
            {
                my_chunk[i] = grid_input[(i-num_row)];
            }
        }
        /*--------------- Distribute the chunks to all processes----------*/
        int chunk_start, chunk_end;
        for(int s=1; s<comm_sz; s++)
        {
            chunk_start = s * chunks * num_row;
            if(s == comm_sz-1)
            {
                chunk_end = num_row * num_row;
            }
            else
            {
                chunk_end = chunk_start + chunks * num_row;
            }
            if(s!=0)
            {
                chunk_start -= num_row;
            }
            if(s!= comm_sz-1)
            {
                chunk_end += num_row;
            }
            int slice_limit = chunk_end - chunk_start;

            int* chunk_bcast;
            if(s == comm_sz-1)
            {
                slice_limit +=num_row;
                chunk_bcast = new int[slice_limit];

                for(int i=0; i<slice_limit; i++)
                {
                    if((i+chunk_start)>(num_row*num_row - 1))
                    {
                        chunk_bcast[i] = grid_input[(i+chunk_start)-(num_row*num_row)];
                    }
                    else
                    {
                        chunk_bcast[i] = grid_input[(i+chunk_start)];
                    }
                }
                MPI_Send(&chunk_bcast[0],slice_limit, MPI_INT, s,1,comm);

                delete [] chunk_bcast;
            }
            else
            {
                MPI_Send(&grid_input[chunk_start], slice_limit, MPI_INT, s, 1, comm);
            }
        }
    }
    else
    {
        MPI_Recv(my_chunk, chunk_sz*num_row, MPI_INT,0,1,comm,NULL);
        // DEBUG
        // cout << "process "<< my_rank << " first element " << my_chunk[0] << " last element " << my_chunk[chunk_sz*num_row - 1] << endl;
    }
    /*============== Grid Generation Calculation ======================*/
    for(int g=1; g<=num_generation; g++)
    {
        if(g>1)
        {
            if(my_rank !=comm_sz-1)
            {
                MPI_Send(&my_chunk[(chunk_sz-2)*num_row], num_row, MPI_INT, my_rank+1, 1,comm);
            }
            if(my_rank!=0)
            {
                MPI_Recv(&my_chunk[0], num_row, MPI_INT, my_rank-1, 1, comm, NULL);
            }
               if(my_rank == comm_sz-1)
            {
                MPI_Send(&my_chunk[(chunk_sz-2)*num_row],num_row, MPI_INT,0,1,comm);
            }
            if(my_rank == 0)
            {
                MPI_Recv(&my_chunk[0], num_row, MPI_INT, comm_sz-1, 1,comm,NULL);
            }


            if(my_rank!=0)
            {
                MPI_Send(&my_chunk[num_row], num_row, MPI_INT, my_rank-1,1,comm);
            }
            if(my_rank !=comm_sz-1)
            {
                MPI_Recv(&my_chunk[(chunk_sz-1)*num_row], num_row, MPI_INT, my_rank+1,1,comm,NULL);
            }
            if(my_rank == 0)
            {
                MPI_Send(&my_chunk[num_row],num_row, MPI_INT, comm_sz-1,1,comm);
            }
            if(my_rank == comm_sz-1)
            {
                MPI_Recv(&my_chunk[(chunk_sz-1)*num_row],num_row, MPI_INT,0,1,comm,NULL);
            }

        }

        int* updated_chunk = new int [chunk_sz * num_row];

        #pragma omp parallel num_threads(thread_count)
        {
            int alive_grid_element =0;
            int my_thread = omp_get_thread_num();
            int omp_chunk = (chunk_sz -2 )/ thread_count;
            int omp_chunk_start = my_thread * omp_chunk + 1;
            int omp_chunk_end;
            if(my_thread == thread_count -1)
            {
                omp_chunk_end = chunk_sz-1;
            }
            else
            {
                omp_chunk_end = omp_chunk_start + omp_chunk;
            }
            // DEBUG
            //cout<< "Thread: "<<my_thread<< " chunk start: "<< omp_chunk_start << " ,chunk end: " << omp_chunk_end<< endl;

            for(int i=omp_chunk_start; i<omp_chunk_end; i++)
            {
                for(int j=0; j<num_row; j++)
                {
                    // neighbour grid search
                    int left = my_chunk[i*num_row+((j-1+num_row)%num_row)];
                    int right = my_chunk[i*num_row+((j+1+num_row)%num_row)];
                    int top = my_chunk[(i-1)*num_row+j];
                    int bottom = my_chunk[(i+1)*num_row+j];
                    int leftTop = my_chunk[(i-1)*num_row + ((j-1+num_row)%num_row)];
                    int rightTop = my_chunk[(i-1)*num_row + ((j+1+num_row)%num_row)];
                    int leftBottom = my_chunk[(i+1)*num_row + ((j-1+num_row)%num_row)];
                    int rightBottom = my_chunk[(i+1)*num_row + ((j+1+num_row)%num_row)];
                    alive_grid_element = left + right + top + bottom + leftTop + rightTop + leftBottom + rightBottom;

                    if(my_chunk[i*num_row+j]==1 && (alive_grid_element ==2 || alive_grid_element ==3))
                    {
                        updated_chunk[i*num_row+j] = 1;
                    }
                    else if(my_chunk[i*num_row+j]==1 && alive_grid_element>3)
                    {
                        updated_chunk[i*num_row+j] = 0;
                    }
                    else if(my_chunk[i*num_row+j]==1 && alive_grid_element<2)
                    {
                        updated_chunk[i*num_row+j] = 0;
                    }
                    else if(my_chunk[i*num_row+j]==0 && alive_grid_element==3)
                    {
                        updated_chunk[i*num_row+j] = 1;
                    }
                    else
                    {
                        updated_chunk[i*num_row+j] = 0;
                    }
                }
            }
        }

        //update original Grid with new chunk data
        for(int i=1; i<chunk_sz-1; i++)
        {
            for(int j=0; j<num_row; j++)
            {
                my_chunk[i*num_row+j] = updated_chunk[i*num_row+j];
            }
        }
        delete [] updated_chunk;
    }

    /* =========Collect all data from other Processes to Process 0====== */
    if(my_rank == 0)
    {
        for(int i=0; i<chunk_sz-2; i++)
        {
            for(int j=0; j<num_row; j++)
            {
                grid_input[i*num_row+j]= my_chunk[(i*num_row)+j+num_row];
            }
        }

        for(int i=1; i <comm_sz; i++)
        {
            int slice_start = i*chunks*num_row;
            int slice_end;
            if(i==comm_sz-1)
            {
                slice_end = num_row*num_row;
            }
            else
            {
                slice_end = slice_start + (chunks * num_row);
            }
            int slice_sz = slice_end - slice_start;
            MPI_Recv(&grid_input[slice_start], slice_sz, MPI_INT, i, 1, comm, NULL);
        }
    }
    else
    {
        MPI_Send(&grid_input[num_row], (chunk_sz -2) * num_row, MPI_INT, 0,1, comm);
    }


    /*--------------------OUTPUT Show----------------------------*/

    if(my_rank ==0)
    {
        cout<<"========Generation Done=======" <<endl;
        output_file <<"========Generation=======" <<endl;

        for (int i=0; i<num_row; i++)
        {
            for (int j=0; j<num_row; j++)
            {
                cout<<grid_input[i * num_row + j] << " ";
                output_file << grid_input[i * num_row + j] << " "; // to make the 2d input into 1d array by jumping segments of column (j)
            }
            cout<<endl;
            output_file << endl;
        }

    }

    delete [] my_chunk;

    //delete [] grid_generation;

    eTime = MPI_Wtime();
    if(my_rank == 0)
    {
        totalTime = eTime - sTime;
        cout<< "Total time required: " << totalTime << endl;
        cout<< endl;
        output_file << "Total time required: " << totalTime << endl;
        output_file<< endl;

    }

    delete [] grid_input;

    //input_file.close();
    //output_file.close();

    MPI_Finalize();

}
