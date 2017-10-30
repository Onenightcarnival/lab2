#include "BoundedBuffer.h"
#include <pthread.h>

BoundedBuffer::BoundedBuffer(int N){
	//TODO: constructor to initiliaze all the varibales declared in BoundedBuffer.h
	buffer = new int[N]();
	buffer_size = N;
	buffer_cnt = 0;
	buffer_last = N - 1;
	pthread_cond_init(&buffer_full, NULL);
	pthread_cond_init(&buffer_empty, NULL);
	pthread_mutex_init(&buffer_lock, NULL);
}


void BoundedBuffer::append(int data){
	//TODO: append a data item to the circular buffer
	pthread_mutex_lock(&buffer_lock);
	while(buffer_cnt == buffer_size){
		pthread_cond_wait(&buffer_full, &buffer_lock);
	}
	buffer_last = (buffer_last + 1) % buffer_size;
	buffer[buffer_last] = data;
	buffer_cnt ++;
	pthread_cond_signal(&buffer_empty);
	pthread_mutex_unlock(&buffer_lock);
}

int BoundedBuffer::remove(){
	//TODO: remove and return a data item from the circular buffer
	pthread_mutex_lock(&buffer_lock);
	while(buffer_cnt == 0){
		pthread_cond_wait(&buffer_empty, &buffer_lock);
	}
	if((buffer_last - (buffer_cnt - 1)) % buffer_size < 0){
		int removed = buffer[(buffer_last - (buffer_cnt - 1)) % buffer_size + buffer_size];
		buffer_cnt --;
		pthread_cond_signal(&buffer_full);
		pthread_mutex_unlock(&buffer_lock);
		return removed;
	}
	else{
		int removed = buffer[(buffer_last - (buffer_cnt - 1)) % buffer_size];
		buffer_cnt --;
		pthread_cond_signal(&buffer_full);
		pthread_mutex_unlock(&buffer_lock);
		return removed;
	}
}

bool BoundedBuffer::isEmpty(){
	//TODO: check is the buffer is empty
	if(buffer_cnt == 0){
		return true;
	}
	else{
		return false;
	}
}
