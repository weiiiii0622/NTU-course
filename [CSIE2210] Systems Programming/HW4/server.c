#include "header.h"

#define DEBUG 0
#define DEPTH 4
#define GROUP 32
#define MINMOV 1500

movie_profile* movies[MAX_MOVIES];
unsigned int num_of_movies = 0;
unsigned int num_of_reqs = 0;

// Global request queue and pointer to front of queue
// TODO: critical section to protect the global resources
request* reqs[MAX_REQ];
int front = -1;

/* Note that the maximum number of processes per workstation user is 512. * 
 * We recommend that using about <256 threads is enough in this homework. */
pthread_t tid[MAX_CPU][MAX_THREAD]; //tids for multithread
pthread_t init_tid[MAX_REQ];
int cpu_front = -1;
int thread_front[MAX_CPU];

pid_t pid[MAX_CPU][MAX_THREAD]; //pids for multiprocess
int prc_front = -1;

//mutex
pthread_mutex_t lock; 
pthread_mutex_t cpu_lock[MAX_CPU]; 

void initialize(FILE* fp);
request* read_request();
int front_pop();
int cpu_pop();
int thread_pop();

int front_pop(){
	front+=1;
	return front;
}

int cpu_pop(){
	cpu_front += 1;
	return cpu_front;
}

int thread_pop(int cpu){
	thread_front[cpu] += 1;
	return thread_front[cpu];
}

int process_pop(int cpu){
	prc_front += 1;
	return prc_front;
}

/*-------------------------------------------------------------------------------------*/
// tserver functions

struct Sortarg{
	int cpu;
	int depth;
	char** movie;
	double* pts;
	int l;
	int r;
};

struct Sortarg* wrap_arg(int cpu, int depth, char** movie, double* pts, int l, int r){
	struct Sortarg* newarg = (struct Sortarg*) malloc(sizeof(struct Sortarg));
	newarg->cpu = cpu;
	newarg->depth = depth;
	newarg->movie = movie;
	newarg->pts = pts;
	newarg->l = l;
	newarg->r = r;
	return newarg;
}

void merge(char** movie, double* pts, int l, int r){
	//temp arr
	int m = (l+r)/2;
	int sizeL = m-l+1, sizeR = r-m;
	char* movL[sizeL];
	char* movR[sizeR];
	double ptsL[sizeL];
	double ptsR[sizeR];
	for(int i=0; i<sizeL; i++){
		movL[i] = movie[l+i];
		ptsL[i] = pts[l+i];
	}
	for(int i=0; i<sizeR; i++){
		movR[i] = movie[m+1+i];
		ptsR[i] = pts[m+1+i];
	}

	// merge
	int i=0, j=0, k=l;
	while(i<sizeL && j<sizeR){
		if(ptsL[i] > ptsR[j]){
			movie[k] = movL[i];
			pts[k] = ptsL[i];
			i += 1;
		}
		else if(ptsL[i] < ptsR[j]){
			movie[k] = movR[j];
			pts[k] = ptsR[j];
			j += 1;
		}
		else{
			if(strcmp(movL[i], movR[j]) > 0){
				movie[k] = movR[j];
				pts[k] = ptsR[j];
				j += 1;	
			}
			else{
				movie[k] = movL[i];
				pts[k] = ptsL[i];
				i += 1;
			}
		}
		k += 1;
	}
	while(i<sizeL){
		movie[k] = movL[i];
		pts[k] = ptsL[i];
		i += 1;
		k += 1;
	}
	while(j<sizeR){
		movie[k] = movR[j];
		pts[k] = ptsR[j];
		j += 1;	
		k += 1;
	}
	return;
}


