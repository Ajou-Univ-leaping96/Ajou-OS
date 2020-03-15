//server.c

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int D_num;
	int recv;
	char buffer[256];
	const char* FIFO = "/tmp/FIFO";
	int buff_count = 0;
	char file_name[256];
	char read_write[2];
	int bytes;
	int fd;
	int num;
	char string[256];
	int str_len;
	char wr_str[256];

	//이전 프로세스에서 남을 수도 있는 FIFO 파일을 지우고 새로운 FIFO 생성
	unlink(FIFO);
	if (mkfifo(FIFO, 0666) == -1)
	{
		printf("파이프 생성 오류\n");
		return -1;
	}

	// FIFO에 대해 OPEN, NONBLOCK 모드로 client의 요청이 없어도 항시대기
	D_num = open(FIFO, O_RDWR | O_NONBLOCK );
	if (D_num < 0)
	{
		printf("open 오류\n");
		return -1;
	}
	while (1)
	{
		// FIFO 를 읽음
		memset(buffer, 0, sizeof(buffer));
		recv = read(D_num, buffer, sizeof(buffer));
		// client의 요청이 있으면
		if (recv > 0)
		{ //차례로 작업 수

			switch (buff_count)
			{
				case 0:
					printf("파일명: [%s]\n", buffer);
					strcpy(file_name, buffer);
					buff_count++;
					continue;
				case 1:
					printf("읽기/쓰기: [%s]\n", buffer);
					read_write[0] = buffer[0];
					read_write[1] = '\0';
					buff_count++;
					continue;

				case 2:
					printf("바이트/스트링: [%s]\n", buffer);
					if (strcmp(read_write, "r") == 0)  
					{
						bytes = atoi(buffer);
					}
					else if (strcmp(read_write, "w") == 0)
					{
						strcpy(string, buffer);
						str_len = strlen(string);
					}
					else
					{
						printf("r/w 입력 에러입니다!");
						break;
					}
					buff_count++;
					continue;
			}
		}

		else if (buff_count == 3)
		{
			if (strcmp(read_write, "r") == 0)
			{

				printf(" \n %s 파일명에 해당하는 파일을 %s(읽어서) %d 바이트 출력\n\n", file_name, read_write, bytes); // 이 기능 구현, 안될시 에러출력

				pid_t pid;

				pid = fork();
				if(pid < 0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				if(pid == 0){// 자식 프로세스
					char str_bytes[bytes];
					sprintf(str_bytes, "%d", bytes);

					//printf("\n %s =str bytes \n",str_bytes);

					execl("/usr/bin/head","/usr/bin/head", "-c", str_bytes, file_name, NULL);

					
				}

				else { //부모 프로세스
					int status;

					waitpid(pid, &status, 0);
					//printf("child (PID=%d) finished; ",pid);
				}

			}

			else if (strcmp(read_write, "w") == 0)
			{
				printf("\n %s 파일명에 해당하는 파일에다 %s 라고 %s 쓴다\n", file_name, string, read_write); //이 기능 구현, 안될시 에러


				pid_t pid;

				pid = fork();
				if(pid < 0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				if(pid == 0){// 자식 프로세스

					for(int i=0; i<str_len; i++){
						wr_str[i] = string[i];
					}
					wr_str[str_len] = '\0';
					printf("\n 최종 쓸 스트링 %c", wr_str[0]);

					fd = open(file_name, O_WRONLY | O_APPEND);
					if (fd == -1)
					{
						perror("\n file name error \n");
					}
					write(fd,wr_str,strlen(wr_str));
					close(fd);
					 printf("쓰여진 바이트수  %d\n", str_len);
					exit(99);
				}

				else { //부모 프로세스
					int status;

					waitpid(pid, &status, 0);
					//printf("child (PID=%d) finished; ",pid);
					exit(0); //작업중
				}

			}

			else
			{
				printf(" 에러");
			}
			buff_count = 0;
		}

		else
		{
			printf("-------------------------------------\n");
		}

		sleep(1);
	}

	// 정리
	close(D_num);
	unlink(FIFO);

	return 0;
}
