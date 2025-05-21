/* resource.h
 * ------------------------------------------------------
 * Vinç ve asansör gibi PAYLAŞILAN KAYNAK semaforlarının
 * tanımı, başlatma ve temizleme prototipleri.
 * ---------------------------------------------------- */

 #ifndef RESOURCE_H
 #define RESOURCE_H
 
 #include <semaphore.h>
 
 /* Semafor tanıtıcıları (global) */
 extern sem_t *crane_sem;   /* NUM_CRANES ile başlatılır  */
 extern sem_t *lift_sem;    /* ELEVATOR_CAPACITY ile     */
 
 /* Başlat ve yok et  */
 int  init_global_resources(void);
 void cleanup_global_resources(void);
 
 #endif 
 /* RESOURCE_H END */
 
 