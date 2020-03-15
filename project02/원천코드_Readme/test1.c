#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#define TABLE_SIZE 30

int work_table[TABLE_SIZE]; //상품을 TABLE_SIZE만큼 놓을 수 있는 작업대
int item = 0;    //작업대 위에 있는 상품의 갯수
int todo_work  = 1000; //소비+생산의 총 목표작업수
int done_work = 0;    //실행한 총 작업의 횟수
int done_prod = 0;    //생산한  횟수
int done_cons = 0;    //소비한  횟수

pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

//생산자, 소비자 스레드의 작업 함수
//함수는 작업1회분량이며 목표작업수에 도달할때까지 무한루프 
int work(int mode){
	//뮤텍스
	pthread_mutex_lock(&mutex);
	//목표작업수 도달할 경우 함수 종료
	if(done_work>= todo_work){ 
		pthread_mutex_unlock(&mutex);
		return -1;
	}
	if(mode == 1){
		//생산자가 작업대를 꽉채우면 함수종료
		if(item >= TABLE_SIZE){
			pthread_mutex_unlock(&mutex);
			return 0;
		}
		//작업대의 0~29 칸을 차례대로 1로 채운다 => 생산
		work_table[done_prod%TABLE_SIZE] = 1;
		//생산한 수 증가
		done_prod++;
		//상품갯수 증가
		item++;
		//총작업수 증가
		done_work++;
		//

	}      
	else{
		//소비자가 작업대를 다 비우면 함수종료
		if(item<=0){
			pthread_mutex_unlock(&mutex);
			return 0;
		}
		//작업대의 0~29칸을 차례대로 0으로 채운다 =>소비
		//0으로 채우는것이 아니라 1을 0으로 비우는 개념으로 생각한다
		work_table[done_cons%TABLE_SIZE] = 0;
		//소비한 수 증가
		done_cons++;
		//상품갯수 감소
		item--;
		//총작업수 증가
		done_work++;
		//

	}
	//결과 출력
	printf("%5d ", done_work);
	for(int i = 0; i<TABLE_SIZE; i++)
		printf("%d",work_table[i]);
	printf("\n");
	pthread_mutex_unlock(&mutex);
	return 0;
}
//생산자 스레드 함수
void* produce_thread(){
	while(1){
		if(work(1)==-1)
			pthread_exit(NULL);
	}
}
//소비자 스레드 함수
void* consume_thread(){
	while(1){
		if(work(0)==-1)
			pthread_exit(NULL);
	}
}

int main(int argc, char* argv[]){
	int status  = 0;//스레드 생성시 에러검출위한 변수
	int p_num =0; //생산자 스레드 번호
	int c_num =0; //소비자 스레드 번호
	pthread_t thread_p; //생산자 스레드
	pthread_t thread_c; //소비자 스레드


	//생산자 쓰레드 생성
	status = pthread_create(&thread_p, NULL,  produce_thread,(void*) (&p_num));
	if(status != 0)
		printf("생산자 스레드의 생성오류입니다 : %d\n",status);


	//소비자 쓰레드 생성
	status = pthread_create(&thread_c, NULL, consume_thread, (void*) (&c_num));
	if(status != 0)
		printf("소비자 스레드의 생성오류입니다. : %d\n",status);

	//생산자가 모두 끝날 때 까지 기다림
	pthread_join(thread_p, (void**)&status);
	//소비자가 모두 끝날 때 까지 기다림
	pthread_join(thread_c, (void**)&status);
	// 생산자 쓰레드의 명령어 실행 횟수 출력
	printf("생산자 %d의 작업량 :%d\n",p_num,done_prod);
	printf("\n");
	//소비자 쓰레드의 명령어 실행 횟수 출력
	printf("소비자 %d의 작업량 :%d\n",c_num,done_cons);


	return 0;
}


