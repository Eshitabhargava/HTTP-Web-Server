#include<stdio.h>
#include<windows.h>
#include<string.h>

#include<WebProjector>
#include<Request>
#include<Response>

using namespace wp;

int extensionEquals(const char *left,const char *right)
{
char ll,rr;
while(*left && *right)
{
ll=*left;
rr=*right;
if(ll>=65 && ll<=90) ll+=32;
if(rr>=65 && rr<=90) rr+=32;
if(ll!=rr) return 0;
++left;
++right;
}
return *left==*right;
}

char* getMIMEType(char *resource)
{
char *mimeType;
mimeType=NULL;
int len=strlen(resource);
if(len<4) return mimeType;
int lastIndexOfDot=len-1;
while(lastIndexOfDot>0 && resource[lastIndexOfDot]!='.') --lastIndexOfDot; 
if(lastIndexOfDot==0) return mimeType; 
if(extensionEquals(resource+lastIndexOfDot+1,"html"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
}
else if(extensionEquals(resource+lastIndexOfDot+1,"css"))
{
mimeType=(char *)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
}
else if(extensionEquals(resource+lastIndexOfDot+1,"js"))
{
mimeType=(char *)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
mimeType[15]='\0';
}
else if(extensionEquals(resource+lastIndexOfDot+1,"jpg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
else if(extensionEquals(resource+lastIndexOfDot+1,"jpeg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
else if(extensionEquals(resource+lastIndexOfDot+1,"png"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
}
else if(extensionEquals(resource+lastIndexOfDot+1,"ico"))
{
mimeType=(char *)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");
}
return mimeType;
}

char isClientSideResource(char *resource)
{
//As of now: No extension=Server side technology
int idx;
for(idx=0;resource[idx]!='\0' && resource[idx]!='.';++idx);
if(resource[idx]=='\0') return 'N';
return 'Y'; 
}

Request* parseRequest(char *bytes)
{
char method[11];
char resource[1001];
int i,j;
for(i=0;bytes[i]!=' ';++i) method[i]=bytes[i];
method[i]='\0';
i+=2; 

char **data=NULL;
int dataCount=0;
if(strcmp(method,"GET")==0)
{
for(j=0;bytes[i]!=' ';++i,++j)
{
if(bytes[i]=='?') break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
++i;
int startingIdx=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&') ++dataCount;
++i;
}
++dataCount;
data=(char **)malloc(sizeof(char *)*dataCount);
int *pc=(int *)malloc(sizeof(int)*dataCount);
i=startingIdx;
int j=0;
while(bytes[i]!=' ')	
{
if(bytes[i]=='&')
{
pc[j]=i;
++j;
}
++i;
}
pc[j]=i;
i=startingIdx;
j=0;
int howManyToPick;
while(j<dataCount)
{
howManyToPick=pc[j]-i;
data[j]=(char *)malloc(sizeof(char)*(howManyToPick+1));
strncpy(data[j],bytes+i,howManyToPick);
data[j][howManyToPick]='\0';
i=pc[j]+1;
++j;
}
}
} 

printf("Request arrived for %s\n",resource);
Request *request=new Request;
request->dataCount=dataCount;
request->data=data;
request->method=(char *)malloc((sizeof(char)*strlen(method)+1));
strcpy(request->method,method);
if(resource[0]=='\0') 
{
request->resource=NULL;
request->isClientSideTechnologyResource='Y';
request->mimeType=NULL;
}
else
{
request->resource=(char *)malloc((sizeof(char)*strlen(resource)+1));
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(resource);
request->mimeType=getMIMEType(resource);
}
return request;
}

WebProjector::WebProjector(int portNumber)
{
this->portNumber=portNumber;
this->url=NULL;
this->ptrOnRequest=NULL;
}

WebProjector::~WebProjector()
{
if(this->url) delete [] this->url;
}

void WebProjector::onRequest(const char *url,void (*ptrOnRequest)(Request &,Response &))
{
if(this->url) delete [] this->url; 
this->url=NULL;
this->ptrOnRequest=NULL;
if(!url || !ptrOnRequest) return;
this->url=new char[strlen(url)+1];
strcpy(this->url,url);
this->ptrOnRequest=ptrOnRequest;
}


void WebProjector::start()
{
FILE *file;
int j,i;
int readCount;
char g;
WORD ver;
WSADATA wsaData;

char responseBuffer[1024];
char requestBuffer[8192]; 

int serverSocketDescriptor;
int clientSocketDescriptor;
struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;
int successCode;
int len;
long int length;
int bytesExtracted;
char message[101];

WSAStartup(ver,&wsaData);
ver=MAKEWORD(1,1);
WSAStartup(ver,&wsaData); //socket library initialized
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to create socket\n");
return;
}

serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
if(successCode<0)
{
sprintf(message,"Unable to bind socket to port %d",this->portNumber);
printf("%s\n",message);
WSACleanup();
return;
}
listen(serverSocketDescriptor,10);
len=sizeof(clientSocketInformation);
while(1)
{
sprintf(message,"Server is ready to accept request on Port %d",this->portNumber);
printf("%s\n",message);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("Unable to accept Client Connection.\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}
bytesExtracted=recv(clientSocketDescriptor,requestBuffer,8192,0);
if(bytesExtracted<0)
{
//later
}
else if(bytesExtracted==0)
{
//later
}
else
{
requestBuffer[bytesExtracted]='\0';
Request *request=parseRequest(requestBuffer);
while(1) 
{   
if(request->isClientSideTechnologyResource=='Y')
{
if(request->resource==NULL) 
{
file=fopen("index.html","rb");
if(file) printf("Sending index.html\n");
if(!file)
{
file=fopen("index.htm","rb");
if(file) printf("Sending index.htm\n");
}
if(!file)
{
strcpy(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:160\nConnection: close\n\n<DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>Web Projector</title></head><body><h2 style='color:red'>Resource / not found</h2></body></html>");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 Page\n");
closesocket(clientSocketDescriptor); 
break; 
}
else
{
fseek(file,0,2); 
length=ftell(file);
fseek(file,0,0); 
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n",length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
readCount=length-i;
if(readCount>1024) readCount=1024;
fread(&responseBuffer,readCount,1,file);
send(clientSocketDescriptor,responseBuffer,readCount,0);
i+=readCount;
}
fclose(file);
closesocket(clientSocketDescriptor);
break;
} 
} 
else
{
file=fopen(request->resource,"rb");
if(file) printf("Sending %s\n",request->resource);
if(!file)
{
char temp[1024];
sprintf(temp,"<DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>Web Projector</title></head><body><h2 style='color:red'>Resource /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n",strlen(temp));
strcat(responseBuffer,temp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 Page\n");
closesocket(clientSocketDescriptor);
break;
} 
else
{
fseek(file,0,2); 
length=ftell(file);
fseek(file,0,0); 
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nConnection: close\n\n",request->mimeType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<length)
{
readCount=length-i;
if(readCount>1024) readCount=1024;
fread(&responseBuffer,readCount,1,file);
send(clientSocketDescriptor,responseBuffer,readCount,0);
i+=readCount;
}
fclose(file);
closesocket(clientSocketDescriptor);
break;
}
} 
} 
else 
{
if(!this->url || !this->ptrOnRequest) 
{
char temp[1024];
sprintf(temp,"<DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>Web Projector</title></head><body><h2 style='color:red'>Resource /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n",strlen(temp));
strcat(responseBuffer,temp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 Page\n");
closesocket(clientSocketDescriptor);
break;
} 
else 
{
int ii=0;
if(this->url[0]=='/') ii=1; 
if(strcmp(this->url+ii,request->resource)==0) 
{
Response response(clientSocketDescriptor);
this->ptrOnRequest(*request,response);
if(request->forwardTo.length()>0) 
{
free(request->resource);
request->resource=(char *)malloc((sizeof(char)*request->forwardTo.length())+1);
strcpy(request->resource,request->forwardTo.c_str());
request->isClientSideTechnologyResource=isClientSideResource(request->resource);
request->mimeType=getMIMEType(request->resource);
continue;
}
if(request->data)
{
for(int i=0;i<request->dataCount;++i) free(request->data[i]);
free(request->data);
}
printf("Sending Processed Page\n");
closesocket(clientSocketDescriptor);
break;
}
else
{
char temp[1024];
sprintf(temp,"<DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>Web Projector</title></head><body><h2 style='color:red'>Resource /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\n\n",strlen(temp));
strcat(responseBuffer,temp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 Page\n");
closesocket(clientSocketDescriptor);
break;
}
} 
}
} 
}
}
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}
