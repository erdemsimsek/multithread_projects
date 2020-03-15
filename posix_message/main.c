#include <stdio.h>
#include <mqueue.h>
#include <sys/fcntl.h>
#include <stdbool.h>

#define MESSAGE_SIZE        sizeof(int)
#define QUEUE_NAME          "test"

static mqd_t queue;

void sendMessageToQueue(int val);
void receiveMessageToQueue(int * val);

void msg_rx_handle(int sig_num){
    printf("Received signal is %d\n", sig_num);
}

int main() {

//    struct mq_attr attr;
//    attr.mq_flags = 0;
//    attr.mq_msgsize = MESSAGE_SIZE;
//    attr.mq_maxmsg = 200;
//
//    queue = mq_open("test", O_RDWR | O_CREAT, 0664, &attr );
//    if(queue == (-1)){
//        printf("Queue creation is unsuccessful\n");
//    }
//
//    mq_close(queue);
//    mq_unlink("test");

    size_t ind = 0;
    int val;
    for(ind = 0; ind < 100; ind++){
        sendMessageToQueue(ind);
        receiveMessageToQueue(&val);
        printf("Received Value %d\n", val);
    }

    return 0;
}

void sendMessageToQueue(int val){
    static bool isInit = false;
    static mqd_t mq;
    if(!isInit){
        mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY);
        if(mq == (-1)){
            printf("Queue Creation Error \n");
        }
        isInit = true;
    }

    if(mq_send(mq, &val, MESSAGE_SIZE, 0) != 0){
        printf("Queue Send Error \n");
    }
}

void receiveMessageToQueue(int * val){
    static bool isInit = false;
    static mqd_t  mq;;

    if(!isInit){
        mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY);
        if(mq == (-1)){
            printf("Queue Creation Error\n");
        }
        isInit = true;
    }

    if(mq_receive(mq, val, MESSAGE_SIZE, NULL) != 0){
        printf("Queue Receive Error\n");
    }
}