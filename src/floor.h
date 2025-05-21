/* floor.h
 * -----------------------------------------------------------------
 *  - Kat (process) düzeyinde ana işlev: run_floor(int floorNo)
 *  - Daire (thread) bağlamı ve apartman iş akışı
 *  - El yapımı bariyer altyapısını dışa açar
 * -----------------------------------------------------------------*/

 #ifndef FLOOR_H
 #define FLOOR_H
 
 #include <pthread.h>
 
 /* ---------- El yapımı bariyer yapısı ---------- */
 typedef struct {
     int  count;              /* Bariyere ulaşan thread sayısı   */
     int  threshold;          /* Gereken toplam thread           */
     pthread_mutex_t mtx;
     pthread_cond_t  cv;
 } simple_barrier_t;
 
 void barrier_init(simple_barrier_t *b, int n);
 void barrier_wait(simple_barrier_t *b);
 
 /* ---------- Thread bağımsız değişkeni ---------- */
 typedef struct {
     int floor_no;            /* Kat numarası (0-9)              */
     int apt_no;              /* Daire numarası (0-3)            */
     simple_barrier_t *bar;   /* Kat-içi bariyer işaretçisi      */
 } apt_ctx_t;
 
 /* Ana kat fonksiyonu (child process yalnızca bunu çağırır) */
 void run_floor(int floor_no);
 
 #endif 
 /* FLOOR_H END */
 
 