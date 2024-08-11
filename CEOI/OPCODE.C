/*OPCODE.C ver 1.0 compilable using TURBO C ver 2.0 or later */
/* must use LARGE or HUGE Model */
#include <alloc.h>
#include <stdio.h>
#include <CONIO.H>
#include <STDLIB.H>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <IO.H>
/* prototypes *************************/
typedef char str20[21];
typedef char str24[35]; /* change as required to max text length+5*/
typedef char string[180];/* large enough to cover max page width*/
typedef char triad[3];
/* prototypes **************************/
void getint(int *in);
void getlong(long int *in);
void makealpha(string out);
void buildsuperstr(void);
void grabthree(str24 s,long int pos);
void ptrsort(char **v[], long int left, long int right,int compf);
void outputer(void);
/* global variables and constants  *************/
str20 in;
str24 *table;
string *pageptr;
str24** wordidx;
str24** codeidx;
str24** idx;
triad *superptr;
long int base = 26;/* number of letters in alphabet*/
int code=0;
int word=1;
int numerical=2;
long int n=1;
string title="Operations Codes  ";
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
long maxitems = 1500; /* must be < maxp */
long int p=0;
long newend=0;
long sn=0;
unsigned long maxp = 17579; /*17579*/
char ach ='A';
char zch ='Y'; /*change to Y or Z when debugged*/
string *pageptr;
string infile ="table.txt";
FILE *infcb;   /* handle for input text file*/
string outfile ="outfile.txt";
FILE *outfcb;   /* handle for output text file */
string serialfile ="serial.num";
FILE *serial;
/******************************************/
void stripnl(char*s,int l) /* strip \n char and truncate string to avoid*/
{  int i;            /* over writing next table entry*/
	if(strlen(s)>l) printf("\nTruncating :%s",s);
	for(i=0;s[i] !='\n' && i<l;i++){}
	s[i] =0;
}
void lpad(string s,char fill,int length)
{
	while(strlen(s)<length){
	memmove(s+1,s,sizeof(string)-1);
	s[0] = fill;
	}
}
string* rpad(char *s,int length)
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
void getint(int *in)/* get integer - avoid garbage from scanf() */
{
	string s;
	memset(s,0,sizeof(string));
	gets(s);
	if(strlen(s) >0){
	sscanf(s,"%d",in);}
	if(*in<0) exit(1);
}
void getlong(long *in)/* get long integer - avoid garbage from scanf() */
{
	string s;
	memset(s,0,sizeof(string));
	gets(s);
	if(strlen(s) >0){
		sscanf(s,"%ld",in);
		if(*in<0) exit(0);  /*just a way out*/
	}
}
int codecompare(char a[],char b[],int mode)/*compare function for qsort*/
{  int ret,offset;
	char first[80],second[80];
	offset = sizeof(triad)+1;
	if(mode == code){
	strncpy(first,a,sizeof(triad));
	strncpy(second,b,sizeof(triad));
	} else if (mode == word){
	memmove(first,a+offset,70);    /* fix number to move */
	memmove(second,b+offset,70);
	} else exit(1);
	ret = stricmp(first,second);
	return(ret);
}
/* modified qsort*/
void ptrsort(char **v[], long int left, long int right,int compf)
{
	long int i, last;
	void swap(char **v[], long int i, long int j);
	if (left >= right) /*do nothing if array contains less than 2 elememts*/
		return;
	swap(v, left, (left + right)/2);
	last = left;
	for (i = left+1; i <= right; i++)
		if (codecompare(v[i], v[left],compf) < 0)
			swap(v, ++last, i);
	swap(v, left, last);
	ptrsort(v, left, last - 1,compf);
	ptrsort(v, last + 1, right,compf);
}
void swap(char **v[], long int i, long int j)
{
	char **temp;
	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

void initialize(void)
{  unsigned long l;
	char **endptr;
	clrscr();
	srand((unsigned)time(NULL));
	pageptr = (string*)malloc(pagelength*sizeof(string));
		if(pageptr != NULL){	memset(pageptr,0,pagelength*sizeof(string));}
		else {printf("\nError allocating page");exit(1);}
	table = (str24 *)malloc(maxitems *sizeof(str24));
		if(table != NULL){memset(table,0,(maxitems * sizeof(str24)));}
		else {printf("\nError allocating table"); exit(1);}
	wordidx =(str24 **)malloc(maxitems * sizeof(str24 *));
	codeidx =(str24 **)malloc(maxitems * sizeof(str24 *));
	superptr = (triad*)farmalloc((unsigned long)(maxp+2)*sizeof(triad));
	if(superptr == NULL) {
		printf("\nFailed malloc of superptr\n");
		exit(1);
		}
	memset(superptr,0,(maxp+1)*sizeof(triad));

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
			if ((serial = fopen(serialfile,  "r+")) == NULL)
		{
		  perror("Error: Unable to open serial.num");
		  exit (1);
		}
	 fgets(snstr,80,serial);
	 sn=strtol(snstr,endptr,10);

}
int getdata(void)
{
	int ret = 0;
	string s;
	char **endptr;
	clrscr();
	printf("\nPAGE LAYOUT   JUST PRESS <ENTER> TO LEAVE UNCHANGED\n");
	printf("Enter a negative number at any numerical prompt to exit\n");
	printf("Title of Table: %s  ",title);
	memset(s,0,sizeof(string));
	gets(s);
	if(strlen(s)>0) strcpy(title,s);
	printf("\nMaximum number of columns on page: %d ",maxcolumn);
	getint(&maxcolumn);
	printf("\nMaximum length of data+numbers(max 34): %d",datalength);
	getint(&datalength);
	if(datalength>sizeof(str24)){
		printf("\nChange definition of str24 in source and recompile");
		exit(1);
	}
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
		if(pagewidth>175)	{ /*if 175 is not enough change def of string*/
				printf("\nI Told you 175 was max. I Quit!\n");
				exit(1);
		}
	printf("\nLeft margin : %d",leftmargin);
	getint(&leftmargin);
	printf("\nCurrent serial number is : %ld\n",sn);
	getlong(&sn);
	return(ret);
}


void buildsuperstr(void)
{  long len;
	char i,j,k;
	str24 temp;
	p=0;
		for (i=ach;i<=zch;i++){
			for(j=ach;j<=zch;j++){
				for(k=ach;k<=zch;k++){
					memset(temp,0,sizeof(str24));
					temp[0]=i;
					temp[1]=j;
					temp[2]=k;
					memmove(superptr[p++],temp,sizeof(triad));
				}
			}
		}
	len =strlen((char*)superptr);
	newend = len/sizeof(triad);
}
void addcode(void)   /*fill table with code triads*/
{
	long len;
	long i=0;
	long pos;
	str24 s;
	triad t;
	len =strlen((char*)superptr);
	newend = len/sizeof(triad);
	for(i=0;i<maxitems;i++){
		memset(s,0,sizeof(s));
		pos = rand() % newend;
		grabthree(s,pos);
		strcpy(table[i],s);
		strcat(table[i]," "); /*add space between code and text*/
	}
}
void addtext(void)  /*fill table with text words from "table.txt"*/
{  int j;
	string s;
	maxitems = 0;
	j=datalength-sizeof(triad)-1;
	fseek(infcb, 0, SEEK_SET);
	while(!feof(infcb)){
		memset(s,0,sizeof(s));
		fgets(s,sizeof(s),infcb);
		stripnl(s,j);
		strcat(table[maxitems],s);
		wordidx[maxitems] = &table[maxitems];
		codeidx[maxitems] = &table[maxitems];
		maxitems++;
	}
	maxitems--;/*set to last item*/
	maxitems--;
}
void buildtable(void)
{
	addcode();
	addtext();
}

void sortindex(void)
{
	ptrsort((char**)codeidx,0,maxitems, code);
	ptrsort((char**)wordidx,0,maxitems, word);
}

void printpage(void)
{ 	int i,j;
		for(j=0;j<copies;j++){
			for(i=0;i<pagelength;i++)
			{
				fprintf(outfcb,"%s",pageptr+i);
			}
/*			fprintf(outfcb,"\f"); */ /*remove comments to add page feed*/
		}
}
void makepage(int type)
{
	string buffer;
	char pgno[10];
	int line=0;
	int column=1;
	string pad,snum;
	sprintf(snum,"%6ld",sn);
	stripnl(snum,10);
	sprintf(pgno,"%3d",pagenumber++);

	memset(pad,0,sizeof(string));
	rpad (pad,leftmargin);
	memset(pageptr,0,pagelength*sizeof(string));
	for(line = 0; line<pagelength;line++)
	{
		strcpy(pageptr[line],pad); /*put in left margin*/
	}
	strcat(pageptr[2],title);
	if(type==code){
		 strcat(pageptr[2]," CODE TO TEXT ");
		 idx=codeidx;
	}
	if(type==word){
		 strcat(pageptr[2]," TEXT TO CODE ");
		 idx=wordidx;
	}
	if(type==numerical) strcat(pageptr[2]," NUMERICAL ");
	strcat(pageptr[2]," Serial No.");
	strcat(pageptr[2],snum);
	strcat(pageptr[2],"  Page ");
	strcat(pageptr[2],pgno);
	for(column = 0;column<maxcolumn && p<=maxitems;column++){
		for(line=tableoffset;line<tablebottom && p<=maxitems;line++){
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,*idx[p++]);
			strcat(pageptr[line],buffer);
			rpad(pageptr[line],(leftmargin + (column+1)*(datalength + columnspace)));
		} /*line*/
	}  /*column*/

	for(line=0;line<pagelength;line++){
		strcat(pageptr[line],"\n");
	} /*put line feeds at the end of each line*/
}

void grabthree(str24 s,long pos)
{

	memmove(s,superptr[pos],sizeof(triad));
	memmove(superptr[pos],superptr[pos+1],(maxp-(pos+1)) * sizeof(triad));
	memset(superptr[newend--],0,1);
}
void incsn(void)
{
	sn++;
	ltoa(sn,snstr,10);
	fseek(serial,0,0);
	fputs(snstr,serial);
}
void closeall(void)
{
	farfree(superptr);
	farfree(codeidx);
	farfree(wordidx);
	farfree(table);
	farfree(pageptr);
	incsn();
	fclose(infcb);
	fclose(outfcb);
	fclose(serial);
}
void outputer(void)
{  int type;
	for(type=1;type>=0;type--){
		p=0;
		while(p<maxitems){
			makepage(type);
			printpage();
		}
	}
}
void main(void)
{
		clrscr();
		printf("\nInitializing ...");
	initialize();
	getdata();
		printf("\nBuilding super string ...");
	buildsuperstr();
		printf("\nBuilding table ...");
	buildtable();
		printf("\nSorting index ...");
	sortindex();
		printf("\nPrinting outfile.txt ...");
	outputer();
	printf("\n\nUse your text editor to edit and print 'outfile.txt'\n");
	closeall();
}