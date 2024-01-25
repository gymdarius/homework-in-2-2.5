#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main() {
    pid_t pid, pid1;
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    } else if (pid == 0) {
        pid1 = getpid();
        printf("child process1 PID: %d\n", pid1);
        // ʹ�� exec �������� system_call ��ִ���ļ�
        char *args[] = {"./system_call", NULL};
        execv(args[0], args);
        // ��� execv ʧ�ܣ����������Ϣ
        perror("execv");
        exit(1);
        printf("child process PID: %d\n", pid1);
    } else {
        pid1 = getpid();
        printf("parent process PID: %d\n", pid1);
        wait(NULL);
    }
    return 0;
}

