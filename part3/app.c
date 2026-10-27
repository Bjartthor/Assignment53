#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


void *writerTask(void *param)
{
    char out_buffer[100];
    int fd = open("/dev/mydev1", O_WRONLY); // write only
    int variable_byte = 10;
    while (1)
    {
        variable_byte = variable_byte + 10;
        if (variable_byte > 100)
        {
            variable_byte = 10;
        }

        ssize_t ret = write(fd, out_buffer, variable_byte);
        sleep(1); 
    }
    close(fd);
    return NULL;
}

int main(void)
{
    pthread_t writeTaskObj;
    pthread_create(&writeTaskObj, NULL, writerTask, NULL);
    pthread_join(writeTaskObj, NULL);
    return 0;
}

