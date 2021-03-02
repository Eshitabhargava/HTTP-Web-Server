#include<iostream>
#include<Response>
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>

using namespace wp;
using namespace std;

Response::Response(int clientSocketDescriptor)
{
this->clientSocketDescriptor=clientSocketDescriptor;
this->socketClosed=false;
this->headerExists=false;
}

void Response::createHeader()
{
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
send(this->clientSocketDescriptor,header,strlen(header),0);
this->headerExists=true;
}

void Response::write(const char* responseData)
{
if(!responseData) return;
int responseDataLen=strlen(responseData);
if(responseDataLen==0) return;
if(!this->headerExists) createHeader();
send(this->clientSocketDescriptor,responseData,responseDataLen,0);
}

void Response::close()
{
if(this->socketClosed) return;
closesocket(this->clientSocketDescriptor);
this->socketClosed=true;
}
