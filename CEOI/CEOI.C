/* ceoi.c ver 1.0 compilable using TURBO C 2.0 or later - use TINY Model */
#include <stdio.h>
#include <CONIO.H>
#include <STDLIB.H>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <IO.H>
#define linemax 26 /* change to 25 to eliminate "Z" (standard U.S.Army)
/*remember to change tab locations and number locations*/
#define pagelength 63
#define pagewidth 80
#define tableoffset 30   /* line no. at start of authentication table*/
/*#define output outfileptr*/
#define output stdprn /* DOS or Windows only not ANSI */
/* prototypes *************************/
typedef char string[81];
void initialize(void);
void makeinstring(void);
int getdata(void);
void shuffle(string  ostr,int line);
void deal(string ostr,int ln);
char grabout(string s,int pos);
char *ltoa(long value, char *string, int radix);/*stdlib.h*/
/* global variables *******************/
string title;
string astring;
string instr,snstr;
long sn;
int pagenumber;
int numberofpages;
int copies;
int disk=0;
string *pageptr;
string vertfile ="vert.txt";
FILE *v;   /* handle for vertical text file*/
string horizfile ="horiz.txt";
FILE *h;   /* handle for horizontal text file */
string serialfile ="serial.num";
FILE *serial;
string outfile ="outfile.txt";
FILE *outfileptr;  /*remove comments to print to disk*/
/*************************************/
void initialize(void)
{
	char **endptr;
	clrscr();
	srand((unsigned)time(NULL) );
	makeinstring();
	pageptr = (string*)malloc(pagelength*sizeof(string));
	memset(pageptr,0,pagelength*sizeof(string));
	if ((v = fopen(vertfile,  "r")) == NULL)
		{
		  perror("Error: Unable to open vert.txt");
		  exit (1);
		}
	if ((h = fopen(horizfile,  "r")) == NULL)
		{
		  perror("Error: Unable to open horiz.txt");
		  exit (1);
		}
	if ((serial = fopen(serialfile,  "r+")) == NULL)
		{
		  perror("Error: Unable to open serial.num");
		  exit (1);
		}
	 fgets(snstr,80,serial);
	 sn=strtol(snstr,endptr,10);
	 if ((outfileptr = fopen(outfile,"w")) == NULL)
		{	perror("Error: Unable to open outfile.txt");
			exit(1);} /*remove comments to print to disk*/
}
string* rpad(string s,int length)
{
	char* blank = " ";
	if(s[0]=='\x0')
	{s[0]=' ';
	s[1]='\x0';}
	while( strlen(s)<length) {
	s= strcat((char*)s,blank);
	}
	return((string*)s);
}
int getdata(void)
{
	int ret = 0;
	char ch=' ';
	string s;
	char **endptr;
	clrscr();
	printf("Enter title of Page : ");
	gets(title);
	strcat(title,"  SERIAL NO. ");
	printf("Current serial number is : %ld ",sn);
	printf("\nEnter new serial number (or just enter to leave as is)");
	gets(s);
	if( strlen(s) >0){
		 strcpy(snstr,s);
		 sn=strtol(snstr,endptr,10);
	}
	printf("Enter number of different pages to produce  :");
	scanf("%d",&numberofpages);
	memset(s,0,sizeof(s));
	while(ch != 'P' && ch != 'p' && ch != 'D' && ch != 'd')
	{
		printf("Do you want to print to Printer or Disk  P or D :");
	/*	gets(s);
		ch=s[0];*/
		ch=getch();
	};
	if(ch == 'P'|| ch == 'p'){
		printf("\nEnter number of copies of each page to print :");
		scanf("%d",&copies);
	} else disk = 1;
	return(ret);
}
void makeinstring(void)/*initialize string with upper case alphabet*/
{
	int c;
	char ch='A';
	for (c=0;c<linemax;c++){
		instr[c]=ch++;
	}
	instr[linemax]='\0';
}
void shuffle(string optr, int line)
{  string i;
	int c,pos;
	strcpy(i,instr);
	optr[linemax]='\0';
	optr[0] =(char)grabout(i,line);/*start line with set letter*/
	for (c=1;c<linemax;c++){
	pos = rand() % (linemax - c);
	optr[c]=(char)grabout(i,pos);
	}

}
char grabout(string s,int pos)
{ char ret;
	int len;
	len =strlen(s);
	ret= s[pos];
	memmove(&s[pos],&s[pos+1],sizeof(string)-(pos+1));
	s[len]='\0';
	return(ret);
}
void pokein(string s,char ch,int pos)
{  int leng;
	leng = strlen(s);
	if(pos<leng){
		memmove(&s[pos+1],&s[pos],leng-pos);
		s[pos]=ch;
		s[leng+1]='\0';
	}
}
void spread(string os)
{  int tab[11]={1,6,10,14,17,20,24,27,30,33,-1};
	int i;
	for(i=0;tab[i]>0;i++)pokein(os,' ',tab[i]);
}
void deal(string  os,int line)
{
	spread(os);
	strcpy(pageptr[line+tableoffset+4],os);
}
void incsn(void)
{
	sn++;
	ltoa(sn,snstr,10);
	fseek(serial,0,0);
	fputs(snstr,serial);

}
void printpage(void)
{ 	int i,j;
		for(j=0;j<copies;j++){
			for(i=0;i<pagelength;i++)
			{
				fprintf(output,"%s",pageptr[i]);
			}
			fprintf(output,"\f");
		}
}
void printdisk(void)
{
  int i;
		for(i=0;i<pagelength;i++)
			{
				fprintf(outfileptr,"%s",pageptr[i]);
			}
}
void makepage(void)
{
	string buffer;
	int line=0;
	string rg;
	memset(pageptr,0,pagelength*sizeof(string));
	strcpy(rg,instr);
	spread(rg);
	strcpy(pageptr[60],"^Set Letter");
	strcpy(pageptr[61],rg);
	strcpy(pageptr[62],"      Readers guide");
	strcpy(pageptr[tableoffset],"     Authentication/Cipher Table");
	strcpy(pageptr[tableoffset+1],title);
	strcat(pageptr[tableoffset+1],snstr);
	strcpy(pageptr[tableoffset+2],"    0   1   2  3  4   5  6  7  8  9");
	for (line=0;line<linemax;line++){
		shuffle(astring,line);
		deal(astring,line);
	}
	for (line=tableoffset;line< pagelength;line++)
	{ rpad(pageptr[line],40);}
	rewind(v);
	for(line=tableoffset;(!feof(v) && line<pagelength);line++)
	{  fgets(buffer,80,v);
		if(strlen(buffer)>39)
		 {buffer[39]='\n';
		  buffer[40]='\x0';}
		strcat(pageptr[line],buffer);
	}
	if(line<pagelength)
	{ while(line<pagelength)
		{strcat(pageptr[line++],"\n");
		}
	}
	rewind(h);
	for(line=0;(line<(tableoffset-1) && !feof(h));line++)
	{ fgets(pageptr[line],80,h);}
}
void closeall(void)
{
	fclose(h);fclose(v);fclose(serial);fclose(outfileptr);
}
void goodby(void)
{ printf("\n\nUse your word processor to edit and print %s", outfile);}
int main(void)
{  int k;
	initialize();
	getdata();
	for(k=0;k<numberofpages;k++){
		makepage();
		if(disk==0) printpage();
		else printdisk();
		incsn();
	}
	closeall();
	if(disk!=0) goodby();
	return(0);
}