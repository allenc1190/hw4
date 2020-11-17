#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

//Code from the example

//creates buffer
int size = 9999;
//Buffer 1
char buffer_1[49][1000];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

//Code from the example
//Buffer 2
char buffer_2[49][1000];
int count_2 = 0;
int prod_idx_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

//Code from the example
char buffer_3[49][1000];
int count_3 = 0;
int prod_idx_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

//Code from the example
void put_buff_1(char* inp){
  pthread_mutex_lock(&mutex_1);
  strcpy(buffer_1[prod_idx_1], inp);
  //buffer_1[prod_idx_1] = inp;
  //printf("in buffer: %s\n", buffer_1[prod_idx_1]);
  count_1++;
  prod_idx_1++;
  pthread_cond_signal(&full_1);
  pthread_mutex_unlock(&mutex_1);
}

//Code from the example
char* get_buff_1(){
  pthread_mutex_lock(&mutex_1);
  while(count_1 == 0){
    pthread_cond_wait(&full_1, &mutex_1);
  }
  char* inp = buffer_1[prod_idx_1];
  pthread_mutex_unlock(&mutex_1);

  return inp;
}

//Code from the example
void put_buff_2(char* inp){
  pthread_mutex_lock(&mutex_2);
  //buffer_2[prod_idx_2] = inp;
  count_2++;
  pthread_cond_signal(&full_2);
  pthread_mutex_unlock(&mutex_2);
}

//Code from the example
char* get_buff_2(){
  pthread_mutex_lock(&mutex_2);
  while(count_2 == 0){
    pthread_cond_wait(&full_2, &mutex_2);
  }
  char* inp = buffer_2[prod_idx_2];
  pthread_mutex_unlock(&mutex_2);

  return inp;
}

//gets user input
/*
char* getUserInp(){
	int linecount = 0;
	int i;
  char *inp2;

	inp2 = (char *)malloc(1000 * sizeof(char));
	printf("Input text: \n");

	while(fgets(inp2, 1000, stdin) != NULL){
		linecount++;
		for(i = 0; i < linecount; i++){
			strcpy(buffer_1[i], inp2); 
		}
		//printf("TEXT FILE == %s\n", inp2);
	}

	//fgets(&inp2, size, stdin);
	//printf("user iniput == %s", &inp2);
	//scanf("%[^\n]%*c", &inp2);

	return inp2;
}
*/
//get user input for thread1
void* input(void* args){
	//printf("\nthread 1\n");
  //char *inp = getUserInp();
	//put_buff_1(inp);
  //printf("%s\n", buffer_1);
  size_t len;
  char *inp2;
  ssize_t nread;
  while((nread = getline(&inp2, &len, stdin)) != -1)
  {
    //printf("%s\n", inp2);
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer_1[prod_idx_1], inp2);
    count_1++;
    prod_idx_1++;
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
    if (strcmp(inp2, "STOP\n") == 0)
      break;
  }
  // for(int i = 0; i < prod_idx_1; i++)
  // {
  //   printf("%s\n", buffer_1[i]);
  // }
	return NULL;
}

char *createLineSep(){
	char *afReplace = get_buff_2();
	//printf("\n\nBUFF 2 == %s\n\n", afReplace);
	int i;
	int len2 = strlen(afReplace);	
	
	char *temp2 = (char*)malloc(len2);

	for(i = 0; i < len2; i++){
		temp2[i] = afReplace[i];
		
		if(afReplace[i] == '\n'){
			temp2[i] = ' ';
		}
		//printf("temppppp ======= %s\n", temp2);
	}

	//printf("TEMP2 == %s\n", temp2);

	return temp2;
}

