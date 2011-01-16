/*this is an experimentation in Continuation passing style
the objective is to build a generic CPS Engine that doesn't
require global variables and can calculate a fibbonaci number 
past what the stack depth would normally allow

The First thing to do is to implement the fibbonacci sequence,
and then find the stack depth, then we write an CPS engine, then
we write the fibbonacci in CPS

*/

#include "stdio.h"
#include "stdlib.h"

/*enum defining exception state, Halt and continue are designed
such that you can while on them*/
typedef enum 
{
    Halt = 0,
    Exception = -1,
    Continue = 1
} Throw;
    

/*represents a continuation
has the exception status, the next pointer
the currval, the currval -1, and the continue to depth
status is the current status for a try...catch...finally
code flow, continuation is the continuation to be used
exception is the exception code,
finally is the finally code
args are the arguments to apply to the continuation

note that continuations take in a void *, this means that
functionas have to manually dereference the void themselves
(not very type safe).

*/
typedef struct _CPSC
{
        Throw Status;
        struct _CPSC (*Continuation)(void *);
        void (*Exn)(Throw);
        void (*Finally)(void);
        void * Args;
} CPSC;

    
typedef struct _FIBBARGS
{
    long long int N;
    long long int N_Min1;
    long long int Depth;
} FibbArgs;

/*normal style fibonacci sequence*/
static long FibbNorm(int const n, int const minn, int const depth);

/*CPS style fibbonacci sequence*/
static CPSC FibbNoStack(void * args);

int main(int argc, char ** argv)
{
    
    
    FibbArgs * currargs = malloc(sizeof(FibbArgs));
    currargs->N = 1;
    currargs->N_Min1 = 0;
    currargs->Depth = 100;
    /*stack check, breaks past 65145*/
    /*printf("%u\n", FibbNorm(1,0,65140));*/
    CPSC mycps = 
    {Continue, &FibbNoStack, NULL, NULL, currargs};

    /*the cps loop, execute the call and get the next call, then
    execute it. Note the Try/catch style with the exception, so
    if you have exception code you can shove it all in the "exception
    section", and you can extend the enum to have different types of
    exceptions. Note, this isn't really "true" cps, this is a procedural
    rewrite for lack of any type of tail call optimization
    
    this chunk of code could be thought of as a "dispatch agent
    for cps structs
    */
    while(mycps.Status)
    {
        switch(mycps.Status)
        {
            case Continue : 
                if(*mycps.Continuation)
                {
                    mycps = (*mycps.Continuation)(mycps.Args);
                }
                else
                {
                    printf("continuation failed");
                    mycps.Status = Exception;
                }
                    break;
            case Halt :
                if(*mycps.Finally)
                {
                    (*mycps.Finally)();
                }
                break;
            case Exception :
                if(*mycps.Exn)
                   (*mycps.Exn)(Exception);
                break;
            default :
                printf("code outdated, severe error occurred");
                break;
        }
    }

    /*print something useful and get out*/
    /*bug...this doesn't quite format correctly, no idea why*/
    printf("%lu\n", currargs->N);
                
    return 0;
}


/*n is the nth-1 value, minn is the nth -2 value, depth is the
number to calculate(fails on 0)*/
static long FibbNorm(int const n, int const minn, int const depth)
{
    /*if this isn't 1 then you get off by one, since you start with
    0 and 1, so if you want 1, then you do nothing, if you want 2
    you do 1 iter etc.*/
    if(depth > 1)
        return FibbNorm((n+minn), n, depth-1);
    else
        return n;
}

/*sets up the continuation based on the depth, and returns it*/
static CPSC FibbNoStack(void * args)
{
    FibbArgs * localdata = ((FibbArgs *)args);
    long long int storelocal;

    CPSC toret;

    if(localdata->Depth > 1)
    {
        storelocal = localdata->N + localdata->N_Min1;
        localdata->N_Min1 = localdata->N;
        localdata->N = storelocal;
        toret.Status = Continue;
        toret.Args = localdata;
        toret.Continuation = &FibbNoStack;
    }
    else
    {
        toret.Status = Halt;
        toret.Continuation = NULL;
    }
    localdata->Depth = localdata->Depth - 1;

    toret.Exn = NULL;
    toret.Finally = NULL;
    toret.Args = localdata;
    return toret;
}
