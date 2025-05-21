/* util.h
 * ------------------------------------------------------
 *  - Zaman damgası (MM:SS) üreten yardımcı fonksiyon
 *  - Renkli, timestamp'li log çıktısı
 * ---------------------------------------------------- */

 #ifndef UTIL_H
 #define UTIL_H
 
 #include <stdio.h>
 
 /* Program başlatılınca çağrılmalı */
 void util_init_clock(void);
 
 /* Renkli ve zaman damgalı printf (ANSI renk kodu ya da NULL) */
 void log_printf(const char *color, const char *fmt, ...)
     __attribute__((format(printf, 2, 3)));
 
 #endif 
 /* UTIL_H END */

