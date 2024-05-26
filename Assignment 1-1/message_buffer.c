#include "message_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int shmid;
void *memory_segment=NULL;

int init_buffer(MessageBuffer **buffer) {
    
    // Get a shared memory segment
    shmid = shmget(KEY, sizeof(MessageBuffer), IPC_CREAT | 0666);
    if (shmid < 0) {
        printf("shmget error!\n\n");
        return -1;
    }

    // Attach the shared memory segment to our process's address space
    memory_segment = shmat(shmid, NULL, 0);
    if (memory_segment == (void *)-1) {
        printf("shmat error!\n\n");
        return -1;
    }

    *buffer = (MessageBuffer *)memory_segment;

    // Initialize the buffer fields
    (*buffer)->is_empty = 1;
    (*buffer)->account_id = -1;
    memset((*buffer)->messages, 0, sizeof((*buffer)->messages));

    printf("init buffer\n");
    return 0;
}

int attach_buffer(MessageBuffer **buffer) {
    
    // Get the shared memory segment
    shmid = shmget(KEY, sizeof(MessageBuffer), 0666);
    if (shmid < 0) {
        printf("shmget error!\n\n");
        return -1;
    }

    // Attach the shared memory segment to our process's address space
    memory_segment = shmat(shmid, NULL, 0);
    if (memory_segment == (void *)-1) {
        printf("shmat error!\n\n");
        return -1;
    }

    *buffer = (MessageBuffer *)memory_segment;
    printf("attach buffer\n");
    printf("\n");
    return 0;
}

int detach_buffer() {
    if (shmdt(memory_segment) == -1) {
        printf("shmdt error!\n\n");
        return -1;
    }

    printf("detach buffer\n\n");
    return 0;
}

int destroy_buffer() {
    if(shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("shmctl error!\n\n");
        return -1;
    }

    printf("destroy shared_memory\n\n");
    return 0;
}

int produce(MessageBuffer **buffer, int sender_id, int data, int account_id) {
    
    // Produce the message
    (*buffer)->messages[0].sender_id = sender_id;
    (*buffer)->messages[0].data += data;
    (*buffer)->account_id = account_id;
    (*buffer)->is_empty = 0; 

    printf("produce message\n");

    return 0;

}

int consume(MessageBuffer **buffer, Message **message) {
    
    if ((*buffer)->is_empty == 1) {
        // Buffer is empty, cannot consume a message
        return -1;
    }

    // Consume the message
    *message = &((*buffer)->messages[0]);
    
    (*buffer)->is_empty = 1; // Mark buffer as empty

    printf("consume message\n");

    return 0;
}
