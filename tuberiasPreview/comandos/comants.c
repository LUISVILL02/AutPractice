#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv){
    int tuberia[2];
    pipe(tuberia);

    pid_t hijo = fork();

    if (hijo == -1){
        printf("error\n");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0){
        close(tuberia[1]);

        char completeComant[200];

        char result[100];
        read(tuberia[0], &result, sizeof(result));
        
        snprintf(completeComant, sizeof(completeComant), "%s %s", argv[3], argv[4]);

        FILE *fpipe = popen(completeComant, "w");
        fprintf(fpipe, "%s", result);

        fclose(fpipe);

    }else{
        close(tuberia[0]);
        char coman[200];
        snprintf(coman, sizeof(coman), "%s %s", argv[1], argv[2]);
        FILE *fpipe = popen(coman, "r");

        char result[100];
        fgets(result, sizeof(result), fpipe);

        write(tuberia[1], &result, sizeof(result));

        fclose(fpipe);
        wait(NULL);
    }
    
    return EXIT_SUCCESS;
}