void* merge_sort(void* arg){
	struct Sortarg* args = (struct Sortarg*)arg;
	int cpu = args->cpu;
	int depth =  args->depth;
	char** movie = args->movie;
	double* pts = args->pts;
	int l = args->l;
	int r = args->r;
	int m = (l+r)/2;
	// fprintf(stdout, "depth: %d l: %d r: %d\n", DEPTH-depth, l, r);
	if(r >= l){
		if(r-l<=MINMOV || depth == 0){
			sort(&(movie[l]), &(pts[l]), r-l+1);
		}
		else{
			int thread_stack[32];
			int l_stack[32];
			int r_stack[32];
			int depth_cnt = 0;
			for(int i=0; i<depth; i++){
				pthread_mutex_lock(&cpu_lock[cpu]);

				int thread_r = thread_pop(cpu);
				//fprintf(stdout, "depth %d created %d\n", DEPTH-depth+i, thread_r);
				thread_stack[i] = thread_r;
				l_stack[i] = l;
				r_stack[i] = r;

				struct Sortarg* arg_r = wrap_arg(cpu, depth-i-1, movie, pts, m+1, r);

				pthread_create(&tid[cpu][thread_r], NULL, merge_sort, arg_r);
				depth_cnt += 1;
				l = l;
				r = m;
				m = (l+r)/2;

				pthread_mutex_unlock(&cpu_lock[cpu]);
				if(r-l <= MINMOV) break;
				//free(arg_r);
			}
			if(r>=l) sort(&(movie[l]), &(pts[l]), r-l+1);

			for(int i=depth_cnt-1; i>=0; i--){
				pthread_join(tid[cpu][thread_stack[i]], NULL);
				merge(movie, pts, l_stack[i], r_stack[i]);
			} 
		}
	}
	pthread_exit(NULL);
}

void* thread_handle_mulreq(){
	// lock mutex
	pthread_mutex_lock(&lock);

	request* cur = reqs[front_pop()];
	cur->cpu = cpu_pop();
	cur->main_thread = thread_pop(cur->cpu);
	//if(DEBUG) fprintf(stdout, "Req %d: start at cpu: %d, thread: %d\n", front, cur->cpu, cur->main_thread);

	pthread_mutex_unlock(&lock);
	
	// get movies
	int movie_cnt = 0;
	char** movie = (char**) malloc(sizeof(char*)*num_of_movies);
	double* pts = (double*) malloc(sizeof(double)*num_of_movies);
	
	if(cur->keywords[0] == '*'){
		movie_cnt = num_of_movies;
		for(int i=0, j=0; i<num_of_movies; i++){
			movie[j] = movies[i]->title;
			double pt = 0;
			for(int k=0; k<NUM_OF_GENRE; k++){
				pt += movies[i]->profile[k] * cur->profile[k];
			}
			pts[j] = pt;
			j += 1;
		}
	}
	else{
		for(int i=0, j=0; i<num_of_movies; i++){
			if(strstr(movies[i]->title, cur->keywords)){
				movie[j] = movies[i]->title;
				double pt = 0;
				for(int k=0; k<NUM_OF_GENRE; k++){
					pt += movies[i]->profile[k] * cur->profile[k];
				}
				pts[j] = pt;
				j += 1;
				movie_cnt += 1;
			}
		}
	}

	struct Sortarg* arg = wrap_arg(cur->cpu, DEPTH, movie, pts, 0, movie_cnt-1);
	pthread_create(&tid[cur->cpu][cur->main_thread], NULL, merge_sort, arg);
	pthread_join(tid[cur->cpu][cur->main_thread], NULL);

	char path[512];
	//if(DEBUG) sprintf(path, "./%dt.txt", cur->id);
	sprintf(path, "./%dt.out", cur->id);
	//if(DEBUG) fprintf(stdout, "req %d opened %s\n", cur->cpu, path);
	int fd = open(path, O_RDWR|O_CREAT|O_TRUNC|O_APPEND, 0777);
	for(int i=0; i<movie_cnt; i++){
		char buf[MAX_LEN];
		//sprintf(buf, "%s pts:%lf\n", movie[i], pts[i]);
		sprintf(buf, "%s\n", movie[i]);
		write(fd, buf, strlen(buf));
	}

	free(movie); free(pts);
	close(fd);
	pthread_exit(NULL);
}

