/* util.c
 * ------------------------------------------------------
 *  - clock_gettime(CLOCK_MONOTONIC) ile geçen süreyi ölçer
 *  - log_printf() tek seferde, kilitli ve renkli satır yazar
 *  - Çok-işli (multithread) çakışmalarını engeller
 * ---------------------------------------------------- */

 #include "config.h"
 #include "util.h"
 
 #include <time.h>
 #include <stdarg.h>
 #include <pthread.h>
 #include <stdio.h>
 
 static struct timespec start_ts;             /* Programın başlama anı          */
 static pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER; /* stdout kilidi   */
 
 /* ---------- Başlangıç ---------- */
 void util_init_clock(void)
 {
     clock_gettime(CLOCK_MONOTONIC, &start_ts);
 }
 
 /* Geçen süreyi MM:SS biçimine çevirir */
 static void elapsed_to_string(char buf[8])
 {
     struct timespec now;
     clock_gettime(CLOCK_MONOTONIC, &now);
 
     long sec = now.tv_sec - start_ts.tv_sec;
     int  mm  = (int)(sec / 60);
     int  ss  = (int)(sec % 60);
 
     snprintf(buf, 8, "%02d:%02d", mm, ss);
 }
 
 /* ---------- Ana log fonksiyonu ---------- */
 void log_printf(const char *color, const char *fmt, ...)
 {
     char ts[8];
     elapsed_to_string(ts);
 
     /* Mesaj gövdesini önce bir buffer’da oluştur (thread-local) */
     char msg[256];
     va_list ap;
     va_start(ap, fmt);
     vsnprintf(msg, sizeof(msg), fmt, ap);
     va_end(ap);
 
     /* stdout’a ATOMİK yaz: renk + ts + mesaj + reset + \n */
     pthread_mutex_lock(&log_mtx);
     fprintf(stdout, "%s[%s] %s%s\n",
             (color ? color : ""),      /* Başlangıç rengi (veya boş) */
             ts,                        /* [MM:SS]                    */
             msg,                       /* Mesaj gövdesi              */
             COL_RESET);                /* Renk sıfırlama             */
     fflush(stdout);
     pthread_mutex_unlock(&log_mtx);
 }
 
 /* log_printf() notları
    - color NULL ise renk kodu basılmaz (varsayılan terminal rengi)
    - fmt: klasik printf format dizgesi (%s, %d, %f, …)
    - %n kullanmayın (güvenlik kısıtı)                                  
 */
 
 