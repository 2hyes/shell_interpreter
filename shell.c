#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXARG 7

int main() {
    char buf[256];
    char *args[MAXARG];
    char *s;
    char *save;
    int argn;
    static const char delim[] = " \t\n";
    int pid, status;

    while(1) {
        printf("[shell] "); /* 프롬프트 출력 */
        gets(buf);
        argn = 0;
       
        s = strtok_r(buf, delim, &save); /* 문자열에서 delim을 기준으로 단어를 잘라냄 */

        while(s) {
            args[argn++] = s;
            s = strtok_r(NULL, delim, &save);
        }
        args[argn] = (char *)0; // 더 이상 인수 없음
        if (argn == 0) /* 명령어가 없으면 루프 계속 */
            continue;
        if (!strcmp(args[0], "quit")) // 프롬프트 종료
            break;

        if ( (pid = fork()) == -1 ) // fork 호출에 실패
            perror("fork failed");
        else if (pid != 0) { // 부모 프로세스 - 자식 프로세스의 종료 wait
            pid = waitpid(pid, &status, 0);
        }
        else { // 자식 프로세스 - execvp를 이용하여 arg[0] 실행
            execvp(args[0], args);
        }
    }
    exit(0);
}
