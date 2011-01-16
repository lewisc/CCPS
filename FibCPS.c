/*this is an experimentation in Continuation passing style
the objective is to build a generic CPS Engine that doesn't
require global variables and can execute a fibbonaci sequence
past the stack depth.

The First thing to do is to implement the fibbonacci sequence,
and then find the stack depth, then we write an CPS engine, then
we write the fibbonacci in CPS

*/

#include "stdio.h"

/*enum defining exception state, Halt and continue are designed
such that you can while on them*/
typedef enum 
{
    Halt = 0,
    Exception = -1,
    Continue = 1
} exception;
    

/*represents a continuation
has the exception status, the next pointer
the currval, the currval -1, and the continue to depth
*/
typedef struct _CPSFIBB
{
        exception Status;
        struct _CPSFIBB (*Continuation)
        (long long const, long long const, long long const);
        long long int n;
        long long int minn;
        long long int depth;
} CPSfibb;

    
    

/*normal style fibonacci sequence*/
static long FibbNorm(int const n, int const minn, int const depth);

/*CPS style fibbonacci sequence*/
static CPSfibb FibbNoStack(long long int const n, 
        long long int const minn, long long int const depth);

int main(int argc, char ** argv)
{
    
    
    /*stack check, breaks past 65145*/
    /*printf("%u\n", FibbNorm(1,0,65140));*/
    CPSfibb mycps = {Continue, &FibbNoStack, 1, 0, 10000000000LL};

    /*the cps loop, execute the call and get the next call, then
    execute it. Note the Try/catch style with the exception, so
    if you have exception code you can shove it all in the "exception
    section", and you can extend the enum to have different types of
    exceptions. Also note that in this case, default behaves as a 
    "finally"*/
    while(mycps.Status)
    {
        switch(mycps.Status)
        {
            case Continue : 
                mycps = (*mycps.Continuation)
                    (mycps.n, mycps.minn, (mycps.depth));
                break;
            case Halt :
                break;
            case Exception :
                printf("there was an error");
                break;
            default :
                printf("code outdated, severe error occurred");
                break;
        }
    }

    /*print something useful and get out*/
    printf("%lu\n", mycps.n);
                
    return 0;
}


/*n is the nth-1 value, minn is the nth -2 value, depth is the
number to calculate(fails on 0*/
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
static CPSfibb FibbNoStack
        (long long int const n, long long int const minn, long long int const depth)
{

    CPSfibb toret;
    if(depth > 1)
    {
        toret.Status = Continue;
        toret.Continuation = &FibbNoStack;
        toret.n = (n+minn);
        toret.minn = n;
        toret.depth = depth-1;
        return toret;
    }
    else
    {
        toret.Status = Halt;
        toret.Continuation = NULL;
        toret.n = n;
        toret.minn = 0;
        toret.depth = 0;
        return toret;
    }
}