/*-------------------------------------------------------------------------------------*/

// pserver function

void process_merge_sort(char *movie[], double *pts, int depth, int l, int r){
	int m = (l+r)/2;

	if(depth == 0){
		char **temp = (char**) malloc(sizeof(char*)*(r-l+1));
		for(int i=0; i<r-l+1; i++){
			temp[i] = (char*) malloc(sizeof(char)*MAX_LEN);
			strcpy(temp[i], movie[i+l]);
		}
		sort(temp, &(pts[l]), r-l+1);
		for(int i=0; i<r-l+1; i++){
			strcpy(movie[i+l], temp[i]);
			free(temp[i]);
		}
		free(temp);
	}
	else{
		int pid;
		if((pid = fork()) == 0){
			process_merge_sort(movie, pts, depth-1, l, m);
			process_merge_sort(movie, pts, depth-1, m+1, r);
			_exit(0);
		}
		else{
			waitpid(pid, NULL, 0);

			//temp arr
			int sizeL = m-l+1, sizeR = r-m;
			char** movL = (char**) malloc(sizeof(char*)*(sizeL));
			char** movR = (char**) malloc(sizeof(char*)*(sizeR));
			double ptsL[sizeL];
			double ptsR[sizeR];
			for(int i=0; i<sizeL; i++){
				movL[i] = (char*) malloc(sizeof(char)*MAX_LEN);
				strcpy(movL[i], movie[l+i]);
				//movL[i] = movie[l+i];
				ptsL[i] = pts[l+i];
			}
			for(int i=0; i<sizeR; i++){
				movR[i] = (char*) malloc(sizeof(char)*MAX_LEN);
				strcpy(movR[i], movie[m+1+i]);
				//movR[i] = movie[m+1+i];
				ptsR[i] = pts[m+1+i];
			}

			// merge
			int i=0, j=0, k=l;
			while(i<sizeL && j<sizeR){
				if(ptsL[i] > ptsR[j]){
					strcpy(movie[k], movL[i]);
					//movie[k] = movL[i];
					pts[k] = ptsL[i];
					free(movL[i]);
					i += 1;
				}
				else if(ptsL[i] < ptsR[j]){
					strcpy(movie[k], movR[j]);
					//movie[k] = movR[j];
					pts[k] = ptsR[j];
					free(movR[j]);
					j += 1;
				}
				else{
					if(strcmp(movL[i], movR[j]) > 0){
						strcpy(movie[k], movR[j]);
						//movie[k] = movR[j];
						pts[k] = ptsR[j];
						free(movR[j]);
						j += 1;	
					}
					else{
						strcpy(movie[k], movL[i]);
						//movie[k] = movL[i];
						pts[k] = ptsL[i];
						free(movL[i]);
						i += 1;
					}
				}
				k += 1;
			}
			while(i<sizeL){
				strcpy(movie[k], movL[i]);
				//movie[k] = movL[i];
				pts[k] = ptsL[i];
				free(movL[i]);
				i += 1;
				k += 1;
			}
			while(j<sizeR){
				strcpy(movie[k], movR[j]);
				//movie[k] = movR[j];
				pts[k] = ptsR[j];
				free(movR[j]);
				j += 1;	
				k += 1;
			}

			free(movL); free(movR);
		}
	}
	
	return;
}

/*-------------------------------------------------------------------------------------*/


