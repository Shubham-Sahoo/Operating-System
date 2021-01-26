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

void split_fn(string input, string *parsed_com, string **parsed_pipe)
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
	
}

int main()
{

	string input;
	string *parsed_com, **parsed_pipe;
 
	int i=1;
	
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

		split_fn(input,parsed_com,parsed_pipe);	
		cout<<parsed_pipe[0][0]<<" "<<flush;
		
		//delete parsed_com;
		//delete parsed_pipe;
	}

	return 0;
}