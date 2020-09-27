#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>  // Ubuntu User: sudo apt install e2fslibs-dev

#include <sys/types.h>
#include <unistd.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
	u8 drive;		/* 0x80 - active */
	u8 head;		/* starting head */
	u8 sector;		/* starting sector */
	u8 cylinder;		/* starting cylinder */
	u8 sys_type;		/* partition type */
	u8 end_head;		/* end head */
	u8 end_sector;	/* end sector */
	u8 end_cylinder;	/* end cylinder */
	u32 start_sector;	/* starting sector counting from 0 */
	u32 nr_sectors;       /* nr of sectors in partition */
};

char *dev = "vdisk";
int fd;

int read_sector(int fd, int sector, char *buf)
{
    // same as shown above
    int n;
    lseek(fd, sector*512, SEEK_SET);
    n = read(fd, buf, 512);
    if(n<=0)
    {
      printf("read failed\n");
      return -1;
    }
    return n;
}

int main()
{
  struct partition *p;
  char buf[512];

  fd = open(dev, O_RDONLY);   // open dev for READ

   read(fd, buf, 512);        // read MBR into buf[]
   //read_sector(fd, 0, buf);    // OR call read_sector()    
   p = (struct partition *)(&buf[0x1be]); // p->P1
   struct partition *p2 = p;
   printf("     Start  #Sectors  SysType\n");
  
   // Write YOUR code to print all 4 partitions
   for(int secNum = 0; secNum < 4; secNum++)
   {
     printf("%8d %8d %8x\n", p2->start_sector, p2->nr_sectors, p2->sys_type);
     p2++;
   }
    
   // ASSUME P4 is EXTEND type; 
   p += 3;      // p-> P4
   if(p->sys_type == 5)
   {
      int p4_start_sector = p->start_sector;
      printf("P4 start_sector = %d\n", p->start_sector);
      read_sector(fd, p4_start_sector, buf);//read p4
      p = (struct partition *)&buf[0x1BE];  // p->localMBR
      printf("FIRST entry of localMBR\n");
      printf("start_sector = %d, nsector = %d\n", p->start_sector, p->nr_sectors);
      // Write YOUR code to get 2nd entry, which points to the next localMBR, etc.
      // NOTE: all sector numbers are relative to P4's start_sector
      printf("start_sector    nsectors    ID\n");
      //while not the end of list
      while(p->sys_type!=0)
      {
        printf("%8d     %8d %8x\n", p->start_sector, p->nr_sectors, p->sys_type);
        
        p++;//increment p to next partation start sector
        if(p->sys_type!=0)
        {
          read_sector(fd, p4_start_sector+p->start_sector, buf);//read p5,p6,p7...
          p = (struct partition *)&buf[0x1BE];//p->localMBR
        }
        
      }
   }
   close(fd);
}