void* lineSep(void* args){
	//printf("thread 2\n");
	//char *inp3 = createLineSep();
	//printf("\n\nBline sep == %s\n\n", inp3);
  for (int i = 0; i < 49; i++)
  {
    pthread_mutex_lock(&mutex_1);
    while (count_1 == 0)
      pthread_cond_wait(&full_1, &mutex_1);

    char *sec = malloc(1000 * sizeof(char));
    strcpy(sec, buffer_1[prod_idx_2]);
    count_1--;
    pthread_mutex_unlock(&mutex_1);
    //printf("%s\n", sec);
    int ii = 0;
    while(sec[ii] != '\n')
    {
      ii++;
    }
    sec[ii] = ' ';
    pthread_mutex_lock(&mutex_2);
    strcpy(buffer_2[prod_idx_2], sec);
    prod_idx_2++;
    count_2++;
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);

    if (strcmp(sec, "STOP ")==0)
      break;

    //printf("index: %d \n %s \n", prod_idx_2, buffer_2[prod_idx_2 - 1]);
  }
 

	return NULL;
}

//find ++ and replace it with ^
char *replace(char* input){
  int count = 0;
	char *result;
	int i;

	for(i = 0; input[i] != '\0'; i++){
    count++;
		if(strstr(&input[i], "++") == &input[i]){
			i++;
			
		}
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

	return result;
}

//get the new string after repalcing all ++ with ^
char* afterPlus(){
	char* finall;
  finall = get_buff_1();
	finall = replace(finall);
	//put string with carets in buffer 2
	put_buff_2(finall);
	//printf("New string: %s\n", finall);
	//printf("after plus: \n %s\n", finall);
	return finall;
}

//thread 2
void* plusSign(void* args){
	//printf("\nThread 3\n");
	//char *aPlus = afterPlus();
	//printf("here === %s\n", aPlus);
  for (int i = 0; i < 49; i++)
  {
    pthread_mutex_lock(&mutex_2);
    while (count_2 == 0)
      pthread_cond_wait(&full_2, &mutex_2);

    char *sec = malloc(1000 * sizeof(char));
    strcpy(sec, buffer_2[prod_idx_3]);
    count_2--;
    pthread_mutex_unlock(&mutex_2);

    char *sec2 = replace(sec);
    //printf("%s\n", sec2);
    //printf("%s\n", sec);
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer_3[prod_idx_3], sec2);
    prod_idx_3++;
    count_3++;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);

    if (strcmp(sec, "STOP ")==0)
      break;

    //printf("index: %d \n %s \n", prod_idx_2, buffer_2[prod_idx_2 - 1]);
  }

  for (int i = 0; i < prod_idx_3; i++)
  {
    printf("%s\n", buffer_3[i]);
  }

	return NULL;
}

//new line after 80 characters
char *createEighty(){
	int count = 0;
	char *oldstr = afterPlus();
  //printf("%d\n", strlen(oldstr));
	int num = strlen(oldstr);
	int num2 = num / 80;

	//allocates memory for temp
	char *temp = (char*)malloc(num+num2);
	//loop through each character until it reaches the last character
  printf("here\n %s\n", oldstr);

	for(int i = 0; i < num+num2; i++){
		count++;
		//copy array to temp array
		temp[i] = oldstr[i];
		//once it hits the 80th character, it prints a new line into the next index of array
		if(count == 80){
			i++;
			temp[i] = '\n';
			count = 0;
		}
	}
	//printf("%s\n", temp);

	return temp;
}

//thread 4
void* output(void* args){
	//printf("\nthread 4\n");
	char *finalOut = createEighty();
	printf("\n%s\n", finalOut);
	//printf("AFTER\n");
	
	return NULL;
}

int main(){

	//printf("Before thread\n");
	
	pthread_t input_t, lineSep_t, plusSign_t, output_t;

	pthread_create(&input_t, NULL, input, NULL);
	pthread_create(&lineSep_t, NULL, lineSep, NULL);
	pthread_create(&plusSign_t, NULL, plusSign, NULL);
	pthread_create(&output_t, NULL, output, NULL);


	pthread_join(input_t, NULL);
	pthread_join(lineSep_t, NULL);
	pthread_join(plusSign_t, NULL);
	pthread_join(output_t, NULL);

	//printf("After thread\n");

  return 0;
}