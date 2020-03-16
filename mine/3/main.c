#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

void Term();
void Expression();
void Add();
void Subtract();
void Factor();
void Ident();
void Assignment();

void Multiply(){
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    /* push off the stack */
    EmitLn("addl $4, %esp");
}

void Divide(){
    Match('/');
    Factor();

    /* for an expresion like a/b we have eax=b and %(esp)=a
     * but we need eax=a, b on the stack
     */
    EmitLn("movl (%esp), %edx");
    EmitLn("addl $4, %esp");

    EmitLn("pushl %eax");

    EmitLn("movl %edx, %eax");

    /* sign extension */
    EmitLn("sarl $31, %edx");
    EmitLn("idivl (%esp)");
    EmitLn("addl $4, %esp");
}

void Ident(){
    char *name = GetName();
    if(Look == '('){
        Match('(');
        Match(')');
        sprintf(tmp, "call %s", name);
        EmitLn(tmp);
    }else{
        sprintf(tmp, "movl %s, %%eax", name);
        EmitLn(tmp);
    }
}

void Factor(){
    if(Look == '('){
        Match('(');
        Expression();
        Match(')');
    }else if(IsAddop(Look)){
        Match('-');
        sprintf(tmp, "movl $%s, %%eax", GetNum());
        EmitLn(tmp);
        EmitLn("negl %eax");
    }else if(IsAlpha(Look)){
        Ident();
    }else{
        sprintf(tmp, "movl $%s, %%eax", GetNum());
        EmitLn(tmp);
    }
}

void Term(){
    Factor();
    while(strchr("*/", Look)){
        EmitLn("pushl %eax");
        switch(Look){
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            default:
                Expected("Mulop");
        }
    }
}

void Expression(){
    if(IsAddop(Look)){
        EmitLn("xor %eax, %eax");
    }else{
        Term();
    }

    while(strchr("+-", Look)){
        EmitLn("pushl %eax");

        switch(Look){
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            default:
                Expected("Addop");
        }
    }
}

void Add(){
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Subtract(){
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}

void Assignment(){
    char *name = GetName();
    Match('=');
    Expression();
    sprintf(tmp, "lea %s, %%ebx", name);
    EmitLn(tmp);
    EmitLn("movl %eax, (%ebx)");
}

int main(){
    Init();
    EmitLn(".text");
    EmitLn(".global_start");
    EmitLn("_start:");
    /* Expression(); */
    Assignment();
    
    if(Look != '\n'){
        Expected("Newline");
    }

    /* return the result */
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
    return 0;
}
