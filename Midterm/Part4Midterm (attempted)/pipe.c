#define NPIPE 10
#define PSIZE  8 //8 chars
#define putchar kputc
typedef struct pipe{
  char buf[8]; //writer cannot write more than 8 chars
  int head, tail;
  int data, room;
  int writerStatus, readerStatus;
}PIPE;

PIPE pipe;

int show_pipe()
{
  PIPE *p = &pipe;
  int i;
  printf("----------------------------------------\n");
  printf("room=%d data=%d buf=", p->room, p->data);
  for (i=0; i<p->data; i++)
    putchar(p->buf[p->tail+i]);
  printf("\n");
  printf("----------------------------------------\n");
}
int kpipe()
{
  int i;
  PIPE *p = &pipe;
  p->head = p->tail = 0;
  p->data = 0; p->room = PSIZE;
}

int read_pipe(PIPE *p, char *buf, int n)
{
  int ret;
  char c;
  
  if (n<=0)
    return 0;
  show_pipe();

  while(n){ //while n not 0
    printf("reader %d reading pipe\n", running->pid);
    ret = 0;
    while(p->data){ //while data not 0
        *buf = p->buf[p->tail++];
        p->readerStatus=1; //means we are reading
        p->tail %= PSIZE;
        buf++;  ret++; 
        p->data--; p->room++; n--;
        if (n <= 0)
            break;
    }
    show_pipe();
    if (ret){   /* has read something */
    //wakeuop writer side, return what you read
       kwakeup(&p->room);
       return ret;
    }
   
    if((p->writerStatus)==0)
    {
      printf("ERROR: Broken Pipe\n"); // if our writer is 0, return out its a broken pipe ->no data
      kexit(-9);
      return 0;
    }

    printf("reader %d sleep for data\n", running->pid);
    kwakeup(&p->room); //wakeup writer side
    ksleep(&p->data);
    continue;
  }
}

int write_pipe(PIPE *p, char *buf, int n) //pipe ptr, buf, how many bytes
{
  char c;
  int ret = 0; 
  show_pipe();
  while (n){
    printf("writer %d writing pipe\n", running->pid);
    while (p->room){ //room has only 8, at most write 8 chars, then wait for someone to wake up
       p->buf[p->head++] = *buf; 
       p->writerStatus=1;
       p->head  %= PSIZE;
       buf++;  ret++; 
       p->data++; p->room--; n--;
       if (n<=0){
         show_pipe();
	 kwakeup(&p->data);
	 return ret;
       }
    }
    show_pipe();
    printf("writer %d sleep for room\n", running->pid);
    kwakeup(&p->data);
    ksleep(&p->room);
  }
  /* 
   n == 0, or reader == 0 (dead reader) kexit and says writer = 0
   */

  if (n==0)
  {
    p->writerStatus=0;
  
    kexit(0);
    
  }
  return 0;
}

int pipe_reader()
{
  char line[128];
  int nbytes, n;
  PIPE *p = &pipe;
  printf("proc %d as pipe reader\n", running->pid);
  if(p->writerStatus ==0 && !p->data)
  {
    printf("Shits broke yo");
    kexit(-1);
  }

  while(1){
    printf("input nbytes to read : " );
    // scanf("%d", &nbytes); getchar();
    nbytes = geti();

    /* 
      if trying to read 0 bytes, exit the process
      set your reader flags
     */

    if(nbytes==0)
    {
      printf("Trying to read 0 bytes\n");
      kexit(0);
      p->writerStatus=0;
    }
    n = read_pipe(p, line, nbytes);
    line[n] = 0;
    printf("Read n=%d bytes : line=%s\n", n, line);
  }
}


int pipe_writer()
{
  char line[128];
  int nbytes, n;
  PIPE *p = &pipe;
  printf("proc %d as pipe writer\n", running->pid);

  while(1){
    printf("input a string to write : " );

    // fgets(line, 128, stdin);
    kgets(line);
    // line[strlen(line)-1] = 0;

    if (strcmp(line, 0)==0)
    {
       return 0;
    }


    nbytes = strlen(line);
    printf("nbytes=%d buf=%s\n", nbytes, line);
    n = write_pipe(p, line, nbytes);
    printf("wrote n=%d bytes\n", n);
  }
}


