#include <iostream>
#include <cstring>
#include <cstdlib>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */

using namespace std;
const int MAX_SIZE_STR = 100;

int main (int argc, char *argv[])
{
    char* input_str;
    int input_num;
    input_str = argv[1];
    input_num = atoi(argv[2]);

    MPI_Init(NULL,NULL); // start MPI initialization

    int comm_sz;
    int my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank !=0)
    {
        MPI_Recv(input_str, MAX_SIZE_STR, MPI_CHAR,(my_rank-1),0,MPI_COMM_WORLD,NULL);
        MPI_Recv(&input_num, 1, MPI_INT,(my_rank-1),0,MPI_COMM_WORLD,NULL);
        cout << "Received by Process " << my_rank << ":" << input_str<< ","<< input_num<<" from Process: "<<(my_rank-1)<<endl;

        string temp_str  = std::to_string(my_rank);
        char cat_str[2];
        strcpy(cat_str,temp_str.c_str());
        strcat(input_str,cat_str);
        //cout<<"........"<<input_str<< " ";

        ++input_num;
    }


    if(my_rank<=comm_sz-2)
    {
        MPI_Send(input_str, strlen(input_str)+1, MPI_CHAR,(my_rank + 1),0,MPI_COMM_WORLD);
        MPI_Send(&input_num, 1, MPI_INT,(my_rank + 1),0,MPI_COMM_WORLD);
    }
    else if (my_rank == comm_sz-1)
    {
        MPI_Send(input_str, strlen(input_str)+1, MPI_CHAR,0,0,MPI_COMM_WORLD);
        MPI_Send(&input_num, 1, MPI_INT,0,0,MPI_COMM_WORLD);
    }

    if(my_rank == 0)
    {

        MPI_Recv(input_str, MAX_SIZE_STR, MPI_CHAR,(comm_sz-1),0,MPI_COMM_WORLD,NULL);
        MPI_Recv(&input_num, 1, MPI_INT,(comm_sz-1),0,MPI_COMM_WORLD,NULL);
        cout << "Received by Process " << my_rank << ":" << input_str<< ","<< input_num<<" from Process: "<<(comm_sz-1)<<endl;

        string temp_str  = std::to_string(my_rank);
        char cat_str[2];
        strcpy(cat_str,temp_str.c_str());
        strcat(input_str,cat_str);
        //cout<<"........"<<input_str<< " ";

        ++input_num;
        cout << "Modified by Process " << my_rank << ":" << input_str<< ","<< input_num<<endl;

    }
    MPI_Finalize();

}
