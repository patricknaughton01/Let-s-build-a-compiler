#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif

void Other();
void BLock(char *L);
void Condition();
void DoProgram();
void DoIf(char *L);
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
void Expression();
void DoDo();
void DoBreak(char *L);

int main(){
    Init();
    DoProgram();
    return 0;
}

void Other(){
    sprintf(tmp, "%c", GetName());
    EmitLn(tmp);
}

void Block(char *L){
    // Tokens that can end a block
    while(!strchr("elu", Look)){
        dprint("Block: get Look = %c\n", Look);
        switch(Look){
            case 'i':
                DoIf(L);
                break;
            case 'w':
                DoWhile();
                break;
            case 'p':
                DoLoop();
                break;
            /*case 'r':
                DoRepeat();
                break;*/
            case 'f':
                DoFor();
                break;
            /*case 'd':
                DoDo();
                break;*/
            case 'b':
                DoBreak(L);
                break;
                //TODO: Why no break here?
            default:
                Other();
                break;
        }
        // Eat a newline
        Newline();
    }
}

void Condition(){
    // Dummy text for now
    EmitLn("<condition>");
}

void DoProgram(){
    Block(NULL);
    if(Look != 'e'){
        Expected("End");
    }
    EmitLn("END");
}

void DoIf(char *L){
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, L1);
    
    Match('i');
    Condition();
    
    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);
    
    Block(L);
    dprint("DoIf: Got Look = %c\n", Look);
    
    if(Look == 'l'){
        // Match `else` statement
        Match('l');
        strcpy(L2, NewLabel());
        
        sprintf(tmp, "jmp %s", L2);
        EmitLn(tmp);
        PostLabel(L1);
        Block(L);
    }
    
    Match('e');
    PostLabel(L2);
}

void DoWhile(){
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    
    Match('w');
    PostLabel(L1);
    Condition();
    sprintf(tmp, "jz %s", L2);
    EmitLn(tmp);
    Block(L2);
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoLoop(){
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    
    Match('p');
    PostLabel(L1);
    Block(L2);
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoFor(){
    // I translate this like a C for loop
    // FOR <init> <cond> <exec> <blk> ENDFOR
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    
    Match('f');
    EmitLn("<init>");
    PostLabel(L1);
    Condition();
    sprintf(tmp, "jz %s", L2);
    EmitLn(tmp);
    Block(L2);
    EmitLn("<exec>");
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoBreak(char *L){
    Match('b');
    if(L != NULL){
        sprintf(tmp, "jmp %s", L);
        EmitLn(tmp);
    }else{
        Abort("No loop to break from");
    }
}

