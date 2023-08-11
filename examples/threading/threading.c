#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    usleep(thread_func_args->wait_to_obtain_ms * 1000); // Convert to microseconds for usleep

    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to lock the mutex");
        thread_func_args->thread_complete_success = false;
        return thread_func_args;
    }

    usleep(thread_func_args->wait_to_release_ms * 1000); // Convert to microseconds for usleep

    if (pthread_mutex_unlock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to unlock the mutex");
        thread_func_args->thread_complete_success = false;
        return thread_func_args;
    }

    thread_func_args->thread_complete_success = true;
    return thread_func_args;
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    struct thread_data *thread_data_ptr = malloc(sizeof(struct thread_data));
    if (!thread_data_ptr) {
        ERROR_LOG("Failed to allocate memory for thread_data");
        return false;
    }

    thread_data_ptr->mutex = mutex;
    thread_data_ptr->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data_ptr->wait_to_release_ms = wait_to_release_ms;
    thread_data_ptr->thread_complete_success = false; // Initialize to false

    if (pthread_create(thread, NULL, threadfunc, thread_data_ptr) != 0) {
        ERROR_LOG("Failed to create thread");
        free(thread_data_ptr); // Don't forget to free allocated memory in case of error
        return false;
    }

    return true;
}

