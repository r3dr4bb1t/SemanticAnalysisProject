/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4
#define MAX_SCOPE 1000
/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

static Scope scopes[MAX_SCOPE];
static int nScope = 0;
static Scope scopeStack[MAX_SCOPE];
static int nScopeStack = 0;

Scope sc_top(void)
{
	return scopeStack[nScopeStack - 1];
}

void sc_pop(void)
{
	--nScopeStack;
}

void sc_push( Scope scope )
{
	scopeStack[nScopeStack++] = scope;
}

Scope sc_create(char *funcName)
{
	Scope newScope;

	newScope = (Scope)malloc(sizeof(struct ScopeRec));
	newScope -> funcNam = funcName;
	newScope -> nestedLevel = nScopeStack;
	newScope -> parent = sc_top();
	scopes[nScope++] = newScope;

	return newScope;
	
}
BucketList st_bucket(char *name)
{
	int h = hast(name);
	Scope sc = sc_top();
	while(sc)
	{
		BucketList l = sc->hashTable[h];
		while ((l != NULL)&&(strcmp(name, l->name)!= 0))
			l = l->next;
		if (l != NULL)
			return l;
		sc = sc->parent;
	}
	return NULL;
}
/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { char * name;
     LineList lines;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
   } * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc )
{ int h = hash(name);
  Scope top = sc_top();
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l ->treeNode = treeNode;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
   top->hashTable[h] = l; }
  else /* found in table, so just add line number */
  {// { LineList t = l->lines;
  //   while (t->next != NULL) t = t->next;
  //   t->next = (LineList) malloc(sizeof(struct LineListRec));
  //   t->next->lineno = lineno;
  //   t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ 
  BucketList l =  st_bucket(name);
  // while ((l != NULL) && (strcmp(name,l->name) != 0))
  //   l = l->next;
   if (l == NULL) return l->memloc;
   return -1;
  // else return l->memloc;
}
int st_exist_top (char * name)
{ int h = hash(name);
  Scope sc = sc_top();
  while(sc) {
    BucketList l = sc->hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL) return TRUE;
    break;
  }
  return FALSE;
}
int st_add_lineno(char * name, int lineno)
{ BucketList l = st_bucket(name);
  LineList ll = l->lines;
  while (ll->next != NULL) ll = ll->next;
  ll->next = (LineList) malloc(sizeof(struct LineListRec));
  ll->next->lineno = lineno;
  ll->next->next = NULL;
}

void printSymTabRows(BucketList *hashTable, FILE *listing) {
  int j;

  for (j=0;j<SIZE;++j)
  { if (hashTable[j] != NULL)
    { BucketList l = hashTable[j];
      TreeNode *node = l->treeNode;

      while (l != NULL)
      { LineList t = l->lines;
        
        fprintf(listing,"%-14s ",l->name);

        switch (node->nodekind) {
        case DeclK:
          switch (node->kind.decl) {
          case FuncK:
            fprintf(listing, "Function  ");
            break;
          case VarK:
            fprintf(listing, "Variable  ");
            break;
          case ArrVarK:
            fprintf(listing, "Array Var.");
            break;
          default:
            break;
          }
          break;
        case ParamK:
          switch (node->kind.param) {
          case NonArrParamK:
            fprintf(listing, "Variable  ");
            break;
          case ArrParamK:
            fprintf(listing, "Array Var.");
            break;
          default:
            break;
          }
          break;
        default:
          break;
        }

        switch (node->type) {
        case Void:
          fprintf(listing, "Void         ");
          break;
        case Integer:
          fprintf(listing, "Integer      ");
          break;
        case Boolean:
          fprintf(listing, "Boolean      ");
          break;
        default:
          break;
        }

        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
}
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */void printSymTab(FILE * listing)
{ int i;

  for (i = 0; i < nScope; ++i) {
    Scope scope = scopes[i];
    BucketList * hashTable = scope->hashTable;

    if (i == 0) {     // global scope
      fprintf(listing, "<global scope> ");
    } else {
      fprintf(listing, "function name: %s ", scope->funcName);
    }

    fprintf(
        listing,
        "(nested level: %d)\n",
        scope->nestedLevel);

    fprintf(listing,"Symbol Name    Sym.Type  Data Type    Line Numbers\n");
    fprintf(listing,"-------------  --------  -----------  ------------\n");

    printSymTabRows(hashTable, listing);

    fputc('\n', listing);
  }
} /* printSymTab */

