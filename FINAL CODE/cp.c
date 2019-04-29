#include "ucode.c"

#define MAX_FILENAME_LEN 256
#define MAX_INPUT_LEN 32
#define MAX_FILELENGTH
int n, count;
int fd, gd, dev;
int nblocks, ninodes, bmap, imap, inode_start;

char buf[MAX_FILELENGTH];
//page 303 in embedded systems and 261 in systems programming
int main(int argc, char *argv[])
{
	// 1. argv[0] is src; argv[1] is dst (where we copying to!)
	if (argc < 3)
	{
		printf("my_cp: ERROR -- need two files to proceed\n");
		exit(1); //
	}
	// char src[MAX_FILENAME_LEN], dst[MAX_FILENAME_LEN];
	// strcpy(src, argv[0]); strcpy(dst, argv[1]);
	// return sw_kl_cp(src, dst);
	fd = open(argv[1], O_RDONLY);
	gd = open(argv[2], O_WRONLY | O_CREAT);
	if (fd < 0 || gd < 0)
	{
		printf("ERROR: Unable to open source or dest file\n");
		exit(1);
	}
	while (n = read(fd, buf, 1024))
	{
		
		write(gd, buf, n);
		count += n;
	}
	printf("Total number of bytes that have been copied: %d\n", count);
	exit(0); //make sure to close
}