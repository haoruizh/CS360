#include <stdio.h>  // for I/O
#include <stdlib.h> // for lib functions
#include <libgen.h> // for dirname()/basename()
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

/************** lab3base.c file **************/
#define MAX 128

char gdir[MAX]; // gdir[ ] stores dir strings
char *dir[64];
int ndir;

char gpath[MAX]; // gpath[ ] stores token strings
char *name[64];
int ntoken = 0;
char *home;

void runCmd(char *cmdName[],char **env,char cmd[64])
{
  int r;
  for (int x = 0; x < ndir; x++)
  {
    strcpy(cmd, dir[x]);
    strcat(cmd, "/");
    strcat(cmd, cmdName[0]);
    printf("i=%d cmd=%s\n", x, cmd);
    r = execve(cmd,cmdName,env);
  }
  //free(cmd);
}

int reDic()
{
  /*********************** 5 *********************
  Write your code to do I/O redirection:
  Example: check any (name[i] == ">"). 
         If so, set name[i] = 0; 
                redirecct stdout to name[i+1] 
  ********************************************/
  for (int i = 0; name[i] != NULL; i++)
  {
    if (strcmp(name[i], ">") == 0)
    {
      name[i] = 0;
      close(1);
      return (open(name[i + 1], O_WRONLY | O_CREAT, 0644));
    }
    else if (strcmp(name[i], "<") == 0)
    {
      name[i] == 0;
      close(0);
      return (open(name[i + 1], O_RDONLY));
    }
    else if (strcmp(name[i], ">>") == 0)
    {
      name[i] == 0;
      close(1);
      return (open(name[i + 1], O_WRONLY | O_APPEND, 0644));
    }
  }
}

/******************* 6 ***********************
 Handle pipe: check pipe symbol | in input line;
 if so, divide line into head, tail

 create PIPE, fork a child to share the pipe
 parent write to  pipe and exec head;
 child  read from pipe and exec tail
********************************************/
void processPipe(char *head[], char *tail[], char **env, char *cmd)
{
  int pid;
  int pd[2];
  pipe(pd);     // creates a PIPE
  pid = fork(); // fork a child (to share the PIPE)
  if (pid)
  {               
    printf("Executing head\n");// parent as pipe WRITER
    close(pd[0]);            // WRITER MUST close pd[0]
    close(1);                // close 1
    dup(pd[1]);            // replace 1 with pd[1]
    close(pd[1]);            // close pd[1]
    runCmd(head,env,cmd); // change image to cmd1
    free(cmd);
  }
  else
  {       
    printf("Executing tail\n");        // child as pipe READER
    close(pd[1]); // READER MUST close pd[1]
    close(0);
    dup(pd[0]);             // replace 0 with pd[0]
    close(pd[0]);          // close pd[0]
    runCmd(tail,env,cmd); // change image to cmd2
    free(cmd);
  }
}

void handlePipe(char* cmd, char *cmd1[], char *cmd2[], int symbolInd, char **env)
{
  int i = 0, j = 0, k = 0;

  //get head
  printf("Head:");
  while (i < symbolInd)
  {
    printf("%s ", name[i]);
    cmd1[j++] = name[i++];
  }
  printf("\n");
  //get tail
  i++;
  printf("-----------------------\n");
  printf("Tail:");
  while (name[i])
  {
    printf("%s ", name[i]);
    cmd2[k++] = name[i++];
  }
  printf("\n");

  processPipe(cmd1,cmd2,env,cmd);
}

void processCmd(char *env[], int pid, int status, char* cmd)
{
  int ifPipe = 0, symbolIndex;
  char *head[128], *tail[128];
  // check if there is pipe symbol
  for (int i = 0; name[i]; i++)
  {
    if (strcmp(name[i], "|") == 0)
    {
      ifPipe = 1;
      symbolIndex = i;
      break;
    }
  }
  // simple child process
  pid = fork(); // fork a child sh
  if (pid)
  {
    printf("parent sh %d waits\n", getppid());
    pid = wait(&status);
    printf("child sh %d died : exit status = %04x\n", pid, status);
    return;
  }
  else
  {
    printf("Enter child process:\n");
    printf("child sh %d begins\n", getpid());
    printf("NDIR: %d\n", ndir);
    if(ifPipe!=0)
    {
      printf("Handling if cmd is pipe:\n");
      handlePipe(cmd, head, tail, symbolIndex, env);
    }
    reDic();
    runCmd(name,env,cmd);
    printf("cmd %s not found, child sh exit\n", name[0]);
    exit(123); // die with value 123
  }
}


int main(int argc, char *argv[], char *env[])
{
  int i, r;
  int x;
  int pid, status;
  char *s, cmd[64], line[MAX];

  printf("************* Welcome to kcsh **************\n");
  i = 0;

  while (env[i])
  {
    printf("env[%d] = %s\n", i, env[i]);

    // Looking for PATH=
    if (strncmp(env[i], "PATH=", 5) == 0)
    {
      printf("show PATH: %s\n", env[i]);

      printf("decompose PATH into dir strings in gdir[ ]\n");
      strcpy(gdir, &env[i][5]);

      /*************** 1 ******************************
      Write YOUR code here to decompose PATH into dir strings in gdir[ ]
      pointed by dir[0], dir[1],..., dir[ndir-1]
      ndir = number of dir strings
      print dir strings
      ************************************************/
      char *dirName;
      dirName = strtok(gdir, ":");
      while (dirName)
      {
        dir[ndir] = dirName;
        printf("%s/", dir[ndir]);
        dirName = strtok(0, ":");
        ndir++;
      }
      printf("\n");
      break;
    }
    i++;
  }

  printf("*********** kcsh processing loop **********\n");

  while (1)
  {
    printf("kcsh %% : ");

    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0; // fgets() has \n at end

    if (line[0] == 0)
      continue;
    printf("line = %s\n", line); // print line to see what you got

    /***************** 2 **********************
    Write YOUR code here to decompose line into token strings in gpath[ ]
    pointed by name[0], name[1],..., name[ntoken-1]
    ntoken = number of token strings
    print the token strings
    ************************************************/
    char *cmdName;
    cmdName = strtok(line, " ");
    printf("name:");
    ntoken = 0;
    while (cmdName)
    {
      name[ntoken] = cmdName;
      cmdName = strtok(0, " ");
      printf("%s ", name[ntoken]);
      ntoken++;
    }
    ntoken--;
    printf("\n");

    // 3. Handle name[0] == "cd" or "exit" case
    if (strcmp(name[0], "cd") == 0)
    {
      char s[100];
      if (name[1] != 0)
      {
        chdir(name[1]);
        printf("%s\n", getcwd(s, 100));
      }
      else
      {
        for (int j = 0; env[j] != 0; j++)
        {
          if (strncmp(env[j], "HOME=", 5) == 0)
          {
            home = strtok(env[j], "=");
            home = strtok(NULL, "=");
            break;
          }
        }
        chdir(home);
        printf("%s\n", getcwd(s, 100));
      }
    }
    else if (strcmp(name[0], "exit") == 0)
    {
      printf("Exiting the program!\n");
      exit(1);
    }
    else
    {
      processCmd(env, pid, status,cmd);
      continue;
    }
  }
}