//client.c
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int D_num;
void send_Msg(const char* str)
{
    int len;
    len = strlen(str);
    write(D_num, str, len);
}

int main(int argc, char* argv[])
{
    const char* FIFO_Pipe = "/tmp/FIFO";
    
    //개방된 FIFO에 대해 Open한다
    D_num = open(FIFO_Pipe, O_WRONLY);
    if(D_num < 0)
    {
        printf("클라이언트 열기 오류\n");
		return -1;
    }
    
    // 클라이언트 요청 메세지를 보낸다.
    
	send_Msg(argv[1]);
    sleep(1);
    send_Msg(argv[2]);
    sleep(1);
    send_Msg(argv[3]);
    close(D_num);
    return 0;
}
