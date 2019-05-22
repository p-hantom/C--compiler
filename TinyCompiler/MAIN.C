#include "globals.h"
#include "scan.h"
#include "scan_table.h"
#include "init_table.h"
#include "parser_ll1.h"
#include "util.h"

int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

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

  
  listing = stdout; 
  fprintf(listing,"\nCOMPILATION: %s\n",pgm);
  
  
  //scanning only
//	while (getToken()!=ENDFILE);

	  bool TraceParse=true;
	  syntaxTree = parse_ll1();
//	  if (TraceParse) {
//	    fprintf(listing,"\nSyntax tree:\n");
//	    printTree(syntaxTree);
//	  }

  fclose(source);
  return 0;
}

