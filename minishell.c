#define _GNU_SOURCE

#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <errno.h>

int Maxchar = 1024;
char prompt[]="sh>";

int main();

int cloned(void* args) {
  printf("Hello World\n");
  return 0;
}

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
  cmdchar[i] = NULL;

  if(i > 0 && !strcmp(cmdchar[0], "cd")) {
    if(i == 1) {
      chdir(getenv("HOME"));
    } else if(i == 2) {
      chdir(cmdchar[1]);
    } else {
      printf("cd: too many arguments\n");
    }
  }
  if(i>0 && !strcmp(cmdchar[0],"clone")){
    if(i==1) {
      printf("usage: clone arg1\n arg1 must be one of newipc ,newuts, newuser, newnet, newns\n");
    }
    if(i==2){
      char* stack;
      char* stackTop;
      stack = malloc(1024 * 1024);
      stackTop = stack + 1024 * 1024;
      int pid = -1;
      int options = 0;
      int i = 1;
      printf("searching for arguments\n");
      while(cmdchar[i]) {
	if(!strcmp(cmdchar[i],"newipc")) {
	  options |= CLONE_NEWIPC;
	}
	printf("at i = %d, found %s\n", i, cmdchar[i]);
	i++;
      }
      printf("running clone\n");
      pid = clone(cloned, stackTop, options | SIGCHLD, cmdchar[0]);
      printf("clone returned %d\n", pid);
      perror("");
    }
  }
  else {
    char* env = malloc(strlen(getenv("PATH")));
    strcpy(env, getenv("PATH"));
    char* token = strtok(env, ":");
    while(token) {
      char *file = malloc(strlen(token)+strlen(cmdchar[0])+1);
      strcat(file, token);
      strcat(file,"/");
      strcat(file, cmdchar[0]);
      if(access(file, X_OK) != -1) {
	int value= fork();
	if (value) {
	  wait(NULL);
	} else {
	  execvp(file, cmdchar);
	}
	return;
      }
      token = strtok(NULL, ":");
    }
    printf("Command '%s' not found.\n", cmdchar[0]);
  }
}

int main() {
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

