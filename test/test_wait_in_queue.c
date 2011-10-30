#include "fiber_manager.h"
#include "test_helper.h"

#define NUM_THREADS 2
#define NUM_FIBERS 100
#define PER_FIBER_COUNT 1000

mpsc_fifo_t fifo;

void* run_function(void* param)
{
    int i;
    for(i = 0; i < PER_FIBER_COUNT; ++i) {
        fiber_manager_wait_in_queue(fiber_manager_get(), &fifo);
    }
    return NULL;
}

int main()
{
    fiber_manager_set_total_kernel_threads(NUM_THREADS);
    mpsc_fifo_init(&fifo);

    fiber_t* fibers[NUM_FIBERS];
    int i;
    for(i = 0; i < NUM_FIBERS; ++i) {
        fibers[i] = fiber_create(20000, &run_function, NULL);
    }

    fiber_yield();

    for(i = 0; i < PER_FIBER_COUNT; ++i) {
        fiber_manager_wake_from_queue(fiber_manager_get(), &fifo, NUM_FIBERS);
    }

    for(i = 0; i < NUM_FIBERS; ++i) {
        fiber_join(fibers[i], NULL);
    }

    mpsc_fifo_destroy(&fifo);

    return 0;
}
