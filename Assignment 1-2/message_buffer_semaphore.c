#include "message_buffer_semaphore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

int shmid;
void *memory_segment=NULL;

int semid;
union semun sem_union;

void init_sem() {
    // Create semaphore
    if ((semid = semget(SEM_KEY, 1, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        // try as a client
        if ((semid = semget(SEM_KEY, 0, 0)) == -1) return;
    } else {
        sem_union.val = 1;
        semctl(semid, 0, SETVAL, sem_union);
    }

    printf("init semid : %d\n", semid);
}

void destroy_sem() {
    // Destroy semaphore
    if (semctl(semid, 0, IPC_RMID) == -1) {
        printf("semctl error!\n");
        return;
    }

    printf("destroy semid : %d\n", semid);
}

void s_wait() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    semid = semget(SEM_KEY, 0, 0);

    if (semop(semid, &buf, 1) == -1) {
        printf("<s_wait> semop error!\n");
	return;
    }
}

void s_quit() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    semid = semget(SEM_KEY, 0, 0);

    if (semop(semid, &buf, 1) == -1) {
        printf("<s_quit> semop error!\n");
	return;
    }
}


int init_buffer(MessageBuffer **buffer){
    
    // Get a shared memory segment
    shmid = shmget(SHM_KEY, sizeof(MessageBuffer), IPC_CREAT | 0666);
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

int attach_buffer(MessageBuffer **buffer){

    // Get the shared memory segment
    shmid = shmget(SHM_KEY, sizeof(MessageBuffer), 0666);
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

int destroy_buffer(){
    s_wait();
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("shmctl error!\n\n");
        s_quit();
        return -1;
    }

    printf("destroy shared_memory\n\n");
    s_quit();
    return 0;
}

int produce(MessageBuffer **buffer, int sender_id, int data, int account_id){
    s_wait();

    (*buffer)->messages[0].sender_id = sender_id;
    (*buffer)->messages[0].data += data;
    (*buffer)->account_id = account_id;
    (*buffer)->is_empty = 0; 
    printf("produce message\n");

    s_quit();
    return 0;
}

int consume(MessageBuffer **buffer, Message **message){
    s_wait();

    if ((*buffer)->is_empty == 1) {
        // Buffer is empty, cannot consume a message
        s_quit();
        return -1;
    }

    // Consume the message
    *message = &((*buffer)->messages[0]);
    (*buffer)->is_empty = 1; // Mark buffer as empty
    printf("consume message\n");
    
    s_quit();
    return 0;
}
