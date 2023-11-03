
#include "ush.h"
#include "makeargv.h"
#define MAX_BUFFER 256


int main (int argc, char *argv[])
{
    char inbuf[MAX_BUFFER];
    char **chargv;

    for(;;) {
      printf(PROMPT_STRING);
	   gets(inbuf);
	   if (strcmp(inbuf, QUIT_STRING) == 0)
          break;
	   else {
          if (fork() == 0) {
             if (makeargv(inbuf, BLANK_STRING, &chargv) > 0) {
                execvp(chargv[0], chargv);
                printf("%s: command not found.", chargv[0]);
             }
             exit(1);
          }
          wait(NULL);
      } 
    }   
    return 0;
}
