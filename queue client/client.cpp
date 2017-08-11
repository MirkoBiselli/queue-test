/*
 * client.c: Client program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
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
#define CONTROL_QUEUE_PRIORITY 0

int main(int argc, char **argv)
{
    mqd_t _EventQueue;   // queue descriptors

    struct mq_attr attr;
    attr.mq_flags = 0; // blocking queue
    attr.mq_maxmsg = CONTROL_QUEUE_MAX_MESSAGES; // actually a number around 2-3 should be enough
    attr.mq_msgsize = CONTROL_QUEUE_MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((_EventQueue = mq_open(CONTROL_MESSAGE_QUEUE_NAME, O_RDWR | O_CREAT, CONTROL_QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("Client: mq_open (client)");
        exit(1);
    }

    appEventStructure_t* event = new (appEventStructure_t);
    event->actuatorStatus.group_id = 4;
    event->actuatorStatus.intensity = 98776554;

//    const char event[]="ciao";

    if (mq_send(_EventQueue, (char*) event, sizeof(event), CONTROL_QUEUE_PRIORITY) != 0)
    {
        perror("Client: mq_send");
        printf("%s: not able to send message - error: %s \n", __func__, strerror(errno));
        exit(1);
    }

    free(event);

    printf("Client: bye\n");

    exit(0);
}
