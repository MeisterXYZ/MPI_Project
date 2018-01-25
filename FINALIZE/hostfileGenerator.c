#import <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //get inputed number as int
    int hosts = 0;
    int i;
    char *value;
    for(i = 1; i < argc; i++)
    {
        value = argv[i];
        hosts += atoi(value);
    }

    char name[30]; 
    sprintf(name, "myhostfile%d", hosts);
    printf("%s\n",name);

    FILE *fp;
	fp = fopen(name, "w");
    
    div_t fullClients = div(hosts,4);
    
    for (i = 0;i<fullClients.quot;i++){
        fprintf(fp, "simson%02d slots = 4\n",i+1);
    }
    if (fullClients.rem != 0){
        fprintf(fp, "simson%02d slots = %d\n",fullClients.quot +1,fullClients.rem);
    }

	fclose(fp);
}