/*
 * floor.c  – Kat (process) ve daire (thread) akışı
 * ----------------------------------------------------------
 *  1. Her daire:
 *     - Vinç ile taşıyıcı + dış duvar
 *     - Bariyer → 4 daire tamamlayınca kat stabil
 *     - Daire-0, ana sürece SIGUSR1 gönderir (dış iskelet bitti)
 *     - İç dekor adımlarını paralel yürütür
 *  2. Ana süreç SIGUSR1 alınca bir sonraki katı fork eder.
 * ---------------------------------------------------------- */

 #include "config.h"
 #include "floor.h"
 #include "resource.h"
 #include "util.h"
 
 #include <pthread.h>
 #include <signal.h>
 #include <stdlib.h>
 #include <unistd.h>
 
 /* Kat-içi ortak kilit */
 static pthread_mutex_t elec_water_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 /* ---------- Bariyer yardımcıları ---------- */
 void barrier_init(simple_barrier_t *b, int n)
 {
     b->count     = 0;
     b->threshold = n;
     pthread_mutex_init(&b->mtx, NULL);
     pthread_cond_init (&b->cv , NULL);
 }
 void barrier_wait(simple_barrier_t *b)
 {
     pthread_mutex_lock(&b->mtx);
     if (++b->count == b->threshold) {
         b->count = 0;
         pthread_cond_broadcast(&b->cv);
     } else {
         pthread_cond_wait(&b->cv, &b->mtx);
     }
     pthread_mutex_unlock(&b->mtx);
 }
 
 /* ---------- Küçük yardımcılar ---------- */
 static void use_crane(int f,int a,const char *msg)
 {
     sem_wait(crane_sem);
     log_printf(COL_CYAN,"Kat-%d Daire-%d ➜ Vinçte %s",f,a,msg);
     sleep(OUTER_SKELETON_SEC);
     sem_post(crane_sem);
 }
 static void use_elec_water(int f,int a,const char *msg,int sec)
 {
     pthread_mutex_lock(&elec_water_mutex);
     log_printf(COL_YELLOW,"Kat-%d Daire-%d ➜ %s",f,a,msg);
     sleep(sec);
     pthread_mutex_unlock(&elec_water_mutex);
 }
 static void use_lift(int f,int a,const char *msg,int sec)
 {
     sem_wait(lift_sem);
     log_printf(COL_MAGENTA,"Kat-%d Daire-%d ➜ Asansörde %s",f,a,msg);
     sleep(sec);
     sem_post(lift_sem);
 }
 static void randsleep(const char *col,int f,int a,const char *msg)
 {
     int t = MIN_DECOR_SEC + rand() % (MAX_DECOR_SEC - MIN_DECOR_SEC + 1);
     log_printf(col,"Kat-%d Daire-%d ➜ %s",f,a,msg);
     sleep(t);
 }
 
 /* ---------- Thread işlevi ---------- */
 static void *apartment_fn(void *arg)
 {
     apt_ctx_t *c = arg;
     int f = c->floor_no, a = c->apt_no;
 
     /* 1–2  Dış iskelet */
     use_crane(f,a,"taşıyıcı iskelet");
     use_crane(f,a,"dış duvar");
 
     /* Bariyer: kat stabil */
     barrier_wait(c->bar);
 
     if (a == 0)                           /* tek sefer sinyal */
         kill(getppid(), SIGUSR1);
 
     log_printf(COL_GREEN,
         "Kat-%d Daire-%d ➜ Dış-iskelet tamam, iç dekora geçiliyor",f,a);
 
     /* 3–8  İç dekorasyonu */
     use_elec_water(f,a,"Elektrik-su hattı döşeme",2);
     randsleep(COL_BLUE   ,f,a,"Sıva");
     randsleep(COL_BLUE   ,f,a,"Boya");
     use_lift   (f,a,"Fayans ve parkeler döşeme",2);
     use_lift   (f,a,"Mobilya taşıma",1);
 
     /* 9    Final kontrol */
     use_elec_water(f,a,"Final kontrol",1);
     log_printf(COL_GREEN,"Kat-%d Daire-%d TAMAMLANDI",f,a);
     return NULL;
 }
 
 /* ---------- Kat süreci ---------- */
 void run_floor(int floor_no)
 {
     log_printf(COL_MAGENTA,"Kat-%d prosesi BAŞLADI (pid=%d)",floor_no,getpid());
 
     simple_barrier_t bar;
     barrier_init(&bar, APARTMENTS_PER_FLOOR);
 
     pthread_t tid[APARTMENTS_PER_FLOOR];
     apt_ctx_t ctx[APARTMENTS_PER_FLOOR];
 
     for (int i=0;i<APARTMENTS_PER_FLOOR;++i){
         ctx[i]=(apt_ctx_t){ .floor_no=floor_no,.apt_no=i,.bar=&bar };
         pthread_create(&tid[i],NULL,apartment_fn,&ctx[i]);
     }
     for (int i=0;i<APARTMENTS_PER_FLOOR;++i)
         pthread_join(tid[i],NULL);
 
     log_printf(COL_MAGENTA,"Kat-%d TAMAMLANDI",floor_no);
 }

 
 