#include <iostream>
#include <omp.h>

using namespace std;

int main (int argc, char *argv[])
{
    // Compute the runtime of the program.
    double sTime = omp_get_wtime();

    if (argc < 2)
    {
        cerr << "enter thread count like: ./program <No. of Thread>" << endl;
        return -1;
    }

    int input_thread_count = atoi(argv[1]); // thread count input from cmd line

    //parallel region block
    #pragma omp parallel num_threads(input_thread_count)
    {
        int my_rank, num_threads;
        my_rank = omp_get_thread_num(); //current thread
        num_threads = omp_get_num_threads(); // total threads

        /*
        #pragma omp critical
        {
            if(my_rank == 0) //At the thread number of 0, it will be displayed
                cout << "No of thread spawned: " << num_threads << endl;

            cout << "Hello from thread # " << my_rank + 1 << " out of " << num_threads << " thread(s)." << endl;
        }
        */
        cout << "Hello from thread # " << my_rank << " out of " << num_threads << " thread(s)." << endl;
    }

    // Displaying the code run time
    double eTime = omp_get_wtime();

    cout << "Time Required: " << (eTime - sTime)<<endl;
}
