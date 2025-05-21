/* main.c
 * -----------------------------------------------------------
 * Kat-düzeyinde senkron stratejisi:
 *   • Kat-0 hemen fork edilir.
 *   • Her kat, DIŞ-İSKELETİ tamamladığında ana sürece SIGUSR1 gönderir.
 *   • Ana süreç sinyali alınca bir sonraki katı fork eder (vinç sırasını korur).
 *   • Bütün katlar fork edildikten sonra hepsinin bitmesini waitpid() ile bekler.
 * --------------------------------------------------------- */

 #include "config.h"
 #include "floor.h"
 #include "resource.h"
 #include "util.h"
 
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/wait.h>
 #include <unistd.h>
 
 static volatile sig_atomic_t next_floor_ready = 0; /* SIGUSR1 bayrağı */
 static pid_t pids[NUM_FLOORS] = {0};
 
 /* — SIGUSR1: kat dış-iskeleti tamam — */
 static void sigusr1_handler(int sig) {
     (void)sig;                 /* kullanılmadı */
     next_floor_ready = 1;
 }
 
 static pid_t fork_floor(int floor_no) {
     pid_t pid = fork();
     if (pid == -1) {
         perror("fork");
         exit(1);
     }
     if (pid == 0) {            /* ÇOCUK ─ kat süreci */
         srand(getpid());
         run_floor(floor_no);
         cleanup_global_resources();   /* kendi referanslarını kapatır */
         exit(0);
     }
     return pid;                /* EBEVEYN ─ PID saklanır */
 }
 
 int main(void)
 {
     /* Zamanlayıcı, sinyaller, semaforlar -------------------------------- */
     util_init_clock();
 
     struct sigaction sa = {0};
     sa.sa_handler = sigusr1_handler;
     sigaction(SIGUSR1, &sa, NULL);
 
     if (init_global_resources() != 0) return 1;
 
     /* 1) Kat-1’ı başlat -------------------------------------------------- */
     int floor_next = 0;
     pids[floor_next] = fork_floor(floor_next);
     ++floor_next;
 
     /* 2) Her SIGUSR1’de sonraki katı fork et ---------------------------- */
     while (floor_next < NUM_FLOORS) {
         pause();                           /* SIGUSR1 bekle */
         if (next_floor_ready) {
             pids[floor_next] = fork_floor(floor_next);
             ++floor_next;
             next_floor_ready = 0;
         }
     }
 
     /* 3) Tüm kat süreçlerinin tamamını bekle --------------------------- */
     for (int i = 0; i < NUM_FLOORS; ++i) {
         waitpid(pids[i], NULL, 0);
     }
 
     cleanup_global_resources();
     log_printf(COL_GREEN, "BİNA İNŞAATI TAMAMLANDI 🎉");
     return 0;
 }
 
