#include <pthread.h>
#include <time.h>
#include "errors.h"

// struct for alarm list
typedef struct alarm_tag
{
  struct alarm_tag *link;
  int id;
  int seconds;
  time_t time; /* seconds from EPOCH */
  char message[128];
  int changedFlag;
} alarm_t;

// struct for tracking display information
typedef struct display_details{
  int id1;
  int count1;
  int id2;
  int count2;
  int id3;
  int count3;
  int current_display;


} display_d;

//initilizing alarm mutex and display conditions
pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t display_cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t display_cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t display_cond3 = PTHREAD_COND_INITIALIZER;

//Main alarm list
alarm_t *alarm_list = NULL;
//Current alarm for use in display thread
alarm_t *current_alarm = NULL;

display_d display_details = {0,0,0,0,0,0,0};

// Takes as parameter the alarm to be added to the list,
// where alarms are ordered by their id
void Start_Alarm(alarm_t *new){
  // retain values of the head and next alarm
  alarm_t **head, *next;
  head = &alarm_list;
  next = *head;

  // cycles through list till alarm is inserted
  // into the right position depending on its id
  while(next != NULL){
      if(next->id >= new->id){
       new->link = next;
       *head = new;
      }else{
       head = &next->link;
       next = next->link;
      }
  }

  // sets the new alarm into the last
  // position in the alarm_list
  if(next == NULL){
      *head = new;
      new->link = NULL;
      new->changedFlag = 0;
  }

  // set expiration time
  new->time = time(NULL) + new->seconds;
}

// Takes as parameters the alarm_list
// and the alarm to be changed,
// and changes the corresponding alarm in the list
void Change_Alarm(alarm_t *updated){
  alarm_t *alarm = alarm_list;

  while(alarm != NULL){
      // checks if this is the alarm
      // to be updated and updates
      if(alarm->id == updated->id){
          strcpy(alarm->message, updated->message);
          alarm->seconds = updated->seconds;
          alarm->time = time(NULL) + updated->seconds;
          updated->changedFlag = 1;
      }
      //move to next alarm
      alarm = alarm->link;
  }
}

int display_selector(){
   int d1 = display_details.count1;
   int d2 = display_details.count2;
   int d3 = display_details.count3;

   if(d1<=d2 && d1<=d3 && d1<3){
       return 1;
   }else if(d2<=d1 && d2<=d3 && d2<3){
       return 2;
   }else{
       return 3;
   }
}

void *display_thread1(void *args){
  alarm_t *alarm;
  int status;
  time_t current_time;

   while(1){
     // lock
     status = pthread_mutex_lock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is unlocked");

     // waiting for alarm_thread to signal when an alarm is ready
     status = pthread_cond_wait(&display_cond1, &alarm_mutex);
     if(status != 0) err_abort(status, "Error: Waiting on condition");

     // Sets the alarm display will work with to the current alarm
     alarm = current_alarm;

    // print every 5 seconds while alarm isnt expired
     while(alarm->time > time(NULL)){
          int firstDisplayFlag = 0;

          if(alarm->changedFlag == 1){
              // Prints message when alarm is first changed
              if(firstDisplayFlag == 0){
               printf("Display Thread(%d) Has Started to Print Changed Message at %ld: %ld %s\n",
               pthread_self(),
               time(NULL),
               alarm->time,
               alarm->message);
               // change flag value for next iteration
               firstDisplayFlag = 1;   
              }
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }else{
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }
     }

     // Alarm has expired
     printf("Alarm(%d) Expired; Display Thread(%d) Stopped Printing Alarm Message at %ld: %ld %s\n",
       alarm->id,
       pthread_self(),
       time(NULL),
       alarm->time,
       alarm->message
     );
     display_details.count1--;

     // No more alarms so terminate thread?
     if(display_details.count1 == 0){
       printf("Display Thread Terminated(%d) at %ld\n", pthread_self(), time(NULL));
     } 
    
     // unlock
     status = pthread_mutex_unlock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is locked");
   }
}

