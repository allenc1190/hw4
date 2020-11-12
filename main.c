#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


// Buffer 1, shared resource between input thread and square-root thread
char* buffer_1[9999];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;





char *inp;
char* getUserInp(){
	inp = (char *)malloc(sizeof(char));

	printf("Input text: ");
	scanf("%[^\n]%*c", inp);
	//int num = strlen(inp);
	//printf("%d\n\n", num);

	return inp;
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* inp){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  buffer_1[prod_idx_1] = inp;
  //printf("in buffer: %s\n", buffer_1[prod_idx_1]);
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

void* input(void* args){
	//printf("thread 1\n");
	//char *inp;
  inp = getUserInp();
	put_buff_1(inp);

	return NULL;
}

/*
Get the next item from buffer 1
*/
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while(count_1 == 0){
    pthread_cond_wait(&full_1, &mutex_1);
  }
  
  char* inp = buffer_1[prod_idx_1];
  // Unlock the mutex
  //pthread_mutex_unlock(&mutex_1);

  // Return the item
  return inp;
}


void* lineSep(void* args){
	printf("thread 2\n");

	return NULL;
}


char *replace(char* input){
	//char oldw[] = "++";
	//char neww[] = "^";
	//int oldwlen = strlen(oldw);
	//int newwlen = strlen(neww);
  int count = 0;
	char *result;
	int i;
  //printf("%s\n", input);
	//printf("check inp: %s\n", inp);
	for(i = 0; input[i] != '\0'; i++){
    count++;
		if(strstr(&input[i], "++") == &input[i]){
			//count how many times ++ shows up
			//printf("count = %d\n", count);
			//printf("%s\n", &inp[i]);
			i++;
			
		}
		//printf("%s\n", &inp[i]);
	}
  
	result = (char*)malloc(i+count);
	i = 0;
	while(*input){
		if(strstr(input, "++") == input){
			strcpy(&result[i], "^");
			i += 1;
			input += 2;
		}
		else{
			result[i++] = *input++;
		}
	}
	result[i] = '\0';
	return result;
}

char* test(){
	char* finall;
  finall = get_buff_1();
	finall = replace(finall);
	printf("\n\nNew string: %s\n", finall);

	return finall;
}


void* plusSign(void* args){
	test();

	return NULL;
}





/**
pass finall result
for (int i ; i < 79; i++)

*/

char* readtxt(char* finall){
	printf("\n\nTXT FILE: %s\n\n", finall);

	return finall;
}



void* output(void* args){
	//printf("thread 4\n");

	return NULL;
}



int main(){

	printf("Before thread\n");
	
	pthread_t input_t, lineSep_t, plusSign_t, output_t;

  /*char* s = "++the test+++ word++++++ +++dog";
  s = replace(s);
  printf("%s\n", s);*/

	//create threads
	pthread_create(&input_t, NULL, input, NULL);
	//pthread_create(&lineSep_t, NULL, lineSep, NULL);
	pthread_create(&plusSign_t, NULL, plusSign, NULL);
	//pthread_create(&output_t, NULL, output, NULL);

	//wait for threads
	pthread_join(input_t, NULL);
	//pthread_join(lineSep_t, NULL);
	pthread_join(plusSign_t, NULL);
	//pthread_join(output_t, NULL);
	

	printf("After thread\n");

  return 0;
}