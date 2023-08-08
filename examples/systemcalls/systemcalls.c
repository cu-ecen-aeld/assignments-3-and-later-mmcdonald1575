#include "systemcalls.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

{
	int result;
	result = system(cmd);

	if(WIFEXITED(result))
	{
		if(WEXITSTATUS(result)==0)
		{
    			return true;
    		}
    		else
    		{
    			return false;
    		}
	}
	else
	{
		return false;
	}
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    command[count] = command[count];
    
    va_end(args);
     
     int execResult = 0;
     pid_t pid_for_exec;
     int wait_result = 0;
     
     pid_for_exec = fork();
     
     
     if (pid_for_exec == 0)
     {
     	execResult = execv(command[0], command);
     	exit(-1);
     	
     }
     else
     {
      	return false;
     }
     
     wait_result = waitpid(pid_for_exec, &execResult, 0);  
     if (wait_result < 0)
     {
     	return false;
     }   
     if(WIFEXITED(execResult) && (WEXITSTATUS(execResult)==0))
     {
    	return true;
     }
     
     else
     {
	return false;
     }
}


bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    command[count] = command[count];
	
    va_end(args);	
	
    pid_t kidpid;
    int execFileResult;
    int fd = 0;
    int fd2 = 0;
    int pidresult = 0;
    
    fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);

if (fd < 0) 
{ 
	perror("open"); 
	return false; 
}

kidpid = fork();
if (kidpid < 0) 
{
  	//perror("fork"); 
  	return false;
}
else
{
    fd2 = dup2(fd, 1);
    close(fd);
    if (fd2 < 0) 
    { 
    	perror("dup2"); 
    	exit(-1); 
    }
    else
    {
    	//close(fd);
    	execv(command[0], command);
    	perror("execv");
    	exit(-1);
    	pidresult = waitpid(kidpid,&execFileResult,0); 
    	//close(fd);
    }
    if(pidresult == -1)
    {
    	return false;
    }
    if(WIFEXITED(execFileResult))
	{
		 if(WEXITSTATUS(execFileResult)==0)
		 {
    			return true;
    		 }
    		 else
    		 {
    		 	return false;
		 }
	}
    else
	{
		return false;
	}
}
}