void *display_thread2(void *args){
  alarm_t *alarm;
  int status;
  time_t current_time;

   while(1){
     // lock
     status = pthread_mutex_lock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is unlocked");

     // waiting for alarm_thread to signal when an alarm is ready
     status = pthread_cond_wait(&display_cond2, &alarm_mutex);
     if(status != 0) err_abort(status, "Error: Waiting on condition");

     // Sets the alarm display will work with to the current alarm
     alarm = current_alarm;

    // print every 5 seconds while alarm isnt expired
     while(alarm->time > time(NULL)){
          int firstDisplayFlag = 0;

          if(alarm->changedFlag == 1){
              // Prints message when alarm is first changed
              if(firstDisplayFlag == 0){
               printf("Display Thread(%d) Has Started to Print Changed Message at %ld: %ld %s\n",
               pthread_self(),
               time(NULL),
               alarm->time,
               alarm->message);
               // change flag value for next iteration
               firstDisplayFlag = 1;   
              }
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }else{
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }
     }

     // Alarm has expired
     printf("Alarm(%d) Expired; Display Thread(%d) Stopped Printing Alarm Message at %ld: %ld %s\n",
       alarm->id,
       pthread_self(),
       time(NULL),
       alarm->time,
       alarm->message
     );
     display_details.count2--;

     // No more alarms so terminate thread?
     if(display_details.count2 == 0){
       printf("Display Thread Terminated(%d) at %ld\n", pthread_self(), time(NULL));
     } 
    
     // unlock
     status = pthread_mutex_unlock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is locked");
   }
}

void *display_thread3(void *args){
  alarm_t *alarm;
  int status;
  time_t current_time;

   while(1){
     // lock
     status = pthread_mutex_lock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is unlocked");
     // waiting for alarm_thread to signal when an alarm is ready
     status = pthread_cond_wait(&display_cond3, &alarm_mutex);
     if(status != 0) err_abort(status, "Error: Waiting on condition");
     // Sets the alarm display will work with to the current alarm
     alarm = current_alarm;
    // print every 5 seconds while alarm isnt expired
     while(alarm->time > time(NULL)){
          int firstDisplayFlag = 0;

          if(alarm->changedFlag == 1){
              // Prints message when alarm is first changed
              if(firstDisplayFlag == 0){
               printf("Display Thread(%d) Has Started to Print Changed Message at %ld: %ld %s\n",
               pthread_self(),
               time(NULL),
               alarm->time,
               alarm->message);
               // change flag value for next iteration
               firstDisplayFlag = 1;   
              }
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }else{
              printf("Display Thread Printing Message Of Alarm(%d): %s\n",
              alarm->id,
              alarm->message);
              sleep(5);
          }
     }
     // Alarm has expired
     printf("Alarm(%d) Expired; Display Thread(%d) Stopped Printing Alarm Message at %ld: %ld %s\n",
       alarm->id,
       pthread_self(),
       time(NULL),
       alarm->time,
       alarm->message
     );
     display_details.count3--;

     // No more alarms so terminate thread?
     if(display_details.count3 == 0){
       printf("Display Thread Terminated(%d) at %ld\n", pthread_self(), time(NULL));
     } 
    
     // unlock
     status = pthread_mutex_unlock(&alarm_mutex);
     if(status != 0) err_abort(status, "Error: Mutex is locked");
   }
}


void *alarm_thread(void *arg){
   int status;
   alarm_t *alarm;
   int sleep_interval;
   time_t current_time;
   pthread_t display;

   while(1){
       // lock
       status = pthread_mutex_lock(&alarm_mutex);
       if(status != 0) err_abort(status, "Error: Mutex is unlocked");
    
       //set alarm to alarm_list
       alarm = alarm_list;

       if(alarm == NULL){
           sleep_interval = 1;
       }else{

           alarm_list = alarm->link;
           current_alarm = alarm;
           int first_time = 0;

           if(current_alarm->changedFlag == 1 && first_time == 0){
               printf("Alarm Thread(%d) Has Informed Display Thread(%d) That It Should Start Printing Changed Message At %ld: %d %s\n",
               pthread_self(),
               display,
               time(NULL),
               current_alarm->id,
               current_alarm->message);
               first_time = 1;
           }else{
               if(display_selector() == 1){
                   if(display_details.count1 == 0){
                       printf("New Display Thread %d Created at %ld\n",
                       display_details.id1,
                       time(NULL));
                       display_details.count1 = display_details.count1+1;
                   }else{
                       display_details.count1 = display_details.count1+1;
                       printf("Alarm(%d) Assigned to Display Thread(%d) at %ld: %ld %s\n",
                       alarm->id,
                       display_details.id1,
                       time(NULL),
                       alarm->time,
                       alarm->message
                       );
                   }
                   status = pthread_cond_signal(&display_cond1);
                   if (status != 0) err_abort(status, "Error: No Condition Signal");
               }else if(display_selector() == 2){
                   if(display_details.count2 == 0){
                       printf("New Display Thread %d Created at %ld\n",
                       display_details.id2,
                       time(NULL));
                       display_details.count2 = display_details.count2+1;
                   }else{
                       display_details.count2 = display_details.count2+1;
                       printf("Alarm(%d) Assigned to Display Thread(%d) at %ld: %ld %s\n",
                       alarm->id,
                       display_details.id2,
                       time(NULL),
                       alarm->time,
                       alarm->message
                       );
                   }

                   // Signal the display thread
                       status = pthread_cond_signal(&display_cond2);
                       if (status != 0) err_abort(status, "Error: No Condition Signal");
               }else{
                   if(display_details.count3 == 0){
                       printf("New Display Thread %d Created at %ld\n",
                       display_details.id3,
                       time(NULL));
                       display_details.count3 = display_details.count3+1;
                   }else{
                       display_details.count3 = display_details.count3+1;
                       printf("Alarm(%d) Assigned to Display Thread(%d) at %ld: %ld %s\n",
                       alarm->id,
                       display_details.id3,
                       time(NULL),
                       alarm->time,
                       alarm->message
                       );
                   }
                    // Signal the display thread
                    status = pthread_cond_signal(&display_cond3);
                    if (status != 0) err_abort(status, "Error: No Condition Signal");
               }
           }
       }

       // unlock
       status = pthread_mutex_unlock(&alarm_mutex);
       if(status != 0) err_abort(status, "Error: Mutex is locked");
       sleep(sleep_interval);
   }
   return 0;
}


