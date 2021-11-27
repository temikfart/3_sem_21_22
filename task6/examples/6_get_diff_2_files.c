/* Программа выводит в stdout разницу между двумя файлами, полученную запуском утилиты diff */
#include <stdio.h>
#include <stdlib.h>

int main()
{
        int res;
        res = system("diff file1.txt file2.txt");
        // use here dup2 to redirect file descriptor of stdout to your file
        return 0;
}
