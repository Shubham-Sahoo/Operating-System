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
#include <cassert>

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

	pipe = check_pipe(input,split_pipe);

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
	string s="";
	int j=0;
	for(int i=1;i<1024;i++)
	{	
		if(parsed_com[i].size()>0)
		{
			j++;
		}
	}
	//full = new char*[j-1];

	for(int i=1;i<j;i++)
	{	
		full[i-1] = &parsed_com[i][0];
	}
	//cout<<input<<" "<<flush;

	cout<<full[0]<<" "<<flush;
	cout<<"Hi";

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 
	else if (pid == 0) 
	{ 
		if (execlp(input, input, full , (char *)NULL)< 0)  //    const_cast<char* const*>(full)  "/bin/sh","/bin/sh", "-c",  ...  ,  (char *)NULL)
		{ 
			printf("\nCould not execute command.."); 
		} 
		exit(0); 
	} 
	else 
	{ 
		
		wait(NULL); 
		return; 
	} 

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
		{
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