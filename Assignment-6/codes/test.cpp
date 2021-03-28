#include <bits/stdc++.h>
#include <string.h>

using namespace std;

int main()
{
	cout<<"Hi";
	char *str;
  	str = (char*)malloc(100*sizeof(char));
  	str[0] = 'h';
  	str[1] = 'k';
  	char *token = strtok(str," ");
  	while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
	//cout<<str;
	//cout<<str2;
	return 0;
}