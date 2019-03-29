#include "mpi_bcast.h"

using namespace std;

void my_Bcast(
    void* data,
    int total,
    MPI_Datatype datatype,
    int root,
    MPI_Comm communicator
)
{
    int my_rank,world_size;
    MPI_Comm_rank(communicator, &my_rank);
    MPI_Comm_size(communicator, &world_size);
    if (my_rank == root)
    {
        // If we are the root process, send our data to everyone
        for (int i = 0; i < world_size; i++)
        {
            if (i != my_rank)
            {
                cout<<"Process: "<< my_rank << " broadcasting array "<<endl;
                MPI_Send(data, total, datatype, i, 0, communicator);
            }
        }
    }
    else
    {
        // If we are a receiver process, receive the data from the root
        cout<<"Process: "<< my_rank << " received array from root process: "<< root<<endl;
        MPI_Recv(data, total, datatype, root, 0, communicator, NULL);
    }
}
