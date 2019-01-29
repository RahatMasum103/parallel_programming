#include <iostream>
#include <omp.h>
#include <cmath>

using namespace std;
int main(int argc, char *argv[])
{
    int arr_size;

    cout<<"Enter number of elements in array"<< endl;
    cin>>arr_size;
    int* my_arr = new int[arr_size];
    int arr_element;

    cout<<"Enter array elements"<< endl;
    for(int i=0; i<arr_size; i++)
    {
        cin>>arr_element;
        my_arr[i]=arr_element;
    }
    cout<<"Input done"<<endl;
    for(int i=0; i<arr_size; i++)
    {
        cout<<my_arr[i]<<" ";
    }
    cout<<endl;

    double sTime = omp_get_wtime();
    int input_thread_count = atoi(argv[1]); // thread count input from cmd line
    if (argc < 2)
    {
        cerr << "enter thread count like: ./program <No. of Thread>" << endl;
        return -1;
    }
    //parallel region block
    int arr_part = ceil(arr_size / input_thread_count);

    #pragma omp parallel num_threads(input_thread_count)
    {
        int my_rank, num_threads;
        my_rank = omp_get_thread_num(); //current thread
        num_threads = omp_get_num_threads(); // total threads

        #pragma omp critical
        {
            int start_index = my_rank * arr_part;
            int end_index = (start_index + arr_part)-1;

            cout << "Thread Number: " << my_rank << ", start index: " << start_index << ", end index: " << end_index << endl;

            for(int i = start_index; i <= end_index; i++)
            {
                cout << my_arr[i] <<" ";
            }
            cout<<endl;
        }

    }
    double eTime = omp_get_wtime();
    cout << "\nTime Required: " << (eTime - sTime)<<endl;
}
