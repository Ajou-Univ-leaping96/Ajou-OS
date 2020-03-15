#include <stdio.h>
#include <stdlib.h>       
#include <pthread.h>      
#include <semaphore.h>    
#include <unistd.h>

#define MAX_READER 3//생성할 reader 의 수
#define MAX_NOTE_PAGE 10//노트의 총 페이지수
sem_t wrt,mutex;//세마포
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
		sem_wait(&wrt);//wrt세마포 증가 또는 대기
		//note에다 write
		note[written_page] = 1;
		written_page++;
		printf("\n--------------------------------------\n");
		printf("writer가 %d 번째 페이지 작성완료\n", written_page);
		printf("--------------------------------------\n\n");
		sem_post(&wrt);//wrt세마포 감소 또는 대기
		sleep(2);
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
		//mutex & wrt
		sem_wait(&mutex);
		read_count++;
		if ( read_count==1 )
			sem_wait(&wrt);
		sem_post(&mutex);
		//note를 read
		printf("\n");
		printf("Reader%d가 %d번째 페이지까지 읽음 /",thread_num,written_page);
		printf("	내용: ");
		for(int i=0; i<MAX_NOTE_PAGE; i++){
			printf("%d" ,note[i]);
		}
		printf("\n");
		//mutex & wrt
		sem_wait(&mutex);
		read_count--;
		if ( read_count==0 )
			sem_post(&wrt);
		sem_post(&mutex);
		sleep(1);
	}while(1);

}

int main()
{
	//writer 스레드와 reader 스레드 변수 선언
	pthread_t   writer_thread;
	pthread_t reader_thread[MAX_READER];

	//wrt세마포와 뮤텍스 초기화
	sem_init(&wrt,0,1);
	sem_init(&mutex,0,1);
	
	int status;
	int thread_num_w = 0;
	
	//wirter 스레드의 생성
	status = pthread_create(&writer_thread, NULL, Writer, NULL);//writer thread 생성
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
		sleep(1);
	}

	pthread_join(writer_thread, NULL);
	//세마포해제
	sem_destroy(&wrt);
	sem_destroy(&mutex);

	return 0;
}