int main(int argc, char *argv[]){
  int status;
  char line[128];
  alarm_t *alarm;
  pthread_t thread;
  pthread_t display;

  // display thread
  status = pthread_create(&thread, NULL, alarm_thread, NULL);
  if(status != 0) err_abort(status, "Create Alarm Thread");

  status = pthread_create(&display, NULL, display_thread1, NULL);
  if(status != 0) err_abort(status, "Create Display Thread 1");
  display_details.id1 = display;

  status = pthread_create(&display, NULL, display_thread2, NULL);
  if(status != 0) err_abort(status, "Create Display Thread 2");
  display_details.id2 = display;

  status = pthread_create(&display, NULL, display_thread3, NULL);
  if(status != 0) err_abort(status, "Create Display Thread 3");
  display_details.id3 = display;

  while(1){
      printf("Alarm> ");
      if (fgets(line, sizeof(line), stdin) == NULL)
          exit(0);
      if (strlen(line) <= 1)
          continue;
      alarm = (alarm_t *)malloc(sizeof(alarm_t));
      if (alarm == NULL)
          errno_abort("Allocate alarm");

      /*
      * Parse input line into seconds (%d) and a message
      * (%128[^\n]), consisting of up to 128 characters
      * separated from the seconds by whitespace.
      */
      if ((sscanf(line, "Start_Alarm(%d): %d %128[^\n]", &alarm->id, &alarm->seconds, alarm->message) < 3) && (sscanf(line, "Change_Alarm(%d): %d %128[^\n]", &alarm->id, &alarm->seconds, alarm->message) < 3))
      {
          fprintf(stderr, "Bad command\n");
          free(alarm);
          continue;
      }
      else if (!(sscanf(line, "Start_Alarm(%d): %d %128[^\n]", &alarm->id, &alarm->seconds, alarm->message) < 3))
      {
          //locks
          status = pthread_mutex_lock(&alarm_mutex);
          if (status != 0)
              err_abort(status, "Error: Mutex is unlocked");
          //Add alarm using Start_Alarm
          Start_Alarm(alarm);
          printf("Alarm(%d) Inserted by Main Thread(%d) Into Alarm list at %ld: %d %s\n",
               alarm->id, pthread_self(), time(NULL), alarm->time, alarm->message);
          //unlocks
          status = pthread_mutex_unlock(&alarm_mutex);
          if (status != 0)
              err_abort(status, "Error: Mutex is locked");
      }
      else
      {
          //locks
          status = pthread_mutex_lock(&alarm_mutex);
          if (status != 0)
              err_abort(status, "Error: Mutex is unlocked");
          //Change alarm using Change_Alarm
          Change_Alarm(alarm);
          printf("Alarm(%d) Changed at <%ld>: %d %s\n", alarm->id, time(NULL), alarm->time, alarm->message);
          //unlocks
          status = pthread_mutex_unlock(&alarm_mutex);
          if (status != 0)
              err_abort(status, "Error: Mutex is locked");
      }
       if(alarm->time >= time(NULL)){
           // alarm has expired so print message and remove it
           printf("Alarm(%d): Alarm Expired at %ld: Alarm removed from alarm list\n",
           alarm->id,
           time(NULL));
       }
  }
}
