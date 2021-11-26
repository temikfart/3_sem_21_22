#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


void SetPidFile(char* filename)
{
    FILE* f;

    f = fopen(filename, "w+");
    if (f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}


int DoAction() {
    while(1) {
        printf("Hop\n"); // Do something
        sleep(1);
    }
    return 0;
}


int LoadConfig(argv[1]) {
                    // Do something
    return 0;
}


int main(int argc, char** argv)
{
    int status;
    int pid;
    
    // если параметров командной строки меньше двух, то покажем как использовать демона
    if (argc != 2)
    {
        printf("Usage: ./my_daemon filename.cfg\n");
        return -1;
    }

    // загружаем файл конфигурации
    status = LoadConfig(argv[1]);
    
    if (!status) // если произошла ошибка загрузки конфига
    {
        printf("Error: Load config failed\n");
        return -1;
    }
    
    // создаем потомка
    pid = fork();

    if (pid == -1) // если не удалось запустить потомка
    {
        // выведем на экран ошибку и её описание
        printf("Error: Start Daemon failed (%s)\n", strerror(errno));
        
        return -1;
    }
    else if (!pid) // если это потомок
    {
        // данный код уже выполняется в процессе потомка
        // разрешаем выставлять все биты прав на создаваемые файлы, 
        // иначе у нас могут быть проблемы с правами доступа
        umask(0);
        
        // создаём новый сеанс, чтобы не зависеть от родителя
        setsid();
        // сохраняем PID для связи с демоном через механизм сигналов
        SetPidFile("this_daemon.pid");

        // переходим в корень диска, если мы этого не сделаем, то могут быть проблемы.
        // к примеру с размантированием дисков
        chdir("/");
        
        // закрываем дискрипторы ввода/вывода/ошибок, так как нам они больше не понадобятся
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        
        // Данная функция будет осуществлять слежение за процессом
        status = MonitorProc();
        
        return status;
    }
    else // если это родитель
    {
        // завершим процес, т.к. основную свою задачу (запуск демона) мы выполнили
        return 0;
    }
}
