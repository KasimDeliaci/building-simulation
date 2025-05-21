/* config.h – tüm değilken ve sayılar burada */

#ifndef CONFIG_H
#define CONFIG_H

/* Yapı parametreleri */
#define NUM_FLOORS             10
#define APARTMENTS_PER_FLOOR    4

/* Paylaşılan kaynaklar */
#define NUM_CRANES              1   /* vinç adedi (global) */
#define ELEVATOR_CAPACITY       3   /* asansör aynı anda kaç daireye hizmet edebilir */

/* Süreler (saniye) */
#define OUTER_SKELETON_SEC      3   /* dış-iskelet adımı sabit */
#define MIN_DECOR_SEC           1   /* iç dekor rastgele aralık */
#define MAX_DECOR_SEC           3

/* Loglama rengi (ANSI) */
#define COL_RESET  "\033[0m"
#define COL_CYAN   "\033[36m"
#define COL_GREEN  "\033[32m"
#define COL_YELLOW "\033[33m"
#define COL_BLUE   "\033[34m"
#define COL_MAGENTA "\033[35m"
#define COL_RED    "\033[31m"

#endif 

/* CONFIG_H END */
