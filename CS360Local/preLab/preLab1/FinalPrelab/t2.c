/**************t2.c file**************/
#include <stdio.h>
#include <stdlib.h>

int *FP;

int main(int argc, char *argv[ ], char *env[ ])
{
  int a,b,c;
  printf("enter main\n");
  
  printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
  //(1). Write C code to print values of argc and argv[] entries
  while(argc>=0)
  {
    printf("%d      %s\n",argc, argv[argc]);
    argc--;
  }
  printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);
  a=1; b=2; c=3;
  A(a,b);
  printf("exit main\n");
}

int A(int x, int y)
{
  int d,e,f;
  printf("enter A\n");
  // write C code to PRINT ADDRESS OF d, e, f
  printf("&d=%8x &e=%8x &f=%8x\n", &d, &e, &f);
  d=4; e=5; f=6;
  B(d,e);
  printf("exit A\n");
}

int B(int x, int y)
{
  int g,h,i;
  printf("enter B\n");
  // write C code to PRINT ADDRESS OF g,h,i
  printf("&g=%8x &h=%8x &i=%8x\n", &g, &h, &i);
  g=7; h=8; i=9;
  C(g,h);
  printf("exit B\n");
}

int C(int x, int y)
{
  int u, v, w, i, *p;

  printf("enter C\n");
  // write C cdoe to PRINT ADDRESS OF u,v,w,i,p;
  printf("&u=%8x &v=%8x &w=%8x, &i=%8x, &p=%8x\n", &u, &v, &w, &i, &p);
  u=10; v=11; w=12; i=13;

  FP = (int *)getebp();  // FP = stack frame pointer of the C() function


  //(2). Write C code to print the stack frame link list.
  printf("FP = %x\n", FP);

  p = (int *)FP;

  printf("stack link list:\n");
  while(p)
  {
      printf("%x ->", p);
      p = *p;
  }
  printf("NULL\n");

  //(3). Print the stack contents from p to the frame of main()
    //YOU MAY JUST PRINT 128 entries of the stack contents.
  printf("stack content:\n");
  p = (int *)&u;
  int count = 0;
  while(p && count <=127)
  {
      printf("%x   %x\n", p, *p);
      p++;
      count++;
  }
  //(4). On a hard copy of the print out, identify the stack contents
    //as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
}