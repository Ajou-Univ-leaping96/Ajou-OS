#include <stdio.h>
#include <stdlib.h>       
#include <pthread.h>      
#include <semaphore.h>    
#include <unistd.h>

#define MAX_READER 3//생성할 reader 의 수
#define MAX_NOTE_PAGE 10//노트의 총 페이지수
int read_count=0;//reader 스레드에서 사용할 count
int written_page=0;//쓰여진 페이지 수
int note[MAX_NOTE_PAGE] ={0,};//writer가 쓰고 reader가 읽는 노트
//writer 스레드 함수
void* Writer()
{
	do
	{	
		if(written_page >= MAX_NOTE_PAGE){
			printf("모든페이지 작성 완료, 프로그램을 종료합니다\n");
			exit(0);
		}
		//note에다 write
		note[written_page] = 1;
		written_page++;
		printf("\n--------------------------------------\n");
		printf("writer가 %d 번째 페이지 작성완료\n", written_page);
		printf("--------------------------------------\n\n");
	}
	//이후 무한반복
	while(1);
}

//reader 스레드 함수
void* Reader(void* arg)  
{
	int thread_num =*(int*)arg;
	do
	{
		//note를 read
		printf("\n");
		printf("Reader%d가 %d번째 페이지까지 읽음 /",thread_num,written_page);
		printf("	내용: ");
		for(int i=0; i<MAX_NOTE_PAGE; i++){
			printf("%d" ,note[i]);
		}
		printf("\n");
	}while(1);

}

int main()
{
	//writer 스레드와 reader 스레드 변수 선언
	pthread_t   writer_thread;
	pthread_t reader_thread[MAX_READER];
	
	int status;
	int thread_num_w = 0;
	
	//wirter 스레드의 생성
	status = pthread_create(&writer_thread, NULL, Writer, &status);//writer thread 생성
	if(status != 0){
		printf("writer 스레드 생성 오류 %d \n", status);
	}

	//reader 스레드의 생성
	for(int i=0; i<MAX_READER; i++)
	{
		status = pthread_create(&reader_thread[i], NULL, Reader, (void*)&i);
		if(status !=0){
			printf("reader 스레드 생성 오류 %d \n", status);
		}
	}

	pthread_join(writer_thread, NULL);
	
	return 0;
}
