/*
 *
 *  Created on: Sep 25, 2016
 *      Author: Kevin Vu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#define MAX_LEN  80

void execute(char **args) {
	pid_t pid;
	int indexOfAmpersand;
	bool ampersandFlag = false;
	int status;

	/** loop through args to find if it contains an ampersand and gets its index */
	for (int j = 0; j < MAX_LEN / 2 + 1; j++)
	{
		if (args[j] != NULL && strcmp(args[j],"&") == 0 && j != MAX_LEN / 2 + 1 && args[j + 1] == NULL)
		{
			indexOfAmpersand = j;
			ampersandFlag = true;
			break;
		}
	}

	if ((pid = fork()) < 0)
	{ 	/* fork a child process           */
		printf("Error : Forked Failed\n");
	}

	/* for the child process */
	else if (pid == 0)
	{
		if (ampersandFlag)
		{
			/* sets the ampersand to NULL in order to execute */
			args[indexOfAmpersand] = NULL;
			if (execvp(args[0], args) < 0) /* execute the command  */
			{
				printf("Error : Exec Failed\n");
				exit(0);
			}
		}
		/* executes normally if there is no ampersand */
		else
		{
			 if (execvp(args[0], args) < 0)  /* execute the command  */
			 {
				printf("Error : Exec Failed\n");
				exit(0);
			 }
		}
	}
	 /* for the parent: */
	else
	{
		// if there is an ampersand, make child process run in the background
		if (ampersandFlag)
		{
			waitpid(-1, &status, WNOHANG);
		}
		// else make parent wait for child process
		else
		{
			waitpid(pid,&status,0);
		}
	}
}

int main(void) {
	char *token;
	int should_run = 1;
	char input[MAX_LEN / 2 + 1];
	char *args[MAX_LEN / 2 + 1];

	printf("Shell from Kevin Vu\n");

	while (should_run) {
		/* show a prompt */
		printf("Kevin-Shell ");

		fflush(stdout);

		/* read a line */
		fgets(input, sizeof(input), stdin);

		/* exit this shell when "exit" is inputed */
		if (strcmp(input, "exit\n") == 0) {
			printf("Goodbye!\n");
			should_run = 0;
			exit(0);
		}

		/* split input for every space */
		token = strtok(input, " \n");
		int i = 1;
		args[0] = token;
		while (token = strtok(NULL, " \n")) {
			args[i] = token;
			i++;
		}

		/* Make end of char array empty for execute */
		for (int j = i; j < MAX_LEN / 2 + 1; j++)
		{
			args[j] = NULL;
		}
	
		//executes arguments
		execute(args);
	}
}
