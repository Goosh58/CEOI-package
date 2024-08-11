#include <stdio.h>
#include <CONIO.H>
#include <STDLIB.H>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <IO.H>
#define output stdout /* DOS or Windows only not ANSI */

/* prototypes *************************/
typedef char string[180];
void initialize(void);
void makedata(string s);
int getdata(void);
char grabout(string s,int pos);
void getint(int *in);
/* global variables *******************/
string title="Operations Codes        ";
string astring;
string instr,snstr;
int item = 0;
int pagenumber = 1;
int maxcolumn = 3;
int datalength = 24; /*length of data string */
int columnspace = 1; /*blank space between columns*/
int tablebottom = 60; /*adjust for bottom margin */
int copies = 1;
int tableoffset = 5;   /* line no. at start of table*/
int pagelength = 66;
int pagewidth = 80;
int leftmargin = 5;
int rightmargin = 5;
string *pageptr;
string infile ="table.txt";
FILE *infcb;   /* handle for input text file*/
string outfile ="outfile.txt";
FILE *outfcb;   /* handle for output text file */
/*************************************/
void initialize(void)
{
	clrscr();
	pageptr = (string*)malloc(pagelength*sizeof(string));
	memset(pageptr,0,pagelength*sizeof(string));
	if ((infcb = fopen(infile,  "rt")) == NULL)
		{
		  perror("Error: Unable to open table.txt");
		  exit (1);
		}
	if ((outfcb = fopen(outfile,  "w")) == NULL)
		{
		  perror("Error: Unable to open output.txt");
		  exit (1);
		}
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
	string s;
	clrscr();
	printf("\nPAGE LAYOUT   JUST PRESS <ENTER> TO LEAVE UNCHANGED\n\n");
	printf("Title of Table: %s\n",title);
   memset(s,0,sizeof(string));
	gets(s);
   if(strlen(s)>0) strcpy(title,s);

/*	printf("\nNumber of copies of each page to print: %d ",copies);
	getint(&copies); */
	printf("\nMaximum number of columns on page: %d ",maxcolumn);
	getint(&maxcolumn);
	printf("\nMaximum length of data+numbers: %d ",datalength);
	getint(&datalength);
	printf("\nSpace between columns: %d ",columnspace);
	getint(&columnspace);
	printf("\nLine number for beginning of table: %d ",tableoffset);
	getint(&tableoffset);
	printf("\nLine number for end of table: %d ",tablebottom);
	getint(&tablebottom);
	printf("\nPage length: %d ",pagelength);
	getint(&pagelength);
	printf("\nWidth of page(max 175): %d ",pagewidth);
	getint(&pagewidth);
	if(pagewidth>175)	{
			printf("\nI Told you 175 was max. I Quit!\n");
			exit(1);
	}
	printf("\nLeft margin %d :",leftmargin);
	getint(&leftmargin);
	return(ret);
}
void getint(int *in)/* get integer - avoid garbage from scanf() */
{
	string s;
	memset(s,0,sizeof(string));
	gets(s);
	if(strlen(s) >0){
	sscanf(s,"%d",in);}

}
/*char grabout(string s,int pos)
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
} */
void printpage(void)
{ 	int i,j;
		for(j=0;j<copies;j++){
			for(i=0;i<pagelength;i++)
			{
				fprintf(outfcb,"%s",pageptr+i);
			}
/*			fprintf(outfcb,"\f");*/
		}
}
void makepage(void)
{
	string buffer;
	char pgno[10];
	int line=0;
	int column=1;
	string pad;
	sprintf(pgno,"%3d",pagenumber++);
	memset(pad,0,sizeof(string));
	rpad (pad,leftmargin);
	memset(pageptr,0,pagelength*sizeof(string));
	for(line = 0; line<pagelength;line++)
	{
		strcpy(pageptr[line],pad); /*put in left margin*/
	}
	strcat(pageptr[2],title);
	strcat(pageptr[2],"           Page ");
	strcat(pageptr[2],pgno);
	for(column = 0;column<maxcolumn && !feof(infcb);column++){
		for(line=tableoffset;line<tablebottom&& !feof(infcb);line++){
			makedata(buffer);
			strcat(pageptr[line],buffer);
			rpad(pageptr[line],(leftmargin + (column+1)*(datalength + columnspace)));
		} /*line*/
	}  /*column*/

	for(line=0;line<pagelength;line++){
		strcat(pageptr[line],"\n");
	} /*put line feeds at the end of each line*/
}
void makedata(string s)
{  int l = datalength - 4;
	string buffer;
	char itemstring[5];
	memset(&buffer,0,sizeof(string));
	fgets(buffer,sizeof(string),infcb);
	buffer[strlen(buffer)-1] = '\x0'; /* remove \n from line */
	if(strlen(buffer)>(l-1)) {	  buffer[l]='\x0';}
	rpad(buffer,l);
	sprintf(itemstring,"%03d",item++);
	strcpy(s,itemstring);
	strcat(s," ");
	strcat(s,buffer);
}
void closeall(void)
{
	fclose(infcb);fclose(outfcb);
}
int main(void)
{  int k;
	getdata();
	initialize();
	while(!feof(infcb)) {
		makepage();
		printpage();
	}
	closeall();
	clrscr();
	printf("\n\n\n   Use your text editor to edit and print 'outfile.txt'");
	return(0);
}