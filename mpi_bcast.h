#if !defined MY_BCAST_H
#define MY_BCAST_H

// Including libraries
#include <iostream>
#include <mpi.h>
#include <cstring>
#include <cstdlib>

// Signature of the methods
void my_Bcast(
    void* data,
    int total,
    MPI_Datatype datatype,
    int root,
    MPI_Comm communicator);

#endif
