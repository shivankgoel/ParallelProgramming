//gcc -fopenmp -O2 -pthread -Wall par-prefix.c -o par -lcrypto
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <omp.h>
#include <string.h>
#include <openssl/md5.h>

void* thread_handler_up(void* threadinfo);
void* thread_handler_down(void* threadinfo);
char* readstring(FILE* infile, size_t size);

struct node{
	long long fromleft;
	long long value;
};

struct thread_struct{
	int tid;
	struct node* arrp;
	int diff;
};


int main(int argc , char* argv[]){

	//Taking input
	int n;
	FILE *inputfile;
	inputfile = fopen("input.txt","r");
	if(!inputfile){
		printf("%s\n", "Can't open input.txt file" );
		exit(0);
	}
	if(fscanf(inputfile,"%d",&n));
	
	
	//Number of elements in array
    struct node* arr;
    arr = malloc (sizeof(struct node)*n);
    
    //Number of thread_struct
    struct thread_struct* threadarr;
    threadarr = malloc (sizeof(struct thread_struct)*n);
    
    //Initialization
    for(int i=0;i<n;i++){
    	if(fscanf(inputfile,"%lld",&arr[i].value));
    	arr[i].fromleft = 0;
    	threadarr[i].tid = i;
    	threadarr[i].arrp = arr;
    	threadarr[i].diff = 1;
    } 
    fclose(inputfile);
	

    //Number Of threads inputted
    int inpnumthreads;
    if(argc < 2){
    	inpnumthreads = 2;
    }
    else{
    	inpnumthreads = atoi(argv[1]);
    }

    int num_threads=2;
    while(num_threads*2 <= inpnumthreads){
	    num_threads *=2;
	}
	
    pthread_t threads[num_threads];


    //Clock Set
    double start_time = omp_get_wtime();


    for(int level=1;level<n;level *= 2){
    	//printf("%d\n", level);
    
	    int tcreated=0;
		bool alreadyjoined = false;
		int tjump = 2 * level;
		int start = level - 1;
	    for(int i=start;i<n;i+=tjump){

	    	threadarr[i].diff = level;
	    	pthread_create(&threads[tcreated], NULL, thread_handler_up, (void*)&threadarr[i] );
			tcreated++;

			if(tcreated == num_threads){

				for(int j=0;j<tcreated;j++){
					pthread_join(threads[j],NULL);
				}
				alreadyjoined = true;	
				tcreated = 0;	
			}

	    }
	    if(!alreadyjoined){
	    	for(int j=0;j<tcreated;j++){
					pthread_join(threads[j],NULL);	
			}
	    }
    

	}


	for(int level=n/2;level>=1;level /= 2){
    
	    int tcreated=0;
		bool alreadyjoined = false;
		int tjump = 2 * level;
		int start = level - 1;
	    for(int i=start;i<n;i+=tjump){

	    	threadarr[i].diff = level;
	    	pthread_create(&threads[tcreated], NULL, thread_handler_down, (void*)&threadarr[i] );
			tcreated++;

			if(tcreated == num_threads){

				for(int j=0;j<tcreated;j++){
					pthread_join(threads[j],NULL);
				}
				alreadyjoined = true;	
				tcreated = 0;	
			}

	    }
	    if(!alreadyjoined){
	    	for(int j=0;j<tcreated;j++){
					pthread_join(threads[j],NULL);	
			}
	    }

	}

	double end_time = omp_get_wtime();
	double totalTime = end_time - start_time;
	//printf("Time Taken %f\n",totalTime);

	//Debug Output
	
	/*
	for(int i=0;i<n;i++){
		printf("%lld  %lld\n",arr[i].value , arr[i].fromleft );
	}
	*/
	

	//Output to a file
	FILE* outfile = fopen("output.txt", "w");
	for(int i=1;i<n;i++){
		fprintf(outfile, "%lld ", arr[i].fromleft);
	}
	fprintf(outfile, "%lld",arr[n-1].value);
	fclose(outfile);

	//Read in a char*
	char* inputstr;
	inputfile = fopen("output.txt", "r");
	inputstr = readstring(inputfile,n);
	//printf("%s\n",inputstr );
	fclose(inputfile);

	//Generate MD5 (Code taken from internet)
	unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, inputstr, strlen(inputstr));
    MD5_Final(digest, &ctx);
 
    char mdString[33];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 
    //printf("md5 digest: %s\n", mdString);


    //Final Output
    printf("Threads: %d\n",inpnumthreads );
    printf("Time: %f\n", totalTime );
    printf("Md5-sum: %s\n", mdString);
    
    //Output to file
    outfile = fopen("output.txt", "w");
		fprintf(outfile,"Threads: %d\n",inpnumthreads );
	    fprintf(outfile,"Time: %f\n", totalTime );
	    fprintf(outfile,"Md5-sum: %s\n", mdString);
	fclose(outfile);
	
	return 0;
}



void* thread_handler_up(void* threadinfo){
	struct thread_struct* tinfo = (struct thread_struct*)threadinfo;
	int id = tinfo->tid;
	int diff = tinfo->diff;
	tinfo->arrp[id+diff].value = tinfo->arrp[id].value + tinfo->arrp[id+diff].value;
	pthread_exit(0);
}


void* thread_handler_down(void* threadinfo){
	struct thread_struct* tinfo = (struct thread_struct*)threadinfo;
	int id = tinfo->tid;
	int diff = tinfo->diff;
	tinfo->arrp[id].fromleft = tinfo->arrp[id+diff].fromleft;
	tinfo->arrp[id+diff].fromleft = tinfo->arrp[id].fromleft + tinfo->arrp[id].value;  
	pthread_exit(0);
}

// Code Taken From 
//http://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
//
char *readstring(FILE* infile, size_t size){
//The size is extended by the input with the value of the provisional
    char *tmpstr;
    int ch;
    size_t strlen = 0;
    tmpstr = malloc(sizeof(char)*size);

    if(!tmpstr){
    	return tmpstr;
    }

    while(EOF!=(ch=fgetc(infile)) && ch != '\n'){
        tmpstr[strlen++]=ch;
        if(strlen==size){
            tmpstr = realloc(tmpstr, sizeof(char)*(size+=16));
            if(!tmpstr)return tmpstr;
        }
    }
    tmpstr[strlen]='\0';

    return realloc(tmpstr, sizeof(char)*strlen);
}