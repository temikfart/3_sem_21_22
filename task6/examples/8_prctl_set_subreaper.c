// inspired from: https://stackoverflow.com/questions/56856275/is-there-some-short-example-of-how-to-use-prctl-when-it-comes-to-setting-subre
// note: your kernel should be 3.4 upstream or above

#include <stdio.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
    int *status;
    int i=0;

    prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
    perror("PARENT:Set");
    printf("PARENT: %d :  my dad : %d\n", getpid(), getppid());
    if(fork() != 0)
    {
        while(1)
        {
            wait(status);
            if(++i == 2)
            {
                break;
            }
        }
        int p = 1;
        prctl(PR_GET_CHILD_SUBREAPER, &p);
        printf("PARENT : %d\n",p);
        printf("PARENT Exiting\n");
    }
    else
    {
        printf("Before CHILD: %d: my dad  %d\n",getpid(), getppid());
        if(fork() == 0)
        {
            int p = 1;
            printf("Before grandchild: %d: my dad %d\n",getpid(), getppid());
            sleep(2);
            printf("After grandchild: %d: my dad %d\n",getpid(), getppid());
            prctl(PR_GET_CHILD_SUBREAPER, &p);
            printf("After grandchild : %d\n",p);
            printf("Grandchild Exiting\n");
            exit(0);
        }
        else
        {
            int p = 1;
            prctl(PR_GET_CHILD_SUBREAPER, &p);
            printf("After CHILD : %d\n",p);
            printf("After CHILD: %d: my dad  %d\n",getpid(), getppid());
            printf("CHILD Exiting\n");
            exit(1);
        }
    }   
    return 0;
}
