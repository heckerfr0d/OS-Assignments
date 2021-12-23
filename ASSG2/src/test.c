#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define BUF_LEN 80 // The buffer length

int main()
{
   static struct termios oldt, newt;

   /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
   tcgetattr(STDIN_FILENO, &oldt);
   /*now the settings will be copied*/
   newt = oldt;

   /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
   newt.c_lflag &= ~(ICANON);

   /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
   tcsetattr(STDIN_FILENO, TCSANOW, &newt);
   int ret, fd;
   fd = open("/dev/chardev", O_RDWR); // Open the device file with read/write access
   if (fd < 0)
   {
      perror("Failed to open the device file...");
      return errno;
   }
   printf(
       "Enter message to send to the character device:\n"
       "A string is sent to the device on ENTER or when buffer is full.\n"
       "A read is triggered on '$' key.\n"
       "Reading from the device also clears its kernel buffer\n"
       "You can close the application using keyboard interrupt (ctrl+C).\n");
   while (1)
   {
      char str[BUF_LEN] = {0};
      int len = 0;
      printf("Message to send to the character device:\n");
      int i = 0;
      do
      {
         if (str[i - 1] == '\n')
         {
            ret = write(fd, str, strlen(str)); // Send the string to the device
            if (ret < 0)
            {
               perror("Failed to write the message to the device.");
               return errno;
            }
            for (int i = 0; i < len; i++)
               str[i] = 0;
            i = 0;
         }
         else if (str[i - 1] == '$')
            break;
         printf("\r[%d chars left]: %s", BUF_LEN - len, str);
         str[i] = 0;
         len++;
      } while ((len < BUF_LEN) && (str[i++] = getchar()));

      if (str[i - 1] != '$')
      {
         printf("\nBuffer full...\nPress any key to read back from the device...");
         getchar();
      }
      else
         str[i - 1] = 0;

      ret = write(fd, str, strlen(str)); // Send the string to the device
      if (ret < 0)
      {
         perror("Failed to write the message to the device.");
         return errno;
      }

      printf("\b \nReading from the device...\n");
      char receive[BUF_LEN] = {0};      // The buffer to receive from the device
      ret = read(fd, receive, BUF_LEN); // Read from the character device
      if (ret < 0)
      {
         perror("Failed to read from the device.");
         return errno;
      }
      printf("The received message is: \n%s\n", receive);
   }
   close(fd);
   /*restore the old settings*/
   tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

   return 0;
}