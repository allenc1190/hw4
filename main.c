#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int size = 9999;
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

char inp2;
char* getUserInp(){
	inp2 = *(char *)malloc(sizeof(char));
	//printf("BEFORE GET INPUT\n");
	printf("Input text: ");
	fgets(&inp2, size, stdin);
	//printf("AFTER GET INPUT\n");
	//printf("user iniput == %s", &inp2);
	//scanf("%[^\n]%*c", &inp2);

	return &inp2;
}

//Put an item in buff_1
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

//get user input
void* input(void* args){
	//printf("\nthread 1\n");
	//printf("BEFORE INPUT\n");
  char *inp = getUserInp();
	put_buff_1(inp);
	//printf("AFTER INPUT\n");

	return NULL;
}

//Get the next item from buffer 1
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while(count_1 == 0){
    pthread_cond_wait(&full_1, &mutex_1);
  }
  char* inp = buffer_1[prod_idx_1];
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);

  // Return the item
  return inp;
}


void* lineSep(void* args){
	printf("thread 2\n");

	return NULL;
}

//find ++ and replace it with ^
char *replace(char* input){
  int count = 0;
	char *result;
	int i;
  //printf("%s\n", input);
	//printf("check inp: %s\n", inp);

	//count amount of times ++ shows up
	//printf("BEFORE REPLACE\n");
	for(i = 0; input[i] != '\0'; i++){
    count++;
		if(strstr(&input[i], "++") == &input[i]){
			//printf("count = %d\n", count);
      
			//printf("%s\n", &inp[i]);
			i++;
			
		}
    //printf("i = %i\n", i);
		//printf("%s\n", &inp[i]);
	}
  
	//allocating memory for a string with enough length
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

	//printf("AFTER REPLACE\n");
	return result;
}

//get the new string after repalcing all ++ with ^
char* afterPlus(){
	char* finall;
	//printf("BEFORE AFTER PLUS\n");
  finall = get_buff_1();
	finall = replace(finall);
	//printf("New string: %s\n", finall);
	//printf("AFTER AFTER PLUS\n");

	return finall;
}


void* plusSign(void* args){
	//printf("\nThread 3\n");
	//printf("BEFORE PLUS SIGN\n");
	
	char *aPlus = afterPlus();

	//printf("AFTER PLUS SIGN\n");
	
	return NULL;
}



/*

char* stdin = [][][][][][][][\n][][][][][][][];
                            ^
stdin = 80size -> X 82 = segfault & memory leak
[][]][][]][stdin][new_vale][][][][][][]

char* temp;

**************** BELOW THIS LINE
int num = strlen(stdin)
int num2 = num / 80
  char* temp = (char*)malloc(num+num2);

  for(int i = 0; i < num+num2; i++){ 
		count++;
		temp[i] = stdin[i]; i = 1
		
		if(count == 80){
			i++;
			temp[i] = "\n"
			count = 0;
		}
	}

	print temp
*/


char *createEighty(){
	int count = 0;
	char *oldstr = afterPlus();
	int num = strlen(oldstr);
	int num2 = num / 80;

	printf("BEFORE CREATE EIGHTY\n");
	char *temp = (char*)malloc(num+num2);
	for(int i = 0; i < num+num2; i++){
		count++;
		temp[i] = oldstr[i];
		//printf("count == %d\n", count);
		if(count == 80){
			i++;
			temp[i] = '\n';
			count = 0;
		}
	}

	//printf("AFTER CREATE EIGHTY\n");

	//printf("%s\n", temp);

	return temp;
}


void* output(void* args){
	//printf("\nthread 4\n");
	char *finalOut = createEighty();
	printf("%s\n", finalOut);
	printf("AFTER\n");

	
	return NULL;
}



int main(int argc, char**argv){

	printf("Before thread\n");
	
	pthread_t input_t, lineSep_t, plusSign_t, output_t;

	//create threads
	pthread_create(&input_t, NULL, input, NULL);
	//pthread_create(&lineSep_t, NULL, lineSep, NULL);
	pthread_create(&plusSign_t, NULL, plusSign, NULL);
	pthread_create(&output_t, NULL, output, NULL);
	

	//wait for threads
	pthread_join(input_t, NULL);
	//pthread_join(lineSep_t, NULL);
	pthread_join(plusSign_t, NULL);
	pthread_join(output_t, NULL);
	

	printf("After thread\n");

  return 0;
}