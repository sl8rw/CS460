/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
GD *gp;
INODE *ip;
DIR *dp;
u32 *up;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

u16 search(INODE *ip, char *name)
{
  u8 i;
  char c;
  DIR *dp;
  for (i = 0; i < 12; i++)
  {
    if ((u16)ip->i_block[i])
    {
      getblk((u16)ip->i_block[i], buf2);
      dp = (DIR *)buf2;
      while ((char *)dp < &buf2[BLK])
      {
        c = dp->name[dp->name_len]; //saves last byte
        dp->name[dp->name_len] = 0; //conversion to str
        prints(dp->name);
        putc(' ');
        if (strcmp(dp->name, name) == 0) //found
        {
          prints("\n\r");
          return ((u16)dp->inode);
        }
        dp->name[dp->name_len] = c; //restores the last byte
        dp = (char *)dp + dp->rec_len;
      }
    }
    error();
  }
}
main()
{
  //DO NOT EXCEED 1K LIMIT
  u16 i, iblk;
  char c;             //temp[64];
  char *cp, *name[2]; //filename[64];
  name[0] = "boot";
  //name[1]=filename;
  //prints("bootname: ");
  //gets(filename);
  //if (filename[0]==0) 
  name[1]="mtx";
  // prints("read block# 2 (GD)\n\r"); //taken from page 64 in MTX
  getblk(2, buf1);                  //reads block 2 to get the group descriptor 0
  gp = (GD *)buf1;
  // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  iblk = (u16)gp->bg_inode_table;
  getblk(iblk, buf1); //reads first inode block
  // prints("inode_block=");
  // putc(iblk + '0');
  // prints("\n\r");

  // 2. WRITE YOUR CODE to get root inode
  ip = (INODE *)buf1 + 1; //ip->root inode 2
  // prints("read inodes begin block to get root inode\n\r");

  // 3. WRITE YOUR CODE to step through the data block of root inode
  for (i = 0; i < 2; i++)
  {
    ino = search(ip, name[i]) - 1;
    if (ino < 0) error();
    getblk(iblk + (ino / 8), buf1); //read inode block of ino
    ip = (INODE *)buf1 + (ino % 8);
  }
  // prints("read data block of root DIR\n\r");

  if ((u16)ip->i_block[12]) //indirect blocks
  {
    getblk((u16)ip->i_block[12], buf2);
  }
  setes(0x1000); //assembly code to set ES to loading segment
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], 0);
    inces();
    // putc('*'); //show a * for each direct block
  }
  if ((u16)ip->i_block[12]) //load indirect block
  {
    up = (u32 *)buf2;
    while (*up)
    {
      getblk((u16)*up, 0);
      inces();
      up++; //this important vs sticking in while loop
      // putc('.'); //show . for each indrect block
    }
  }
  // 4. print file names in the root directory /
  prints("ready?\n");getc();
}

int prints(char *s)
{
  char c;
  while (*s != '\0')
  { 
    c = *s;  //assigns a character to c
    putc(c); //call to putc
    *s++;    //ptr increment
  }
}

int gets(char *s)
{
  while ((*s = getc()) != '\r') //looking for return character
  {
    putc(*s++);
  }
  *s = 0; //makes a null character at the end
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd(blk / 18, ((2 * blk) % 36) / 18, ((2 * blk) % 36) % 18, buf);
}
