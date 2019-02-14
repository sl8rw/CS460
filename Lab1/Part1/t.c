/*********  t.c file *********************/
//note if stuck, do control+option+esc on a mac running virtual linux

int prints(char *s)
{
  char c;
  while(*s!='\0')
  {
    c=*s; //assigns a character to c
    putc(c); //call to putc
    *s++; //ptr increment
  }
}

int gets(char *s)
{
  while((*s=getc())!='\r') //looking for return character
  {
    putc(*s);
    *s++; //ptr increment
  }
  *s=0; //makes a null character at the end
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
