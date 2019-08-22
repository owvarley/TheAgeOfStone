
/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF		32768
#define MAX_BUF_LIST 	11
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2
