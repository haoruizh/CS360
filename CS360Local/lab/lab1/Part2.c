//Part2
#include<stdio.h>
typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10; 

int rpu(u32 x)
{  
    char c;
    if (x){
       c = ctable[x % BASE];
       rpu(x / BASE);
       putchar(c);
    }
}

//print unsigned integer
int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

//print an integer(x can be negative)
int  printd(int x)
{
    BASE = 10;
    if(x == 0)
    {
        putchar('0');
    }
    else if(x < 0)
    {
        putchar('-');
        rpu(-x);
    }
    else
    {
        rpu(x);
    }
    
}

//print x in HEX
int  printx(u32 x)
{
    BASE = 16;
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

//print x in Octal
int  printo(u32 x)
{
    BASE = 8;
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

//print char x
int printc(char x)
{
    putchar(x);
}

//print string x
int prints(char* x)
{
    while(*x)
    {
        putchar(*x);
        x++;
    }
}

int myPrintf(char *fmt, ...)
{
    char *cp = fmt;
    int *ip =(int *)&fmt;
    ip++;
    while(*cp)
    {
        if(*cp == '%')
        {
            cp ++; //read next char
            switch (*cp)
            {
                case 'd' : //integer
                    printd(*ip);
                    break;
                case 's' : //string
                    prints((char *)*ip);
                    break;
                case 'u' : //
                    printu(*ip);
                    break;
                case 'c' : //char
                    printc(*ip);
                    break;
                case 'o' : 
                    printo(*ip);
                    break;
                case 'x' :
                    printx(*ip);
                    break;
                default:
                    putchar(*ip);
                    break;
            }
            ip++;
        }
        else if(*cp == '\n')//escape symbol
        {
            putchar('\r');
        }
        else //regular char and symbol
        {
            putchar(*cp);
        }
        cp++;
    }
}

int main(int argc, char* argv[], char *env[])
{
    myPrintf("%d\n", argc);
    while(argc)
    {
        myPrintf("%s\n", argv[argc-1]);
        argc--;
    }
    
    myPrintf("%d\n", -5);
    myPrintf("char = %c string = %s   dec = %d hex = %x  oct=%o neg=%d\n", 'A', "this is a test", 100, 100, 100, -100);
}