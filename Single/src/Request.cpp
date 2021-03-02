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
return string("2"); //Temporarily hard-coded
}
