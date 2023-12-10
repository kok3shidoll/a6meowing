#ifndef LOG_H
#define LOG_H

extern bool debug_enabled;

/* LOG macro */
#define ERROR(x, ...) \
do { \
printf("- \x1b[36m[ERR] \x1b[39m\x1b[31m"x" \x1b[39m: \x1b[35m%s()\x1b[39m\n", ##__VA_ARGS__, __FUNCTION__); \
} while(0)

#define DEVMEOW(x, ...) \
do { \
if(debug_enabled) \
printf("- \x1b[36m[DEV] \x1b[39m\x1b[34m"x" \x1b[39m: \x1b[35m%s()\x1b[39m\n", ##__VA_ARGS__, __FUNCTION__); \
} while(0)

#define MEOWWWW(x, ...) \
do { \
printf("- \x1b[36m[LOG] \x1b[39m\x1b[32m"x" \x1b[39m: \x1b[35m%s()\x1b[39m\n", ##__VA_ARGS__, __FUNCTION__); \
} while(0)

#define MEOW_NOFUNC(x, ...) \
do { \
printf(""x"\n", ##__VA_ARGS__); \
} while(0) \

#endif
