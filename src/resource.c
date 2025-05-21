/* resource.c
 * ------------------------------------------------------
 * POSIX *named* semaforlarla vinç (crane) ve asansör
 * (lift) kaynaklarını yönetir.
 *  - named = macOS arm64'te process-paylaşımlı garanti
 *  - init -> sem_open(O_CREAT)
 *  - cleanup -> sem_close + sem_unlink
 * ---------------------------------------------------- */

 #include "config.h"
 #include "resource.h"
 #include <fcntl.h>      /* O_CREAT, O_EXCL           */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <errno.h>
 
 #define CRANE_SEM_NAME "/crane_sem_aptsim"
 #define LIFT_SEM_NAME  "/lift_sem_aptsim"
 
 sem_t *crane_sem = NULL;
 sem_t *lift_sem  = NULL;
 
 static void unlink_if_exists(const char *name)
 {
     /* Önceki çakılı kapanışlardan kalan nesneyi sil */
     sem_unlink(name);
 }
 
 int init_global_resources(void)
 {
     /* Çakışma ihtimaline karşı eski tanımları temizle */
     unlink_if_exists(CRANE_SEM_NAME);
     unlink_if_exists(LIFT_SEM_NAME);
 
     /* 0600 => sahibi okuyup yazabilir */
     crane_sem = sem_open(CRANE_SEM_NAME, O_CREAT | O_EXCL, 0600, NUM_CRANES);
     if (crane_sem == SEM_FAILED) {
         perror("sem_open(crane_sem)");
         return -1;
     }
 
     lift_sem  = sem_open(LIFT_SEM_NAME,  O_CREAT | O_EXCL, 0600,
                          ELEVATOR_CAPACITY);
     if (lift_sem == SEM_FAILED) {
         perror("sem_open(lift_sem)");
         sem_close(crane_sem);
         sem_unlink(CRANE_SEM_NAME);
         return -1;
     }
 
     return 0;
 }
 
 void cleanup_global_resources(void)
 {
     /* Sıraya dikkat: önce close, sonra unlink */
     if (crane_sem && crane_sem != SEM_FAILED) {
         sem_close(crane_sem);
         sem_unlink(CRANE_SEM_NAME);
     }
     if (lift_sem && lift_sem != SEM_FAILED) {
         sem_close(lift_sem);
         sem_unlink(LIFT_SEM_NAME);
     }
 }
 
 