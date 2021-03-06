#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
//typedef enum
//   { START,INASSIGN,INSLASH,INCOMMENT_START,INCOMMENT_END,INNUM,
//   INGT,INLT,INGE,INLE,INNOTEQ,INID,DONE }
//   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];


#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */


static int getNextChar()
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


static void ungetNextChar()
{ if (!EOF_flag) linepos-- ;}


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


TokenType getToken(void)
{  
   int tokenStringIndex = 0;
   
   TokenType currentToken;
   StateType state = START;
   int save;
   while (state != DONE)
   { int c = getNextChar();
   	 //printf("%c",c);
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == '=')
           state = INASSIGN;
         else if (c == '/')
		   state = INSLASH; 
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE;
         else if (c == '>'){
         	state = INGT;
		 }
         else if (c == '<'){
          	state = INLT;
		 }
		 else if (c == '!'){
		 	state = INNOTEQ;
		 }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               currentToken = ENDFILE;
               break;
             case '+':
               currentToken = PLUS;
               break;
             case '-':
               currentToken = MINUS;
               break;
             case '*':
               currentToken = TIMES;
               break;
             case '(':
               currentToken = LPAREN;
               break;
             case ')':
               currentToken = RPAREN;
               break;
             case '{':
               currentToken = LCURL;
               break;
             case '}':
               currentToken = RCURL;
               break;
             case '[':
               currentToken = LSQ;
               break;
             case ']':
               currentToken = RSQ;
               break;
             case ',':
             	currentToken = COMMA;
             	break;
             case ';':
               currentToken = SEMI;
               break;
             default:
               currentToken = ERROR;
               break;
           }
         }
         break;
       case INSLASH:
       		if(c == '*'){
       			state = INCOMMENT_START;
       			save = FALSE;
			}
			else{
				state = DONE;
				currentToken = OVER;
	   			ungetNextChar();
			}
			break;
	   case INCOMMENT_START:
	   		save = FALSE;
	   		tokenStringIndex=0;
	   		if(c=='*'){
	   			state=INCOMMENT_END;
			}
	   		break;
	   case INCOMMENT_END:
	   		save = FALSE;
	   		if(c=='*'){
	   			state=INCOMMENT_END;
			}
			else if(c=='/'){
				state = START;
			}
			else{
				state = INCOMMENT_START;
			}
			break;
	   case INGT:
	   		if(c=='='){
	   			state = INGE;
			}
			else{
				currentToken = GT;
				save=FALSE;
				ungetNextChar();
				state = DONE;
			}  
			break;
		case INGE:
			currentToken = GE;
			save=FALSE;
			ungetNextChar();
			state = DONE;
			break;
		case INLT:
	   		if(c=='='){
	   			state = INLE;
			}
			else{
				currentToken = LT;
				save=FALSE;
				ungetNextChar();
				state = DONE;
			}  
			break;
		case INLE:
			currentToken = LE;
			save=FALSE;
			ungetNextChar();
			state = DONE;
			break;
		case INNOTEQ:
			state = DONE;
			if(c == '='){
				currentToken = NOTEQ;
			}
			else{
				currentToken = ERROR;
			}
			break;
       case INASSIGN:
         state = DONE;
         if (c == '=')
           currentToken = EQ;   // ==
         else
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           currentToken = ASSIGN;
         }
         break;
       case INNUM:
         if (!isdigit(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
         }
         break;
       case INID:
         if (!isalpha(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     TokenType tt =  currentToken;
     //test(currentToken);
     printToken(currentToken,tokenString);
     //test(currentToken);
   }
   return currentToken;
} 


