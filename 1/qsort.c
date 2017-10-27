for (i=my_start_index; i<my_start_index + my_length; i++){
 if (a[i] < head -> pivot)
 less ++;
 else
 greater ++;
}
pthread_mutex_lock(&(head->sum_lock));
head -> less_than += less; 
/* --- Critical section -
 Each thread updates the
 global value less_than --- */
pthread_mutex_unlock(&(head->sum_lock));
barrier(&(head -> barrier2), head -> numthreads);
/* -- Barrier call
 to wait for all
 threads to update
 value less_than ---*/


for (i=0; i<NUM_THREADS; i++)
{
 pthread_create(&sort_threads[i], NULL, sort_thread,(void *)
&workspace[i]);
}
void *sort_thread(void *ptr)
{
barrier(&(head -> barrier1), head -> numthreads);
(void)pthread_once(&once_ctr, once_rtn);