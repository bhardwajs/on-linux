#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)

const char* shared_mem_path="/my-mmmap";



int
main()
{

    size_t total_size = sizeof(size_t) * 2; /* lock and counter*/
    size_t *shared_mem = NULL;

    int fd = shm_open(shared_mem_path, O_CREAT |  O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) errExit("shm_open");


    shared_mem = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_mem == MAP_FAILED) errExit("mmap");

    for(;;)
    {
        while(0 == __sync_bool_compare_and_swap (shared_mem, 0, 1))
        {
            sleep(1);
        }
            // get counter
            size_t counter = *(shared_mem + 1);

            printf("counter set to %lu \n", counter);

            // release lock
            *shared_mem  = 0;
            sleep(1);
    }

    return 0;
}