int main(int argc, char *argv[]){

	if(argc != 1){
#ifdef PROCESS
		fprintf(stderr,"usage: ./pserver\n");
#elif defined THREAD
		fprintf(stderr,"usage: ./tserver\n");
#endif
		exit(-1);
	}

	FILE *fp;

	if ((fp = fopen("./data/movies.txt","r")) == NULL){
		ERR_EXIT("fopen");
	}

	initialize(fp);
	assert(fp != NULL);
	fclose(fp);
	
	for(int i=0; i<num_of_reqs; i++){
		thread_front[i] = -1;
		pthread_mutex_init(&cpu_lock[i], NULL);
	}
	pthread_mutex_init(&lock, NULL);

#ifdef PROCESS
		//if(DEBUG) fprintf(stdout, "This is a pserver.\n");
		int movie_cnt = 0;
		request* cur = reqs[0];
		char *movie[num_of_movies];
		double* pts;
		pts = mmap(NULL, sizeof(double)*num_of_movies, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


		if(cur->keywords[0] == '*'){
			movie_cnt = num_of_movies;
			for(int i=0, j=0; i<num_of_movies && j<movie_cnt; i++){
				movie[j] = mmap(NULL, sizeof(char)*MAX_LEN, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
				//movie[j] = mmap(NULL, sizeof(char)*strlen(movies[i]->title), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
				strcpy(movie[j], movies[i]->title);
				double pt = 0;
				for(int k=0; k<NUM_OF_GENRE; k++){
					pt += movies[i]->profile[k] * cur->profile[k];
				}
				pts[j] = pt;
				j += 1;
			}
		}
		else{
			for(int i=0, j=0; i<num_of_movies; i++){
				if(strstr(movies[i]->title, cur->keywords)){
					movie[j] = mmap(NULL, sizeof(char)*MAX_LEN, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
					//movie[j] = mmap(NULL, sizeof(char)*strlen(movies[i]->title), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
					strcpy(movie[j], movies[i]->title);
					double pt = 0;
					for(int k=0; k<NUM_OF_GENRE; k++){
						pt += movies[i]->profile[k] * cur->profile[k];
					}
					pts[j] = pt;
					j += 1;
					movie_cnt += 1;
				}
			}
		}

		// start merge
		process_merge_sort(movie, pts, DEPTH, 0, movie_cnt-1);

		char path[512];
		//if(DEBUG) sprintf(path, "./%dp.txt", cur->id);
		sprintf(path, "./%dp.out", cur->id);
		//if(DEBUG) fprintf(stdout, "req %d opened %s\n", cur->cpu, path);
		int fd = open(path, O_RDWR|O_CREAT|O_TRUNC|O_APPEND, 0777);
		for(int i=0; i<movie_cnt; i++){
			char buf[MAX_LEN];
			//sprintf(buf, "%s pts:%lf\n", movie[i], pts[i]);
			sprintf(buf, "%s\n", movie[i]);
			write(fd, buf, strlen(buf));
		}

		//free(movie); free(pts);
		close(fd);

#elif defined THREAD
		if(num_of_reqs == 32){
			int part = 0;
			int total_round = (num_of_reqs%GROUP==0)?num_of_reqs/GROUP:num_of_reqs/GROUP+1;
			// 11, 15 < 6.0sec
			while(part<total_round){
				// fprintf(stdout, "part: %d\n", part);
				for(int i=0; i<GROUP; i++){
					if(part*GROUP+i < 32) pthread_create(&init_tid[part*GROUP+i], NULL, thread_handle_mulreq, NULL);
				}
				for(int i=0; i<GROUP; i++){
					if(part*GROUP+i < 32)  pthread_join(init_tid[part*GROUP+i], NULL);
					// fprintf(stdout, "thread: %d joined\n", part*8+i);
				}
				part += 1;
			}
		}
		else{
			for(int i=0; i<num_of_reqs; i++){
				pthread_create(&init_tid[i], NULL, thread_handle_mulreq, NULL);
			}
			for(int i=0; i<num_of_reqs; i++){
				pthread_join(init_tid[i], NULL);
				//if(DEBUG) fprintf(stdout, "thread: %d joined\n", i);
			}
		}
		// int total=0;
		// for(int i=0; i<num_of_reqs; i++){
		// 	total += thread_front[i]+1;
		// }
		//fprintf(stdout, "total thread: %d\n", total);
#endif

	pthread_mutex_destroy(&lock);
	for(int i=0; i<num_of_reqs; i++){
		pthread_mutex_destroy(&cpu_lock[i]);
	}

	return 0;
}

/**=======================================
 * You don't need to modify following code *
 * But feel free if needed.                *
 =========================================**/

request* read_request(){
	int id;
	char buf1[MAX_LEN],buf2[MAX_LEN];
	char delim[2] = ",";

	char *keywords;
	char *token, *ref_pts;
	char *ptr;
	double ret,sum;

	scanf("%u %254s %254s",&id,buf1,buf2);
	keywords = malloc(sizeof(char)*strlen(buf1)+1);
	if(keywords == NULL){
		ERR_EXIT("malloc");
	}

	memcpy(keywords, buf1, strlen(buf1));
	keywords[strlen(buf1)] = '\0';

	double* profile = malloc(sizeof(double)*NUM_OF_GENRE);
	if(profile == NULL){
		ERR_EXIT("malloc");
	}
	sum = 0;
	ref_pts = strtok(buf2,delim);
	for(int i = 0;i < NUM_OF_GENRE;i++){
		ret = strtod(ref_pts, &ptr);
		profile[i] = ret;
		sum += ret*ret;
		ref_pts = strtok(NULL,delim);
	}

	// normalize
	sum = sqrt(sum);
	for(int i = 0;i < NUM_OF_GENRE; i++){
		if(sum == 0)
				profile[i] = 0;
		else
				profile[i] /= sum;
	}

	request* r = malloc(sizeof(request));
	if(r == NULL){
		ERR_EXIT("malloc");
	}

	r->id = id;
	r->keywords = keywords;
	r->profile = profile;

	return r;
}

/*=================initialize the dataset=================*/
void initialize(FILE* fp){

	char chunk[MAX_LEN] = {0};
	char *token,*ptr;
	double ret,sum;
	int cnt = 0;

	assert(fp != NULL);

	// first row
	if(fgets(chunk,sizeof(chunk),fp) == NULL){
		ERR_EXIT("fgets");
	}

	memset(movies,0,sizeof(movie_profile*)*MAX_MOVIES);	

	while(fgets(chunk,sizeof(chunk),fp) != NULL){
		
		assert(cnt < MAX_MOVIES);
		chunk[MAX_LEN-1] = '\0';

		const char delim1[2] = " "; 
		const char delim2[2] = "{";
		const char delim3[2] = ",";
		unsigned int movieId;
		movieId = atoi(strtok(chunk,delim1));

		// title
		token = strtok(NULL,delim2);
		char* title = malloc(sizeof(char)*strlen(token)+1);
		if(title == NULL){
			ERR_EXIT("malloc");
		}
		
		// title.strip()
		memcpy(title, token, strlen(token)-1);
	 	title[strlen(token)-1] = '\0';

		// genres
		double* profile = malloc(sizeof(double)*NUM_OF_GENRE);
		if(profile == NULL){
			ERR_EXIT("malloc");
		}

		sum = 0;
		token = strtok(NULL,delim3);
		for(int i = 0; i < NUM_OF_GENRE; i++){
			ret = strtod(token, &ptr);
			profile[i] = ret;
			sum += ret*ret;
			token = strtok(NULL,delim3);
		}

		// normalize
		sum = sqrt(sum);
		for(int i = 0; i < NUM_OF_GENRE; i++){
			if(sum == 0)
				profile[i] = 0;
			else
				profile[i] /= sum;
		}

		movie_profile* m = malloc(sizeof(movie_profile));
		if(m == NULL){
			ERR_EXIT("malloc");
		}

		m->movieId = movieId;
		m->title = title;
		m->profile = profile;

		movies[cnt++] = m;
	}
	num_of_movies = cnt;

	// request
	scanf("%d",&num_of_reqs);
	assert(num_of_reqs <= MAX_REQ);
	for(int i = 0; i < num_of_reqs; i++){
		reqs[i] = read_request();
	}
}
/*========================================================*/