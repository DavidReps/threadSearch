// basicsearch.c: Search for a pattern in a text string.
//                Uses a single thread with a brute force algorithm.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int searchPosition(int i);
void * searcher(void * arg);

#define TEXT_LEN 1000000
#define NUM_THREADS 5
pthread_mutex_t countlock;
// The pattern and text strings are global variables
char *pattern;
char text[TEXT_LEN];

int nextAvailable = 0; //tracking our progress
int result = -1; //ultimate end location

int main(int argc, char *argv[]) {

   // Step 1: Extract the pattern string from the command line.

   pattern = argv[1];
   int patlen = strlen(pattern);

   // Step 2: Create and populate the text string.
   // Note that it isn't a proper "string" in the C sense,
   // because it doesn't end in '\0'.

   int textlen = TEXT_LEN; // will be overridden by the actual length
   int count = 0;
   while (count < TEXT_LEN) {
       int status = scanf("%c", &text[count]);
       count++;
       if (status == EOF) {
          textlen = count;
          break;
       }
   }

   // Step 3: Search for the pattern in the text string

   pthread_mutex_init(&countlock, NULL);
   int i = 0;
   pthread_t threads[NUM_THREADS];
   for(i; i < NUM_THREADS; i++){ //make the threads
     pthread_create(&threads[i], NULL, searcher, NULL);
   }
   int j = 0;
   for(j; j < NUM_THREADS; j++){ //join threads
     pthread_join(threads[j], NULL);
   }

   // Step 4: Determine the result and print it

   if (result == -1)
       printf("Pattern not found\n");
   else
       printf("Pattern begins at character %d\n", result);
}


int searchPosition(int i) {
   int j;
   for (j=0;j<strlen(pattern); j++)
       if (text[i+j] != pattern[j])
          return 0;
   return 1;
}

void * searcher (void * arg){
  int location;

  while ((result == -1) && (nextAvailable < TEXT_LEN)){
    pthread_mutex_lock(&countlock);
    location = nextAvailable; //choose location
    nextAvailable++; //increment location
    pthread_mutex_unlock(&countlock);

    if (searchPosition(location) == 1){ //check current position
      pthread_mutex_lock(&countlock);
      result = location;
      pthread_mutex_unlock(&countlock);
    }
  }

  return NULL;
}
