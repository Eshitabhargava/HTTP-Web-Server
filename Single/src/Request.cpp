#include<iostream>
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<Request>

using namespace wp;
using namespace std;

void Request::forward(string forwardTo)
{
this->forwardTo=forwardTo;
}

string Request::get(string name)
{
string res;
int row,col;
for(row=0;row<this->dataCount;++row)
{
for(col=0;data[row][col] && data[row][col]!='=';++col);
if(data[row][col]!='=') continue;
if(strncmp(data[row],name.c_str(),col)==0) break;
}
if(row==this->dataCount) res="";
else res=string(data[row]+(col+1));
return res;
}
