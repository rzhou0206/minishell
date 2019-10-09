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

void eval(char *cmdline) {

  int maxnum = 1024;
  char *cmdchar[maxnum];
  char *cmdstring=strtok(cmdline," ");
  int i=0;
  while (cmdstring){
    if(cmdstring[0] == '$') {
      if(cmdstring[1] == '$') {
	int pid1 = getpid();
	cmdchar[i] = malloc(10);
	sprintf(cmdchar[i], "%d", pid1);
      } else {
	char* env = getenv(cmdstring+1);
	if(env) {
	  cmdchar[i] = malloc(strlen(env));
	  strcpy(cmdchar[i], env);
	} else {
	  cmdchar[i] = cmdstring;
	}
      }
    } else {
      cmdchar[i] = cmdstring;
    }
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
  } else if(i>0 && !strcmp(cmdchar[0],"clone")){
    char* stack;
    char* stackTop;
    stack = malloc(1024 * 1024);
    stackTop = stack + 1024 * 1024;
    pid_t pid;
    int options = 0;
    int i = 1;
    //printf("searching for arguments\n");
    while(cmdchar[i]) {
      if(!strcmp(cmdchar[i],"ipc")) {
  	options |= CLONE_NEWIPC;
      } else if(!strcmp(cmdchar[i], "childtid")) {
  	options |= CLONE_CHILD_CLEARTID;
      } else if(!strcmp(cmdchar[i], "childsettid")) {
  	options |= CLONE_CHILD_SETTID;
      } else if(!strcmp(cmdchar[i], "files")) {
  	options |= CLONE_FILES;
      } else if(!strcmp(cmdchar[i], "fs")) {
  	options |= CLONE_FS;
      } else if(!strcmp(cmdchar[i], "io")) {
  	options |= CLONE_IO;
      } else if(!strcmp(cmdchar[i], "net")) {
  	options |= CLONE_NEWNET;
      } else if(!strcmp(cmdchar[i], "ns")) {
  	options |= CLONE_NEWNS;
      } else if(!strcmp(cmdchar[i], "pid")) {
  	options |= CLONE_NEWPID;
      } else if(!strcmp(cmdchar[i], "user")) {
  	options |= CLONE_NEWUSER;
      } else if(!strcmp(cmdchar[i], "uts")) {
  	options |= CLONE_NEWUTS;
      } else if(!strcmp(cmdchar[i], "parent")) {
  	options |= CLONE_PARENT;
      } else if(!strcmp(cmdchar[i], "parentsettid")) {
  	options |= CLONE_PARENT_SETTID;
      } else if(!strcmp(cmdchar[i], "ptrace")) {
  	options |= CLONE_PTRACE;
      } else if(!strcmp(cmdchar[i], "settls")) {
  	options |= CLONE_SETTLS;
      } else if(!strcmp(cmdchar[i], "sighand")) {
  	options |= CLONE_SIGHAND;
      } else if(!strcmp(cmdchar[i], "sysvsem")) {
  	options |= CLONE_SYSVSEM;
      } else if(!strcmp(cmdchar[i], "thread")) {
  	options |= CLONE_THREAD;
      } else if(!strcmp(cmdchar[i], "untraced")) {
  	options |= CLONE_UNTRACED;
      } else if(!strcmp(cmdchar[i], "vfork")) {
  	options |= CLONE_VFORK;
      } else if(!strcmp(cmdchar[i], "vm")) {
  	options |= CLONE_VM;
      }
      i++;
    }
    pid = clone(main, stackTop, options | SIGCHLD, cmdchar[0]);
    if(pid == -1) {
      perror("This is the reason why clone failed::");
      free(stack);
      return;
    }
    waitpid(pid, NULL, 0);
  } else if(i > 0 && !strcmp(cmdchar[0], "exit")) {
    exit(0);
  } else {
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
    //return 0;
  }  
}

