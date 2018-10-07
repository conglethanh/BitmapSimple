#include "Bmp.h"

void main(int argc, char** argv)
{
	char*filePath;
	filePath = strdup(argv[1]);

	char*destFile = strdup(filePath);
	destFile[strlen(destFile)-4] = '\0';
	strcat(destFile, "_copy");

	int catDoc = 1, catNgang = 1;

	if (argc == CAT_DOC_VA_NGANG)
	{
		if (strcmp(argv[2], "-h") == 0)
			catDoc = atoi(argv[3]);
		else
			catDoc = -1;
		if (strcmp(argv[4], "-w") == 0)
			catNgang = atoi(argv[5]);
		else
			catNgang = -1;
	}
	else if (argc == CAT_DOC_HOAC_NGANG)
	{
		if (strcmp(argv[2], "-h")==0)
		{
			catDoc = atoi(argv[3]);
			
		}
		else if (strcmp(argv[2], "-w")==0)
		{
			catNgang = atoi(argv[3]);
		}
		else
		{
			catDoc = -1;
			catNgang = -1;
		}
	}
	else
	{
		printf("cannot cut Bmp!\n");
		free(filePath);
		return;
	}

	if (catDoc >= 1 && catNgang >= 1)
		printf("Bmp is cut successfully!\n");
	else
	{
		printf("cannot cut Bmp!\n");
		free(filePath);
		return;
	}

	splitBmpFile(filePath, destFile, catDoc, catNgang);

	free(filePath);
	fcloseall();
}