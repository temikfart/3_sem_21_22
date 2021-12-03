#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    fd_set rfds; //специальная структурка, в которую записываются файловые дескрипторы специальным макросом FD_SET
    struct timeval tv; // структурка, которая будет содержать время тайм-аута
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 5; // таймаут==5 секунд
    tv.tv_usec = 0;
    int num_of_descriptors = 1;

    retval = select(num_of_descriptors, &rfds, NULL, NULL, &tv);

    if (retval) {
        printf("Данные доступны.Читаем!\n");

        printf("Статус FD_ISSET: %d\n", FD_ISSET(0, &rfds)); //вернет истинное значение в случае успеха
        // Заметим, что для select дескрипторы приходится перебирать вручную через данный макрос
        if (FD_ISSET(0, &rfds)) {
            char str[256];
            scanf("%s", str);
            printf("%s", str);
        }
    }
    else
        printf("Timeout.\n");
    return 0;
}
