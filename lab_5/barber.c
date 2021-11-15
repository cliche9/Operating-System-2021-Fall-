/* 
 * Filename : barber.c 
 * Function : 建立并模拟理发师进程 
 */ 

#include "ipc.h"
int main(int argc, char *argv[]) {
    int rate;    
    if(argv[1] != NULL)
        rate = atoi(argv[1]);
    else 
        rate = 3;
    
    Msg_buf msg_arg;
    // 建立room队列, 可容纳13人
    sofa_quest_flg = IPC_CREAT | 0644;
    sofa_quest_key = 111;
    sofa_quest_id = set_msq(sofa_quest_key, sofa_quest_flg);
    sofa_respond_flg = IPC_CREAT | 0644;
    sofa_respond_key = 112;
    sofa_respond_id = set_msq(sofa_respond_key, sofa_respond_flg);
    // 建立barber队列, 可容纳4人
    barber_quest_flg = IPC_CREAT | 0644;
    barber_quest_key = 211;    
    barber_quest_id = set_msq(barber_quest_key, barber_quest_flg);    
    barber_respond_flg = IPC_CREAT | 0644;
    barber_respond_key = 212;    
    barber_respond_id = set_msq(barber_respond_key, barber_respond_flg);    
    // 建立顾客信号量
    customer_key = 311;    
    sem_flg = IPC_CREAT | 0644;    
    sem_val = 0;    
    customer_sem = set_sem(customer_key, sem_val, sem_flg);    
    // 建立账本信号量
    account_key = 312;    
    sem_flg = IPC_CREAT | 0644;
    sem_val = 1;    
    account_sem = set_sem(account_key, sem_val, sem_flg);

    int pid[2];
    pid[0] = fork();
    if (pid[0] == 0) {
        // barber 1
        while (1) {
            int bpid = getpid();
            printf("barber 1 is sleeping\n");
            // printf("barber %d is sleeping\n", bpid);
            // 设置阻塞状态
            barber_quest_flg = 0;
            // 此时msgtype可以 = 0, 只接收第一条信息
            if (msgrcv(barber_quest_id, &msg_arg, sizeof(msg_arg), 0, barber_quest_flg) >= 0) {
                // quest from sofa message queue     
                // msgflg = 0, 阻塞状态          
                msgsnd(barber_respond_id, &msg_arg, sizeof(msg_arg), 0);
                // printf("barber %d is serving customer %d\n", bpid, msg_arg.mid);
                printf("barber 1 is serving customer %d\n", msg_arg.mid);
                sleep(rate);
                // 账本信号量互斥             
                sem_wait(account_sem);                
                // printf("barber %d is charging customer %d\n", bpid, msg_arg.mid);
                printf("barber 1 is charging customer %d\n", msg_arg.mid);
                sem_signal(account_sem);
            } else {
                // printf("barber %d is sleeping\n", bpid);
                printf("barber 1 is sleeping\n");
            }
        }    
    } else {
        pid[1] = fork();
        if (pid[1] == 0) {
            // barber 2
            while (1) {
                int bpid = getpid(); 
                printf("barber 2 is sleeping\n");          
                // printf("barber %d is sleeping\n", bpid);
                barber_quest_flg = 0;                
                if (msgrcv(barber_quest_id, &msg_arg, sizeof(msg_arg), 0, barber_quest_flg) >= 0) {
                    // quest from sofa message queue                
                    msgsnd(barber_respond_id, &msg_arg, sizeof(msg_arg), 0);
                    // printf("barber %d is serving customer %d\n", bpid, msg_arg.mid);
                    printf("barber 2 is serving customer %d\n", msg_arg.mid);
                    sleep(rate);                
                    sem_wait(account_sem);
                    // printf("barber %d is charging customer %d\n", bpid, msg_arg.mid); 
                    printf("barber 2 is serving customer %d\n", msg_arg.mid);
                    sem_signal(account_sem);                
                } else {
                    // printf("barber %d is sleeping\n", bpid);
                    printf("barber 2 is sleeping\n");
                }
            }
        } else {
            while (1) {
                // barber 3
                int bpid = getpid();
                // printf("barber %d is sleeping\n", bpid);
                printf("barber 3 is sleeping\n");
                barber_quest_flg = 0;                
                if (msgrcv(barber_quest_id, &msg_arg, sizeof(msg_arg), 0, barber_quest_flg) >= 0) {
                    // quest from sofa message queue
                    msgsnd(barber_respond_id, &msg_arg, sizeof(msg_arg), 0);
                    // printf("barber %d is serving for customer %d\n", bpid, msg_arg.mid);
                    printf("barber 3 is serving for customer %d\n", msg_arg.mid);
                    sleep(rate);               
                    sem_wait(account_sem);
                    // printf("barber %d is charging customer %d\n", bpid, msg_arg.mid);
                    printf("barber 3 is charging customer %d\n", msg_arg.mid);
                    sem_signal(account_sem);                
                } else {
                    // printf("barber %d is sleeping\n", bpid);
                    printf("barber 3 is sleeping\n");
                }
            }
        }
    }

    return EXIT_SUCCESS;
}