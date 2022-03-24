#include <stdio.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>

#include <arpa/inet.h>

#include <stdlib.h>

//main function

int main(int argc, char *argv[]){

   if(argc!=2){printf("Wrong Arguments\n"); return 0;}

  

   int cskt,n,bytes_sent, bytes_recv,choice,score,checkend;

   char buffer[1024*4],userid[1024],fname[1024],lname[1024];

   struct sockaddr_in sadr,from;

   socklen_t addr_size;

   //argv[1] contains the port details of server

   //create a struct with these details and localhost

   cskt = socket(PF_INET, SOCK_DGRAM, 0);

   sadr.sin_family = AF_INET;

   sadr.sin_port = htons(atoi(argv[1]));

   sadr.sin_addr.s_addr = inet_addr("127.0.0.1");

   memset(sadr.sin_zero, '\0', sizeof sadr.sin_zero);

   addr_size = sizeof(sadr);

   //ask user the option

   printf("Choose an option.\n1. ADD\n2.Display ID\n3.Display Score\n4.Display All\n5.Delete ID\n6.Exit\n");

   scanf("%d",&choice);

   while(choice!=6){

       //send the option first to server

       bytes_sent = sendto(cskt,&choice,sizeof(int),0,(struct sockaddr *) &sadr,addr_size);

       if(bytes_sent<0){ printf("Error in sending\n"); }

       bzero(buffer,sizeof(buffer));

       bytes_recv = recvfrom(cskt, buffer, 1024, 0,(struct sockaddr *) &from,&addr_size);

       if(bytes_recv<0){ printf("Error in recieving"); }      

       //if choice is 1 then add user to database

       if(choice==1){

           //Ask user to enter the details

           printf("Enter ID, First Name, Last Name, Score\n" );

           scanf("%s%s%s%d",userid,fname,lname,&score);

           bzero(buffer,sizeof(buffer));

           sprintf(buffer,"%s,%s,%s,%d\n",userid,fname,lname,score );

           //send the details to server

           bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

           if(bytes_sent<0){ printf("Error in sending\n"); }

           bzero(buffer,sizeof(buffer));

           bytes_recv = recvfrom(cskt, buffer, 1024, 0,(struct sockaddr *) &from,&addr_size);

           if(bytes_recv<0){ printf("Error in recieving"); }      

           printf("%s\n",buffer );

       }else if(choice==2){

           //if option 2 then print the details of id

           //ask user to enter user id

           printf("Enter ID\n" );

           scanf("%s",userid);

           bzero(buffer,sizeof(buffer));

           sprintf(buffer,"%s\n",userid);

           //send the user details to server

           bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

           if(bytes_sent<0){ printf("Error in sending\n"); }

           bzero(buffer,sizeof(buffer));

           //get the details from server

           bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

           if(bytes_recv<0){ printf("Error in recieving"); }      

           printf("%s\n",buffer );

       }else if(choice==3){

           //if option 3 then print the score

           printf("Enter Score\n" );

           scanf("%d",&score);

           //send the score to server

           bytes_sent = sendto(cskt,&score,sizeof(int),0,(struct sockaddr *) &sadr,addr_size);

           if(bytes_sent<0){ printf("Error in sending\n"); }

           bzero(buffer,sizeof(buffer));

           //start receving details of user greater than score from server

           //there might be lot of users so keep a loop until we receive all the users

           bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

           if(bytes_recv<0){ printf("Error in recieving"); }

           sscanf(buffer,"%d",&checkend);

           //loop to get all the details from server

           while(checkend!=2){

               // printf("%s\n",buffer );

               sscanf(buffer,"%d,%[^,],%[^,],%[^,],%d",&checkend,userid,fname,lname,&score);

               printf("%s %s %s %d\n",userid,fname,lname,score );

               bzero(buffer,sizeof(buffer));

               sprintf(buffer,"ok");

               bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

               if(bytes_sent<0){ printf("Error in sending\n"); }

               bzero(buffer,sizeof(buffer));

               bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

               if(bytes_recv<0){ printf("Error in recieving"); }

               sscanf(buffer,"%d",&checkend);

           }

       }else if(choice==4){

           //Print the users

           sprintf(buffer,"Start Sending\n");

           //Ask server to start sending

           bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

           if(bytes_sent<0){ printf("Error in sending\n"); }

           bzero(buffer,sizeof(buffer));

           //start receving from server

           bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

           if(bytes_recv<0){ printf("Error in recieving"); }

           sscanf(buffer,"%d",&checkend);

           while(checkend!=2){

               // printf("%s\n",buffer );

               sscanf(buffer,"%d,%[^,],%[^,],%[^,],%d",&checkend,userid,fname,lname,&score);

               printf("%s %s %s %d\n",userid,fname,lname,score );

               bzero(buffer,sizeof(buffer));

               sprintf(buffer,"ok");

               bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

               if(bytes_sent<0){ printf("Error in sending\n"); }

               bzero(buffer,sizeof(buffer));

               bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

               if(bytes_recv<0){ printf("Error in recieving"); }

               sscanf(buffer,"%d",&checkend);

           }

       }else if(choice==5){

           //delete a id

           printf("Enter ID\n" );

           scanf("%s",userid);

           bzero(buffer,sizeof(buffer));

           sprintf(buffer,"%s\n",userid);

           //send id to server

           bytes_sent = sendto(cskt,buffer,strlen(buffer),0,(struct sockaddr *) &sadr,addr_size);

           if(bytes_sent<0){ printf("Error in sending\n"); }

           bzero(buffer,sizeof(buffer));

           //receive the acknowledgement

           bytes_recv = recvfrom(cskt, buffer, 1024*4, 0,(struct sockaddr *) &from,&addr_size);

           if(bytes_recv<0){ printf("Error in recieving"); }      

           printf("%s\n",buffer );

       }

       printf("Choose an option.\n1. ADD\n2.Display ID\n3.Display Score\n4.Display All\n5.Delete ID\n6.Exit\n");

       scanf("%d",&choice);

   }

   //send that the program have to end

   bytes_sent = sendto(cskt,&choice,sizeof(int),0,(struct sockaddr *) &sadr,addr_size);

   if(bytes_sent<0){ printf("Error in sending\n"); }

   bytes_recv = recvfrom(cskt, buffer, 1024, 0,(struct sockaddr *) &from,&addr_size);

   if(bytes_recv<0){ printf("Error in recieving"); }

   return 0;

}
