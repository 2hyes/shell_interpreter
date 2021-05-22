#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXARG 7

int isBackground = 0;
char *args[MAXARG];

int tokenizeByDelim(char buf[], char *delim);
void command_exec(int i);

int main() {
    char buf[256];
    char *s;
    char *save;
    int argn, argc;
    static const char delim[] = " \t\n";
    int pid, status;

    int mode, brpoint;
    int i;
    char *input = "";
    char *output = "";
    
    while(1) {
        printf("[shell] "); /* 프롬프트 출력 */
        gets(buf);
        
        argc = 0;
        if (strchr(buf, ';') != NULL) // (3) 순차 실행
            argc = tokenizeByDelim(buf, ";");
        else {
            argc = 1;
            args[0] = buf;
            args[1] = (char *)0;
        }
        if (!strcmp(args[0], "quit")) // 프롬프트 종료
            break;
        
        for (i = 0; i < argc; i++) { // (3) 명령어 개수만큼 반복하면서 순차 실행
            argn = 0; mode = 0; isBackground = 0;
            
            s = strtok_r(args[i], delim, &save); /* 문자열에서 delim을 기준으로 단어를 잘라냄 */
            
            while(s) {
                if (strcmp(s, "&") == 0) { // (2) 후면 실행
                    isBackground = 1;
                }
 
                else {
                    if(mode < 2)
                        args[argn++] = s;
                    else if(mode == 2)
                        strcpy(input, s);
                    else
                        strcpy(output, s);
                }
                s = strtok_r(NULL, delim, &save);
            }
            
            args[argn] = (char *)0;
            
            if (mode == 0) {
                for (i = 0; i < argc; i++)
                    command_exec(i);
            }

        }
    }
    exit(0);
}

int tokenizeByDelim(char buf[], char *delim){
    char *s;
    char *save;
    int argn = 0;
    int i;
    
    s = strtok_r(buf, delim, &save);
    
    while(s) {
        args[argn++] = s;
        s = strtok_r(NULL, delim, &save);
    }
    
    args[argn] = (char *)0; // 더 이상 인수 없음
    
    return argn;
}


void command_exec(int i) {
    int status, pid;
    
    if ( (pid = fork()) == -1 ) // fork 호출에 실패하면
        perror("fork failed");
    else if (pid != 0) { // 부모 - 자식 프로세스의 종료 wait
        if ( isBackground ) {
            printf("[1] %d\n", getpid());
            waitpid(pid, &status, WNOHANG);
        }
        else
            pid = waitpid(pid, &status, 0);
    }
    else { // 자식 - execvp를 이용하여 arg[0] 실행
        execvp(args[i], args);
    }
}
