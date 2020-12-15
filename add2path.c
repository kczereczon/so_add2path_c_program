#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


#define HOME_BASH_RC ".bashrc"

int main(int argc, char const *argv[])
{
    if(argv[1] == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("Current working dir: %s\n\n", cwd);
            return add2path(cwd);
        }
        else
        {
            perror("getcwd() error");
            return 1;
        }
    } else {
        DIR *folder = opendir(argv[1]);
        if(folder == NULL) {
            perror("folder not found");
            return 1;
        } else {
            printf("Current working dir: %s\n\n", argv[1]);
            return add2path(argv[1]);
        }
    }
    return 0;
}

int add2path(char const *path) {
    char *home = getenv ("HOME");
    char filename[PATH_MAX] = "";
    char buf[BUFSIZ] = "";

    char *buffer;
    size_t bufsize = 32;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));

    sprintf (filename, "%s/%s", home, HOME_BASH_RC);

    FILE *file = fopen(filename, "r");
    FILE *temp =  fopen("temp", "w"); 
    if(file != NULL) {
        char const *export = "export PATH=";
        int exportFound = 0;
        while(getline(&buffer, &bufsize, file) > 0) {
            if(exportFound = strstr(buffer, export)!=NULL) {
                char *firstOccur = (char *) malloc(strlen(path)*sizeof(char));
                firstOccur = strstr(buffer, path);
                if(firstOccur==NULL) { 
                    //add dir to path
                    //removing two lat chars 
                    buffer[strlen(buffer)-2] = '\0';

                    strcat(buffer, ":");
                    strcat(buffer, path);
                    strcat(buffer, "\"\n");

                    fputs(buffer, temp);
                    printf("Added %s to PATH! \n", path);
                    break;
                } else { 
                    int start = firstOccur - buffer-1;
                    int end = firstOccur - buffer + strlen(path);

                    char *beforeCurrentPath = (char*) malloc((strlen(buffer)-strlen(path))*sizeof(char));
                    char *afterCurrentPath = (char*) malloc(strlen(buffer)*sizeof(char));
                    memcpy(beforeCurrentPath, buffer, start);
                    memcpy(afterCurrentPath, buffer+end, strlen(buffer));

                    strcat(beforeCurrentPath, afterCurrentPath);
                    fputs(beforeCurrentPath, temp);
                    printf("Removed %s from PATH! \n", path);
                }
                
            } else {
                fputs(buffer, temp);
            }
        }

        if(exportFound == 0) {
            char pathString[300] = "export PATH=\"$PATH:";
            strcat(pathString, path);
            strcat(pathString, "\"\n");
            fputs(pathString, temp);
            printf("Created PATH with %s in .bashrc! \n", path);
        }

        fclose(file);
        fclose(temp);


        /* Delete original source file */
        remove(filename);

        /* Rename temporary file as original file */
        rename("temp", filename);
    } else {
        perror("cannot open .bashrc");
        return 1;
    }

}
