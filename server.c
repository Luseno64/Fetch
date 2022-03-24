#include <stdio.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>

#include <arpa/inet.h>

#include <stdlib.h>



//We store the database as a file

//so add_user function adds the details into server

int add_user(char *userid, char *fname, char *lname, int score){

   //open the file

   FILE *fp = fopen("database.txt","a+");

   char euserid[1024],efname[1024],elname[1024],tempchar;

   int escore;

   //read from the file

   while(fscanf(fp,"%[^,],%[^,],%[^,],%d",euserid,efname,elname,&escore)==4){

       //compare the userid

       //to see if already exists

       if(strcmp(euserid, userid)==0) {fclose(fp); return -1;}

       fscanf(fp,"%c",&tempchar);

   }

   fseek(fp,0,SEEK_END);

   //if doesn't exist then append to the file

   fprintf(fp, "%s,%s,%s,%d\n", userid, fname,lname,score);

   fclose(fp);

   return 1;

}

//This function gets the details of a user given userid

char* getUserDetails(char *userid){

   char *retstr,tempchar;

   //retstr to return the details to main function

   retstr = (char*)malloc(sizeof(char)*1024*4);

   //open the file

   FILE *fp = fopen("database.txt","r");

   char euserid[1024],efname[1024],elname[1024];

   int escore;

   //read from the file

   while(fscanf(fp,"%[^,],%[^,],%[^,],%d",euserid,efname,elname,&escore)==4){

       //compare with the userid

       if(strcmp(euserid, userid)==0) {

           bzero(retstr,sizeof(retstr));

           sprintf(retstr,"%s,%s,%s,%d",euserid,efname,elname,escore);

           fclose(fp);

           return retstr;

       }

       fscanf(fp,"%c",&tempchar);

   }

   fclose(fp);

   return NULL;

}

//This function deletes from file a user

int deleteID(char *userid){

   //open the file

   FILE *fp = fopen("database.txt","r");

   //create the temp file

   FILE *fp2 = fopen(".tmp.database.txt","w");

   char euserid[1024],efname[1024],elname[1024],tempchar;

   int escore,checkUser=0;

   //read from the file

   while(fscanf(fp,"%[^,],%[^,],%[^,],%d",euserid,efname,elname,&escore)==4){

       //compare with user_id

       //if it matches then don't insert into the new temp file

       //else insert into the new temp file

       if(strcmp(euserid, userid)!=0) fprintf(fp2, "%s,%s,%s,%d\n", euserid, efname,elname,escore);

       else checkUser = 1;

       fscanf(fp,"%c",&tempchar);

   }

   fclose(fp);

   fclose(fp2);

   //new temp file contains the database with userid deleted

   //so remove the original file

   remove("database.txt");

   //rename the new file

   rename(".tmp.database.txt","database.txt");

   if(checkUser==0) return -1;

   else return 1;

}

