#include "csapp.h"
#define MAXARGS   128

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

char *defaultPrompt = "sh257";

void signal_catchfunc(int signal) {
    printf("\n%s> ", defaultPrompt);
    fflush(stdout);
    return;
}

int main(int argc, char *argv[]) {
    
  char cmdline[MAXLINE]; /* Command line */
  if (argc == 3) {
    if (strcmp(argv[1], "-p")){
        exit(0);
    }
    defaultPrompt = argv[2];
  }
  
  signal(SIGINT, signal_catchfunc);
    while (1) {
	printf("%s>",defaultPrompt);                   
	Fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	eval(cmdline);
    } 
}
  
/* eval - Evaluate command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; 
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs job */
            if (execvp(argv[0], argv) < 0) {
		printf("Execution failed (in fork)\n");
                printf("%s: Command not found.\n", argv[0]);
                exit(1);
            }
	  exit(0);
        }

	/* Parent waits for job to terminate */
	if (!bg) {
	  int status;
	  if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");
		
		int exitStatus = WEXITSTATUS(status);
	        printf("Process exited with status code %d\n", exitStatus);

	} 
	else
	    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "exit")){
      int ret = raise(SIGTERM);
    } /* quit command */
    if (!strcmp(argv[0], "&"))    /* Ignore single & */
	return 1;
	  else if(!strcmp(argv[0], "pid")){
	    printf("%d\n", getpid());
	    return 1;
	  }
	  else if(!strcmp(argv[0], "ppid")){
	    printf("%d\n", getppid());
	    return 1;
	  }
	  else if(!strcmp(argv[0], "help")){
	    printf("********************************************************************** \n");
        printf("A Custom Shell for CMSC 257\n");
        printf("    - Sohum Dharamsi\n");
        printf("Usage:\n");
        printf("    - You can change prompt by doing ./sh257 -p <newprompt> \n");
        printf("********************************************************************** \n");
        printf("BUILTIN COMMANDS: \n");
        printf("    - exit: exits the shell \n");
        printf("    - pid: prints the process id of the shell \n");
        printf("    - ppid: prints the parent process id of the shell \n");
        printf("    - help: \n");
        printf("          - prints developer name for shell \n");
        printf("          - prints usage information (how to change shell prompt, maybe list of built-in commands) \n");
	printf("    - cd: prints the current working directory (“cd <path>” will change the current working directory) \n");
        printf("SYSTEM COMMANDS: \n");
        printf("    - Use man pages to get help for system commands \n");

	    return 1;
	  }
	  else if (!strcmp(argv[0], "cd")) { /* get process id */
	  
        if (argv[1] == NULL) {
            char cwd[100];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current dir: %s\n", cwd);
            }
        else {
            printf("getcwd() error");
            return 1;
            }
        }
    else if (argv[1] != NULL) {
        chdir(argv[1]);
        }
    return 1;
    }
    return 0; 
}                

int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Create the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
