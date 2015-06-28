/* Just a simple executable to check install and give an example of
 * Makefile
 */
#include <pghx/logicaldecoding.h>
#include <stdlib.h>

int
main(void)
{
    pghx_ld_reader r;
    pghx_ld_reader *pr = &r;
    int res;

    res = pghx_ld_reader_init(pr);
    if (!res){
        puts("Did compile but doesn't work");
        return(1);
    }
    puts("It works!");
    return 0;
}