int main(int argc, char *argv[]){

   if(argc!=2){printf("Wrong Arguments\n"); return 0;}

   int sskt, fromLen,n,choice,score,retval;

   char buffer[1024*4],userid[1024],fname[1024],lname[1024],*retstr,tempchar;

   struct sockaddr_in sadr,from;

   struct sockaddr_storage serverStorage;

   socklen_t addr_size;

   //create a struct with the details

   sskt = socket(PF_INET, SOCK_DGRAM, 0);

   sadr.sin_family = AF_INET;

   sadr.sin_port = htons(atoi(argv[1]));

   sadr.sin_addr.s_addr = inet_addr("127.0.0.1");

   memset(sadr.sin_zero, '\0', sizeof sadr.sin_zero);

   //bind the socket

   bind(sskt, (struct sockaddr *) &sadr, sizeof(sadr));

   fromLen = sizeof(struct sockaddr_in);

   choice=1;

   while(choice!=6){

       bzero(buffer,1024);

       //receive the option selected

       n = recvfrom(sskt, &choice, sizeof(int), 0,(struct sockaddr *) &from, &fromLen );

       if(n<0) printf("error in receiving");

       printf("Option %d choosen\n",choice);

       bzero(buffer,sizeof(buffer));

       sprintf(buffer,"ok");

       sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

       //option 1 to add the user

       if(choice==1){

           bzero(buffer,1024*4);

           n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

           if(n<0) printf("error in receiving");

           //retrieve all the details from buffer

           sscanf(buffer,"%[^,],%[^,],%[^,],%d",userid,fname,lname,&score);

           //call add_user to add into database

           retval = add_user(userid,fname,lname,score);

           bzero(buffer,sizeof(buffer));

           if(retval==-1) sprintf(buffer,"USERID already exists\n");

           else sprintf(buffer,"Successfully entered\n");

           sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

       }else if(choice==2){

           //send the details of userid

           bzero(buffer,1024*4);

           //get the user id from client

           n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

           if(n<0) printf("error in receiving");

           sscanf(buffer,"%s",userid);

           //get the details using the funciton getUserDetails

           retstr = getUserDetails(userid);

           bzero(buffer,sizeof(buffer));

           if(retstr==NULL) sprintf(buffer,"USERID Doesn't exist\n");

           else sprintf(buffer,"%s\n",retstr);

           //send to client

           sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

       }else if(choice==3){

           //send the scores greater than given score

           //open the file

           FILE *fp = fopen("database.txt","r");

           char euserid[1024],efname[1024],elname[1024];

           int escore;

           //receive the score from client

           n = recvfrom(sskt, &score, sizeof(int), 0,(struct sockaddr *) &from, &fromLen );

           if(n<0) printf("error in receiving");

           //get the user details from file and compare

           while(fscanf(fp,"%[^,],%[^,],%[^,],%d",euserid,efname,elname,&escore)==4){

               if(escore>score) {

                   bzero(buffer,sizeof(buffer));

                   sprintf(buffer,"1,%s,%s,%s,%d",euserid,efname,elname,escore);

                   //send the details to client

                   sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

                   n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

                   if(n<0) printf("error in receiving");

               }

               fscanf(fp,"%c",&tempchar);

           }

           bzero(buffer,sizeof(buffer));

           sprintf(buffer,"2\n");

           //send ending of the sending details

           sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

           fclose(fp);

       }else if(choice==4){

           //print all the users

           FILE *fp = fopen("database.txt","r");

           char euserid[1024],efname[1024],elname[1024];

           int escore;

           bzero(buffer,1024*4);

           //Wait until client asks to send

           n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

           if(n<0) printf("error in receiving");

           printf("%s\n",buffer );

           while(fscanf(fp,"%[^,],%[^,],%[^,],%d",euserid,efname,elname,&escore)==4){

               bzero(buffer,sizeof(buffer));

               sprintf(buffer,"1,%s,%s,%s,%d",euserid,efname,elname,escore);

               //send each user details

               sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

               n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

               if(n<0) printf("error in receiving");

               fscanf(fp,"%c",&tempchar);

           }

           bzero(buffer,sizeof(buffer));

           sprintf(buffer,"2\n");

           //send ending of the sending details

           sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

           fclose(fp);

       }else if(choice==5){

           //delete a userid

           bzero(buffer,1024*4);

           //get the userid from client

           n = recvfrom(sskt, buffer, 1024*4, 0,(struct sockaddr *) &from, &fromLen );

           if(n<0) printf("error in receiving");

           sscanf(buffer,"%s",userid);

           //call deleteID to delete the user

           retval = deleteID(userid);

           bzero(buffer,sizeof(buffer));

           if(retval==-1) sprintf(buffer,"USERID Doesn't exist\n");

           else sprintf(buffer,"Successfully deleted\n");

           //send acknowledgment

           sendto(sskt,buffer,strlen(buffer),0,(struct sockaddr *) &from, fromLen);

       }

   }

   return 0;

}
