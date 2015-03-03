#define _POSIX_C_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <netinet/in.h>

// Length of the buffer in seconds
#define BUF_SEC 2


// Convenient macros for system calls

#define MY_READ(FILE,ADDR,SIZE)                     \
    {                                               \
        ret = fread (ADDR, 1, SIZE, FILE);          \
    }

#define MY_IOCTL(FD,ID,ARG)                     \
    {                                           \
        ret = ioctl (FD, ID, ARG);              \
        if (ret == -1)                          \
        {                                       \
            perror ("ioctl");                   \
            return 2;                           \
        }                                       \
    }

// Std ints
#include <stdint.h>


/*****
 * Main function
 *****/
int main(int argc, char ** argv)
{
  // Args
  if (argc != 2)
    {
      fprintf (stderr, "Wrong number of args.\n");
      return 1;
    }

  /*****
   * Step 1: opening the music file
   *****/

  // File name
  char const * file_name = argv[1];
  printf ("File name: '%s'.\n", file_name);

  
  // Open file
  FILE * audio_file_fd = fopen (file_name, "r");
  if (audio_file_fd == NULL)
  {
      fprintf (stderr, "Couldn't open the file!\n");
      return 2;
  }

  // Look for magic number to determine file type
  int ret;
  int magic_number;
  MY_READ (audio_file_fd, & magic_number, sizeof(magic_number));
  magic_number = ntohl (magic_number);

  if (magic_number != 0x2e736e64)
    {
      
      fprintf (stderr, "File format not recognized.\n");
      return 2;
    }
  printf("File supposedly an AU file.\n");
 
  // Let's read the header file
  // TO COMPLETE !!!

  /*****
   * Step 2: open and configure the dsp file
   *****/

  // Sound dev descriptor
  int fd_dsp;

  // Open sound device
  fd_dsp = open ("/dev/dsp", O_WRONLY);
  if (fd_dsp == -1)
  {
      perror("open: /dev/dsp");
      return 2;
  }
  
  // Let's configure the sound device
  // TO COMPLETE !!!

  // Don't forget to compute the number of octet to play one 1sec of music
  unsigned oct_per_sec;

  /*****
   * Step 3: let's rock baby !!!
   *****/

  // Alloc the buffer used to play
  unsigned int buf_size = BUF_SEC * oct_per_sec;
  unsigned char * buf = malloc (buf_size);

  if (! buf)
  {
      fprintf (stderr, "Couldn't allocate the buffer to play the file.\n");
      return 2;
  }

  // Finally, we play the file
  // TO COMPLETE !!!

  // Close the open files
  close (fd_dsp);
  fclose (audio_file_fd);

  // Free the memory
  free(buf);


  return 0;
}
