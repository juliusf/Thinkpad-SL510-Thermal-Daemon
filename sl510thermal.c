#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define CHILD 0
#define ERROR -1

void start_daemon(char *log_name, int facility)
{
 int i;
 pid_t pid;

 /*kill parent process*/
 if((pid = fork()) != CHILD) exit(0);
 
 if(setsid() == ERROR)
  {
  fprintf(stderr, "%s failed to become session leader!\n",log_name);
  exit(0);
  }
 /*clean up the parent's mess... */
  chdir("/");
  umask(0);
  for(i=sysconf(_SC_OPEN_MAX); i>0; i--)
  close(i);

 /*opening the log*/

 openlog(log_name, LOG_PID, facility);
}



int getCPUTemp()
{
FILE*fp;
 char temps[56];
 int temp;

         fp = fopen ("/proc/acpi/ibm/thermal","r");
         if(fp == NULL)
         {
               syslog(LOG_ERR, "could not read thermal information. Is the ThinkPad ACPI Extras Driver installed?");
               exit(-1);
         } 
         fgets(temps, 56, fp);
	 fclose(fp); 

         temp = atoi(strndup(strchr(temps, 32)+1,2)); //get the corresponding Temp of the first core. strchr is used on the first whitespace, since i don't know if the first parameter will be populated some times. 	
        
	return temp;	
}

void setFanLevel(char * level)
{
FILE *fp; 
	fp = fopen("/proc/acpi/ibm/fan","w");
	 if(fp == NULL)
         {
               syslog(LOG_ERR, "could not set fan level. Is the ThinkPad ACPI Extras Driver installed?");
               exit(-1);
         }

        int ret = fprintf(fp,"level %s\n",level);  
	
        fclose(fp); /*done!*/ 
	
 


}

int main(int argc, char **argv)
{
 start_daemon("Thinkpad SL510 Thermal", LOG_USER);
 
 char * automode = "auto";
 char * level0 = "0";
 char * disengaged = "disengaged";
 int cooling = 0;
 setFanLevel(level0);
  
 syslog(LOG_WARNING, "succsessfully initalized Thinkpad SL510 Thermal");
 
  while(1)
  {	
    sleep(5); //sleep 5 seconds
    int temp = getCPUTemp();
    

	if(temp >= 49 && cooling == 0)
	{
	setFanLevel(automode);
	syslog(LOG_WARNING, "Temperature too high. Activating fan"); 
	cooling = 1;
	}
	else
	{
        	if(temp <= 43 && cooling == 1) //using hysteresis to avoid ping-pong effects
		{
		setFanLevel(level0);
		syslog(LOG_WARNING, "Temperature is okay. Deactivating fan"); 
		cooling = 0;
 		}
	}


  }
}
