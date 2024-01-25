#include <pthread.h>
#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include <semaphore.h>
// �������
int shared_variable = 0;
// �ź���
sem_t sem2;  // �����̲߳���˳�����
// �̺߳���
void *thread_function1(void *thread_id) {
    int i;
    for (i = 0; i < 100000; i++) {
        shared_variable++;
    }
    sem_post(&sem2); // �ͷ�sem2�������߳�2ִ��
    pthread_exit(NULL);
}
void *thread_function2(void *thread_id) {
    int i;
    sem_wait(&sem2); // �ȴ�sem2��ȷ���߳�1��ִ��
    for (i = 0; i < 100000; i++) {
        shared_variable--;
    }
    pthread_exit(NULL);
}
int main() {
    pthread_t thread1, thread2;
    // ��ʼ���ź���
    sem_init(&sem2, 0, 0);
    // �����������߳�
    if (pthread_create(&thread1, NULL, thread_function1, (void *)1) == 0) {
        printf("Thread 1 created successfully.\n");
    } else {
        printf("Thread 1 creation failed.\n");
    }
    if (pthread_create(&thread2, NULL, thread_function2, (void *)2) == 0) {
        printf("Thread 2 created successfully.\n");
    } else {
        printf("Thread 2 creation failed.\n");
    }
    // �ȴ��߳̽���
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // �����ź���
    sem_destroy(&sem2);
    printf("Final shared_variable value: %d\n", shared_variable);
    return 0;
}
