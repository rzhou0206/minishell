#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int Maxchar = 1024;
char prompt[]="sh>";

void eval(char *cmdline) {

  int maxnum = 1024;
  char *cmdchar[maxnum];
  char *cmdstring=strtok(cmdline," ");
  int i=0;
  while (cmdstring){
    cmdchar[i] = cmdstring;
    cmdstring=strtok(NULL," ");
    i++;
  }
  int value= fork();
  if (value) {
    wait(NULL);
  } else {
    execvp(cmdchar[0], cmdchar);
  }
  
}

int main(int argc, char** argv) {
  char cmdline[Maxchar];
  while (1) {
    printf("%s", prompt);

    if ((fgets(cmdline, Maxchar, stdin)== NULL) && ferror(stdin))
      error("wrong input");

    if (feof(stdin)){
      printf("\n");
      exit(0);
    }
    cmdline[strlen(cmdline)-1]='\0';

    eval(cmdline);
  }  
}

