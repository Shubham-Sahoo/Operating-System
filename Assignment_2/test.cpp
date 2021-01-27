#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 
#include<iostream>
#include<vector>

#define MAX_LENGTH 1024


using namespace std;

void print_direc()
{
	char *usr = new char[MAX_LENGTH];
	char *hst = new char[MAX_LENGTH];
	char *dir = new char[MAX_LENGTH];
 	
	//char dir[1024];

	getlogin_r(usr, MAX_LENGTH);
	gethostname(hst, MAX_LENGTH);
	getcwd(dir, MAX_LENGTH);    

    cout<<""<<usr<<"@"<<hst<<":"<<string(dir)<<"$ ";
    delete(usr);
    delete(hst);
    delete(dir);
}

int check_pipe(string input, string* split_pipe)
{
	int i; 
	char *in = &input[0];
	char *out;

	for (i = 0; i < MAX_LENGTH; i++) 
	{ 
		out = strsep(&in, "|");
		if(out!=NULL)
			split_pipe[i] = out;
		if (out==NULL) 
		{	
			break; 
		}
	}
	
	if (split_pipe[1].size()==0) 
		return 0;
	else { 
		return i; 
	} 

}

void parse(string input, string *parsed_com)
{
	int i; 
	char *in = &input[0];
	char *out;
	for (i = 0; i < MAX_LENGTH; i++) 
	{ 
		out = strsep(&in, " ");
		if(out!=NULL)
			parsed_com[i] = out;

		if (out == NULL) 
		{
			break;
		}
		if (parsed_com[i].size() == 0)
		{
			i--; 
		}

	}

}

void parse_piped(string *split_pipe, string **parsed_pipe, int n)
{
	int i;
	string *pr_val;
	for(i=0;i<n;i++)
	{	
		pr_val = new string[MAX_LENGTH];
		parse(split_pipe[i],pr_val);
		parsed_pipe[i] = pr_val;
	}

}

int split_fn(string input, string *parsed_com, string **parsed_pipe)
{
	string *split_pipe = new string[MAX_LENGTH];
	
	int pipe = 0;
	cout<<pipe<<" "<<flush;
	pipe = check_pipe(input,split_pipe);

	cout<<pipe<<" "<<flush;
	if(pipe>0)
	{
		parse_piped(split_pipe, parsed_pipe, pipe);
	}
	else
	{
		parse(split_pipe[0],parsed_com);
	}
	return pipe;
}

void launch_proc(string *parsed_com)
{
	pid_t pid = fork(); 

	const char *input;
	input = &parsed_com[0][0];

	char *full[1024];
	for(int i=1;i<1024;i++)
	{	
		full[i] = &parsed_com[i][0];
	}

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 
	else if (pid == 0) 
	{ 
		if (execvp(input, full) < 0) 
		{ 
			printf("\nCould not execute command.."); 
		} 
		cout<<"Executed";
		exit(0); 
	} 
	else 
	{ 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 

}

void launch_io_redirect(string *parsed_com)
{
	if (parsed_com[0] == NULL){	// Empty command
		return 1;
	}

	// Copy the current Standard Input and Output file descriptors
	// so they can be restored after executing the current command
	int std_in, std_out, std_err;
	std_in = dup(0);
	std_out = dup(1);
	std_err = dup(2);

	// Check if redirection operators are present
	int i = 1;

	while ( parsed_com[i] != NULL ){
		if ( strcmp( parsed_com[i], "<" ) == 0 ){	// Input redirection
			int inp = open( parsed_com[i+1], O_RDONLY );
			if ( inp < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minsh");
				return 1;
			}
			close(inp);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;
		}
		else if ( strcmp( parsed_com[i], "<<" ) == 0 ){	// Input redirection
			int inp = open( parsed_com[i+1], O_RDONLY );
			if ( inp < 0 ){

				perror("minsh");
				return 1;
			}

			if ( dup2(inp, 0) < 0 ){
				perror("minsh");
				return 1;
			}
			close(inp);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( parsed_com[i], ">") == 0 ){	// Output redirection

			int out = open( parsed_com[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minsh");
				return 1;
			}
			close(out);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( parsed_com[i], ">>") == 0 ){	// Output redirection (append)
			int out = open( parsed_com[i+1], O_WRONLY | O_APPEND | O_CREAT, 0755 );
			if ( out < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(out, 1) < 0 ){
				perror("minsh");
				return 1;

			}
			close(out);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( parsed_com[i], "2>") == 0 ){	// Error redirection
			int err = open( parsed_com[i+1], O_WRONLY | O_CREAT, 0755 );
			if ( err < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(err, 2) < 0 ){
				perror("minsh");
				return 1;
			}
			close(err);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;
		}
		else if( strcmp( parsed_com[i], "2>>") == 0 ){	// Error redirection
			int err = open( parsed_com[i+1], O_WRONLY | O_CREAT | O_APPEND, 0755 );

			if ( err < 0 ){
				perror("minsh");
				return 1;
			}

			if ( dup2(err, 2) < 0 ){
				perror("minsh");
				return 1;
			}
			close(err);
			parsed_com[i] = NULL;
			parsed_com[i+1] = NULL;
			i += 2;

		}
		else{
			i++;
		}
	}

	// If the command is a built-in command, execute that function
	for(i = 0 ; i < BUILTIN_COMMANDS ; i++){
		if ( strcmp(parsed_com[0], builtin[i]) == 0 ){
			int ret_status = (* builtin_function[i])(parsed_com);
			
			// Restore the Standard Input and Output file descriptors
			dup2(std_in, 0);
			dup2(std_out, 1);
			dup2(std_err, 2);

			return ret_status;
		}
	}

	// For other commands, execute a child process
	int ret_status = launch_proc(parsed_com);

	// Restore the Standard Input and Output file descriptors
	dup2(std_in, 0);
	dup2(std_out, 1);
	dup2(std_err, 2);

	return ret_status;
}


int main()
{

	string input;
	string *parsed_com, **parsed_pipe;
 
	int i=1,pipe_val=0;
	
	while(1)
	{	
		parsed_com = new string[MAX_LENGTH];
		parsed_pipe = new string*[MAX_LENGTH];
		print_direc();
		getline(cin,input);
		if(input.size()==0)
		{	
			//delete parsed_com;
			//delete parsed_pipe;
			continue;
		}

		pipe_val = split_fn(input,parsed_com,parsed_pipe);	
		//cout<<parsed_pipe[0][0]<<" "<<flush;

		if(pipe_val==0)
		{	int chk = 0;
			for(int i=1;i<MAX_LENGTH;i++)
			{
				if(strcmp(parsed_com[i],"<")==0 || strcmp(parsed_com[i],">" || strcmp(parsed_com==">>")==0)
				{
					chk ++;break;
				}
			}
			 if(chk)
				launch_io_redirect(parsed_com);
			 else
				launch_proc(parsed_com);
		}
		else
		{
			//launch_proc_pipe(parsed_pipe);
		}
		
		//delete parsed_com;
		//delete parsed_pipe;
	}

	return 0;
}
