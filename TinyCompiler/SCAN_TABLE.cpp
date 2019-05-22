#include "globals.h"
#include "util.h"
#include "scan_table.h"
#include "init_table.h"
/* states in scanner DFA */
//typedef enum
//   { START,INASSIGN,INSLASH,INCOMMENT_START,INCOMMENT_END,INNUM,
//   INGT,INLT,INGE,INLE,INNOTEQ,INID,DONE }
//   StateType;

/* lexeme of identifier or reserved word */
char tokenString_t[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"else",ELSE},{"while",WHILE},
      {"return",RETURN},{"void",VOID},{"int",INT}};

static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

TokenType getToken_t(map<State_Input,TableVal> &M){  

    int tokenStringIndex = 0;
   /* holds current token to be returned */
    TokenType currentToken;
   /* current state - always begins at START */
    StateType state = START;
    TableVal tbVal;
   /* flag to indicate save to tokenString */
    int save;
    while (state != DONE){ 
   		 int c = getNextChar();
   		 int c_ascii = c;
   	 	 //printf("%c",c);
     	 if(isalpha(c)){
     	 	c_ascii='a';
		 }
		 else if(isdigit(c)){
		 	c_ascii='0';
		 }
		 tbVal = M[{state,c_ascii}];
		 state=tbVal.st;
		 if(state == INCOMMENT_START){
		 	tokenStringIndex=0;
		 }
		 
		 currentToken = tbVal.tk;
		 if ((tbVal.save) && (tokenStringIndex <= MAXTOKENLEN))
	       tokenString_t[tokenStringIndex++] = (char) c;
	     if(tbVal.unget){
	     	ungetNextChar();
		 }
	     
	     if (state == DONE)
	     { tokenString_t[tokenStringIndex] = '\0';
	       if (currentToken == ID)
	         currentToken = reservedLookup(tokenString_t);
	     }
	     
	     
	}
	
	if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     //test(currentToken);
     printToken(currentToken,tokenString_t);
     //test(currentToken);
   }
	
	return currentToken;
}

