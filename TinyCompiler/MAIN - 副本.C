#include "globals.h"

///* set NO_PARSE to TRUE to get a scanner-only compiler */
//#define NO_PARSE TRUE
///* set NO_ANALYZE to TRUE to get a parser-only compiler */
//#define NO_ANALYZE FALSE
//
///* set NO_CODE to TRUE to get a compiler that does not
// * generate code
// */
//#define NO_CODE FALSE
//
// 
//#include "util.h"
//#if NO_PARSE
#include "scan.h"
#include "scan_table.h"
#include "init_table.h"
//#else
//#include "parse.h"
//#if !NO_ANALYZE
//#include "analyze.h"
//#if !NO_CODE
//#include "cgen.h"
//#endif
//#endif
//#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;


bool operator< ( State_Input a, State_Input b ){
	return std::make_pair(a.st,a.c) < std::make_pair(b.st,b.c) ;
}
map<State_Input,TableVal> M;
void initTable(){
	M[{START,' '}]={START,NULL_TK,0,0}; M[{START,'\n'}]={START,NULL_TK,0,0};
	M[{START,'\t'}]={START,NULL_TK,0,0}; M[{START,-1}]={DONE,ENDFILE,0,0};
	M[{START,'+'}]={DONE,PLUS,1,0};
	M[{START,'-'}]={DONE,MINUS,1,0}; M[{START,'*'}]={DONE,TIMES,1,0};
	M[{START,';'}]={DONE,SEMI,1,0}; M[{START,'('}]={DONE,LPAREN,1,0};
	M[{START,','}]={DONE,COMMA,1,0}; M[{START,')'}]={DONE,RPAREN,1,0};
	M[{START,'{'}]={DONE,LCURL,1,0}; M[{START,'['}]={DONE,LSQ,1,0};
	M[{START,'}'}]={DONE,RCURL,1,0}; M[{START,']'}]={DONE,RSQ,1,0};
	
	M[{START,'a'}]={INID,ID,1,0};
	M[{INID,'a'}]={INID,ID,1,0};
	for(int i=-1;i<128;i++){ //'other' for ID
		if(!isalpha(i)){
			M[{INID,i}]={DONE,ID,0,1};
		}
	}
	
	M[{START,'0'}]={INNUM,NUM,1,0};
	M[{INNUM,'0'}]={INNUM,NUM,1,0};
	for(int i=-1;i<128;i++){ //'other' for NUM
		if(!isdigit(i)){
			M[{INNUM,i}]={DONE,NUM,0,1};
		}
	}
	
	M[{START,'='}]={INASSIGN,NULL_TK,1,0};
	M[{INASSIGN,'='}]={DONE,EQ,1,0};
	for(int i=-1;i<128;i++){ //'other' for '='
		if(i!='='){
			M[{INASSIGN,i}]={DONE,ASSIGN,0,1};	
		}
	}
	
	M[{START,'/'}]={INSLASH,NULL_TK,1,0};
	M[{INSLASH,'*'}]={INCOMMENT_START,NULL_TK,0,0};
	for(int i=-1;i<128;i++){ //'other' for '/'
		if(i!='*'){
			M[{INSLASH,i}]={DONE,OVER,1,1};	
			M[{INCOMMENT_START,i}]={INCOMMENT_START,NULL_TK,0,0};	
		}
	}
	M[{INCOMMENT_START,'*'}]={INCOMMENT_END,NULL_TK,0,0};
	M[{INCOMMENT_END,'/'}]={START,NULL_TK,0,0};
	M[{INCOMMENT_END,'*'}]={INCOMMENT_END,NULL_TK,0,0};
	for(int i=-1;i<128;i++){ //'other' for '/','*'
		if(i!='*' && i!='/'){
			M[{INCOMMENT_END,i}]={INCOMMENT_START,NULL_TK,0,0};	
		}
	}
	
}

main() 
{ 	
  int argc = 2;
  char * argv[2];
  argv[1] = (char *)malloc(20*sizeof(int));
  strcpy(argv[1],"test/input.txt");
  
  TreeNode * syntaxTree;
  char pgm[120]; /* source code file name */
  if (argc != 2)
    { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".tny");
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }


	initTable();
  
  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nTINY COMPILATION: %s\n",pgm);
//#if NO_PARSE
//  while (getToken_t(M)!=ENDFILE);
	while (getToken()!=ENDFILE);
//#else
//  syntaxTree = parse();
//  if (TraceParse) {
//    fprintf(listing,"\nSyntax tree:\n");
//    printTree(syntaxTree);
//  }
//#if !NO_ANALYZE
//  if (! Error)
//  { if (TraceAnalyze) fprintf(listing,"\nBuilding Symbol Table...\n");
//    buildSymtab(syntaxTree);
//    if (TraceAnalyze) fprintf(listing,"\nChecking Types...\n");
//    typeCheck(syntaxTree);
//    if (TraceAnalyze) fprintf(listing,"\nType Checking Finished\n");
//  }
//#if !NO_CODE
//  if (! Error)
//  { char * codefile;
//    int fnlen = strcspn(pgm,".");
//    codefile = (char *) calloc(fnlen+4, sizeof(char));
//    strncpy(codefile,pgm,fnlen);
//    strcat(codefile,".tm");
//    code = fopen(codefile,"w");
//    if (code == NULL)
//    { printf("Unable to open %s\n",codefile);
//      exit(1);
//    }
//    codeGen(syntaxTree,codefile);
//    fclose(code);
//  }
//#endif
//#endif
//#endif
  fclose(source);
  return 0;
}

