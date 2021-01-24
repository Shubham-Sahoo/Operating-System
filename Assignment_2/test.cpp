#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 
#include<iostream>

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

int main()
{

	string input;
	string *parsed_val, *parsed_pipe;
	int i=1;
	
	while(1)
	{
		print_direc();
		getline(cin,input);
		if(input.size()==0)
		{
			continue;
		}
		
		split_fn(input,parsed_val,parsed_pipe);	
		

		
	}
	return 0;
}