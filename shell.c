#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXARG 7

int isBackground = 0;

void command_exec(char *args[]);

int main() {
    char buf[256];
    char *args[MAXARG];
    char *s;
    char *save;
    int argn;
    static const char delim[] = " \t\n";
    int pid, status;

    int mode = 0;
    
    while(1) {
        printf("[shell] "); /* 프롬프트 출력 */
        gets(buf);
        argn = 0;
       
        s = strtok_r(buf, delim, &save); /* 문자열에서 delim을 기준으로 단어를 잘라냄 */

        while(s) {
            if (strcmp(s, "&") == 0) { // (2) 후면 실행
                isBackground = 1;
            }
            args[argn++] = s;
            s = strtok_r(NULL, delim, &save);
        }
        args[argn] = (char *)0; // 더 이상 인수 없음
        if (argn == 0) /* 명령어가 없으면 루프 계속 */
            continue;
        if (!strcmp(args[0], "quit")) // 프롬프트 종료
            break;
        
        if (mode == 0)
            command_exec(args);
    }
    exit(0);
}

void command_exec(char *args[]) {
    int status, pid;
    
    if ( (pid = fork()) == -1 ) // fork 호출에 실패하면
        perror("fork failed");
    else if (pid != 0) { // 부모 - 자식 프로세스의 종료 wait
        if ( isBackground ) {
            printf("[1] %d\n", getpid());
            waitpid(pid, &status, WNOHANG);
            isBackground = 0;
        }
        else
            pid = waitpid(pid, &status, 0);
    }
    else { // 자식 - execvp를 이용하여 arg[0] 실행
        execvp(args[0], args);
    }
}
