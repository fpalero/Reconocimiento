#include <stdio.h>
#include <string.h>

int main ()
{
   FILE * pFile;
   char sentence [256];

   printf ("Enter sentence to append: ");
   fgets (sentence,255,stdin);
   pFile = fopen ("mylog.txt","a");
   fputs (sentence,pFile);
  
   fclose (pFile);
   return 0;
}
