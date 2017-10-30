#include "ProducerConsumer.h"
#include <thread>
#include <sys/time.h>
#include <chrono>
#include <atomic>
using namespace std;
//TODO: add BoundedBuffer, locks and any global variables here

atomic<int> sharedP(0);
atomic<int> sharedC(0);
BoundedBuffer *buff = new BoundedBuffer (10);
int pSleep;
int cSleep;
int item_cnt;
ofstream output;
pthread_mutex_t lock;
double tBegin = 0;
atomic<int> buffer_cnt(0);

void InitProducerConsumer(int p, int c, int psleep, int csleep, int items){
	//TODO: constructor to initialize variables declared
	//also see instruction for implementation
	pthread_mutex_init(&lock, NULL);
	pthread_t producers[p];
	pthread_t consumers[c];
	cSleep = csleep;
	pSleep = psleep;
	item_cnt = items;
	output.open("output.txt");

	struct timeval t;
	gettimeofday(&t, NULL);
	tBegin = (double)t.tv_sec * 1000 + (double)t.tv_usec / 1000;
	
	for(int i = 0; i < p; i++){
		pthread_create(&producers[i], NULL, producer, (void *)(intptr_t) i);
	}

	for(int i = 0; i < c; i++){
		pthread_create(&consumers[i], NULL, consumer, (void *)(intptr_t) i);
	}

	for(int i = 0; i < p; i++){
		pthread_join(producers[i], NULL);
	}

	for(int i = 0; i < c; i++){
		pthread_join(consumers[i], NULL);
	}

	output.close();
}
// Producer #i, time = current time, producing data item #j, item value=foo
// Consumer #k, time = current time, consuming data item with value=foo
void* producer(void* threadID){
	//TODO: producer thread, see instruction for implementation
	int id = (intptr_t)threadID;
	while(1){
		this_thread::sleep_for(chrono::milliseconds(pSleep));
		if(sharedP < item_cnt){
			int add = rand() % 10;
			sharedP.fetch_add(1);
			buff->append(add);
			buffer_cnt.fetch_add(1);
			struct timeval t;
			gettimeofday(&t, NULL);
			double currentTime = (double)t.tv_sec * 1000 + (double)t.tv_usec / 1000;
			pthread_mutex_lock(&lock);
			output << "Producer #" << id << ", time = " 
			<< currentTime - tBegin << ", producing data item #" 
			<< sharedP << ", item value = " << add << endl;
			pthread_mutex_unlock(&lock);
		}
		else{
			break;
		}
	}
	return NULL;
}

void* consumer(void* threadID){
	//TODO: consumer thread, see instruction for implementation
	int id = (intptr_t)threadID;
	while(1){
		this_thread::sleep_for(chrono::milliseconds(cSleep));
		if(sharedC < buffer_cnt){
			sharedC.fetch_add(1);
			int removed = buff->remove();
			struct timeval t;
			gettimeofday(&t, NULL);
			double currentTime = (double)t.tv_sec * 1000 + (double)t.tv_usec / 1000;
			pthread_mutex_lock(&lock);
			output << "Consumer #" << id << ", time = " 
			<< currentTime - tBegin << ", consuming data item with value = " 
			<< removed << endl;
			pthread_mutex_unlock(&lock);
		}
		else if(sharedC >= item_cnt){
			break;
		}
	}
	return NULL;
}
