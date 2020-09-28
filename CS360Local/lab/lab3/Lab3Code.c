/************** lab3base.c file **************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define MAX 128

char gdir[MAX];    // gdir[ ] stores dir strings
char *dir[64];
int  ndir;

char gpath[MAX];   // gpath[ ] stores token strings
char *name[64];
int  ntoken;

int main(int argc, char *argv[], char *env[])
{
  int  i, r;
  int  pid, status;
  char *s, cmd[64], line[MAX];

  printf("************* Welcome to kcsh **************\n");
  i = 0;
  while (env[i]){
    printf("env[%d] = %s\n", i, env[i]);
    
    // Looking for PATH=
    if (strncmp(env[i], "PATH=", 5)==0){
      printf("show PATH: %s\n", env[i]);

      printf("decompose PATH into dir strings in gdir[ ]\n");
      strcpy(gdir, &env[i][5]);

      /*************** 1 ******************************
      Write YOUR code here to decompose PATH into dir strings in gdir[ ]
      pointed by dir[0], dir[1],..., dir[ndir-1]
      ndir = number of dir strings
      print dir strings
      ************************************************/
      break;
    }
    i++;
  }
  
  printf("*********** kcsh processing loop **********\n");

  while(1){
     printf("kcsh % : ");

     fgets(line, 128, stdin);
     line[strlen(line)-1] = 0;      // fgets() has \n at end

     if (line[0]==0)
       continue;
     printf("line = %s\n", line);   // print line to see what you got

     /***************** 2 **********************
      Write YOUR code here to decompose line into token strings in gpath[ ]
      pointed by name[0], name[1],..., name[ntoken-1]
      ntoken = number of token strings
      print the token strings
      ************************************************/
     
     // 3. Handle name[0] == "cd" or "exit" case

     // 4. name[0] is not cd or exit:
     pid = fork();   // fork a child sh

     if (pid){
        printf("parent sh %d waits\n", getpid());
        pid = wait(&status);
        printf("child sh %d died : exit status = %04x\n", pid, status);
        continue;
     }
     else{
        printf("child sh %d begins\n", getpid());
        for (i=0; i<ndir; i++){
	  	  
	   strcpy(cmd, dir[i]); strcat(cmd, "/"); strcat(cmd, name[0]);
	   printf("i=%d cmd=%s\n", i, cmd);
	   
	   r = execve(cmd, name, env);
	}
	printf("cmd %s not found, child sh exit\n", name[0]);
	exit(123);   // die with value 123
     }
  }
}

/*********************** 5 *********************
Write your code to do I/O redirection:
Example: check any (name[i] == ">"). 
         If so, set name[i] = 0; 
                redirecct stdout to name[i+1] 
********************************************/

/******************* 6 ***********************
 Handle pipe: check pipe symbol | in input line;
 if so, divide line into head, tail

 create PIPE, fork a child to share the pipe
 parent write to  pipe and exec head;
 child  read from pipe and exec tail
********************************************/