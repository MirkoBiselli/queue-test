/* * server.c: Server program * to demonstrate interprocess commnuication
 * * with POSIX message queues */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

typedef struct appEvent {
    struct actuatorStatus_t {
        int group_id;
        int intensity;
    };

    union {
        actuatorStatus_t actuatorStatus;
    };
} appEventStructure_t;

#define CONTROL_MESSAGE_QUEUE_NAME "/control-app-queue"
#define CONTROL_QUEUE_PERMISSIONS 0660
#define CONTROL_QUEUE_MAX_MESSAGES 10 // actually a number around 2-3 should be enough
#define CONTROL_QUEUE_MAX_MSG_SIZE sizeof(appEventStructure_t)
#define CONTROL_QUEUE_BUFFER_SIZE (CONTROL_QUEUE_MAX_MSG_SIZE + 1) //must be greater than the mq_msgsize attribute of the queue

int main(int argc, char **argv)
{
    mqd_t _EventQueue;   // queue descriptors

    printf("Server: Hello, World!\n");

    struct mq_attr attr;
    attr.mq_flags = 0; // blocking queue
    attr.mq_maxmsg = CONTROL_QUEUE_MAX_MESSAGES; // actually a number around 2-3 should be enough
    attr.mq_msgsize = CONTROL_QUEUE_MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((_EventQueue = mq_open(CONTROL_MESSAGE_QUEUE_NAME, O_RDWR | O_CREAT, CONTROL_QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("Server: mq_open (server)");
        exit(1);
    }

//    appEventStructure_t eventMessage;
    appEventStructure_t* eventMessage = new (appEventStructure_t);
//    char* eventMessage = (char*) malloc(30);
//    char in_buffer [20];

    while (1)
    {
        // get the oldest message with highest priority
        if (mq_receive(_EventQueue, (char*) eventMessage, sizeof(eventMessage), NULL) == -1)
        {
            perror("Server: mq_receive");
            printf("%s: not able to receive message - error: %s \n", __func__, strerror(errno));
            exit(1);
        }

//                printf("eventMessage : %s \n", eventMessage);
        printf("eventMessage->actuatorStatus.group_id = %d \n", eventMessage->actuatorStatus.group_id);
        printf("eventMessage->actuatorStatus.intensity = %d \n", eventMessage->actuatorStatus.intensity);

//        free(eventMessage);

        printf("Server: message received.\n");
    }
}
