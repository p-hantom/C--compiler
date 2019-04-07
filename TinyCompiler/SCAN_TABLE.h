#ifndef _SCAN_TABLE_H_
#define _SCAN_TABLE_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString array stores the lexeme of each token */
extern char tokenString_t[MAXTOKENLEN+1];

/* function getToken returns the 
 * next token in source file
 */
TokenType getToken_t(map<State_Input,TableVal> &M);

#endif
