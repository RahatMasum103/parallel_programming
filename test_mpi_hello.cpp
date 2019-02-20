#include <iostream>
#include <cstring>
#include <cstdlib>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */

using namespace std;
const int MAX_SIZE_STR = 100;

int main (int argc, char *argv[])
{
    char* input_str;
    input_str = argv[1];
    static int input_num = atoi(argv[2]);

    cout<<"message: "<< input_str << " number: "<<input_num << endl;

    int comm_sz;
    int my_rank;
    MPI_Init(NULL,NULL); // start MPI initialization

    MPI_Request request = MPI_REQUEST_NULL;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

    ++input_num;
    if(my_rank<=comm_sz-2)
    {
        //++input_num;
        MPI_Isend(input_str, strlen(input_str)+1, MPI_CHAR,(my_rank + 1),0,MPI_COMM_WORLD, &request);
        //MPI_Send(input_str, strlen(input_str)+1, MPI_CHAR,(my_rank + 1),0,MPI_COMM_WORLD);
        cout<<"......"<<my_rank<<" "<<input_num<<endl;;
        MPI_Isend(&input_num, 1, MPI_INT,(my_rank + 1),0,MPI_COMM_WORLD,&request);

    }
    else if (my_rank == comm_sz-1)
    {
        //++input_num;
        MPI_Isend(input_str, strlen(input_str)+1, MPI_CHAR,0,1,MPI_COMM_WORLD,&request);
        cout<<"......"<<my_rank<<" "<<input_num<<endl;
        MPI_Isend(&input_num, 1, MPI_INT,0,1,MPI_COMM_WORLD,&request);
        //++input_num;
    }
    if(my_rank !=0)
    {
        MPI_Irecv(input_str, MAX_SIZE_STR, MPI_CHAR,(my_rank-1),0,MPI_COMM_WORLD,&request);
        MPI_Irecv(&input_num, 1, MPI_INT,(my_rank-1),0,MPI_COMM_WORLD,&request);
        cout << "Received by Process " << my_rank << ": " << input_str<<" from Process: "<<(my_rank-1)<<endl;
        cout << "message: "<<input_str<<" number: "<< input_num<<endl;
    }
    else if(my_rank == 0)
    {
        MPI_Irecv(input_str, MAX_SIZE_STR, MPI_CHAR,(comm_sz-1),1,MPI_COMM_WORLD,&request);
        MPI_Irecv(&input_num, 1, MPI_INT,(comm_sz-1),1,MPI_COMM_WORLD,&request);
        cout << "Received by Process " << my_rank << ": " << input_str<<" from Process: "<<(comm_sz-1)<<endl;
        cout << "message: "<<input_str<<" number: "<< input_num<<endl;
    }

    MPI_Finalize();

   // return 0;

}
