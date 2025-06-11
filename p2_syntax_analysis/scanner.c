/****************************************************/
/* File: lexer.c                                     */
/* The lexer implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in lexer DFA */
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

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
int getNextChar(void)
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
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the lexer  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{  /* Initialization: */
   /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;

   /* The while loop simulates the process of DFA:
      get one lexical unit each time,
      update the currentToken/state/save above
      according to current state and the received lexical unit.
   */
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     {
       case START:
         if (isspace(c))
           save = FALSE;
         else if (c == ':')
           state = INASSIGN;
         else if (c == '{')
         {
           save = FALSE;
           state = INCOMMENT;
         }
         else if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == EOF)
         {
           save = FALSE;
           state = DONE;
           currentToken = ENDFILE;
         }
         else
         {
           state = DONE;
           switch (c)
           {
             case '<': currentToken = LT; break;
             case '=': currentToken = EQ; break;
             case '+': currentToken = PLUS; break;
             case '-': currentToken = MINUS; break;
             case '*': currentToken = TIMES; break;
             case '/': currentToken = OVER; break;
             case '(': currentToken = LPAREN; break;
             case ')': currentToken = RPAREN; break;
             case ';': currentToken = SEMI; break;
             default:  currentToken = ERROR; break;
           }
         }
         break;
       case INASSIGN:
         if (c == '=')
         {
           state = DONE;
           currentToken = ASSIGN;
         }
         else
         {
           /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ERROR;
         }
         break;
       case INCOMMENT:
         if (c == '}')
         {
           save = FALSE;
           state = START;
         }
         else if (c == EOF)
         {
           save = FALSE;
           state = DONE;
           currentToken = ENDFILE;
         }
         else
           save = FALSE;
         break;
       case INNUM:
         if (!isdigit(c))
         {
           /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
         }
         break;
       case INID:
         if (!isalpha(c))
         {
           /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       default:
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     {
       tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceLex) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} /* end getToken */
