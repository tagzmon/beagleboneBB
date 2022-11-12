/** Simple I2C example to read the first address of a device in C
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and 
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2014
* ISBN 9781118935125. Please see the file README.md in the repository root 
* directory for copyright and GNU GPLv3 license information.            */




#include<unistd.h>       //unistd.h is the name of the header file that provides access to the POSIX operating system API
//using read() and write()
#include<stdio.h>        //C library to perform Input/Output operations
#include<fcntl.h>        //file control options. O_RDWR defines value. (OPEN for READ WRITE)
#include<sys/ioctl.h>    //system (linux) io control
#include<linux/i2c.h>    //
#include<linux/i2c-dev.h>//called by the kernel doc used for "I2C_SLAVE" definition. 
//See kernel.org for more information.
//The default search path for include files is /usr/include
//Note it has to be known WHERE these headers live. 
// Example on my beaglebone ioctl.h lives in <sys/ioctl.h> or /usr/inclue/sys/ioctl.handing
// on my ubunutu emulator that is not the case as it lives in <linux/ioctl.h> so that is something of note.


// Small macro to display value in hexadecimal with 2 places
#define DEVID       0x00
#define BUFFER_SIZE 40

int main(){
   int file;
   printf("Starting the ADXL345 test application\n");
   //--------------------------------------------------------------------------------
   // an alternative would be:
   //    char filename[20];
   //    snprintf(filename, 19, "/dev/i2c-d%2", adapter_nr); //not sure why its that specific size but...
   //    file = open(filename, ORWR);  //same data type Though i can't find the following ORWR....
   //                                  //its possible that its supposed to be O_RDWR...
   //--------------------------------------------------------------------------------
   
   //--------------------------------------------------------------------------------
   // open, openat, creat - open and possibly create a file
   // open function defined by ubuntu manual pages
   // manual documentation : manpages.ubuntu.com/manpages/jammy/man2/open.2
   // currently found though:  manpages.ubuntu.com -> Browse the repository -> jammy -> man2 -> open.2
   // jammy used only because as of writing its current LSF.
   // Definition (there are multiple overloads so only 2 will be shown in this doc): 
   //    int open(const char *pathname, int flags);
   //    int open(const char *pathname, int flags, mode_t mode); //only shown for reference
   //
   //    Given  a  pathname  for  a  file,  open()  returns a file descriptor, a small, nonnegative
   //        integer for use in subsequent system calls (read(2), write(2), lseek(2), fcntl(2),  etc.).
   //        The  file  descriptor  returned  by  a  successful  call  will be the lowest-numbered file
   //        descriptor not currently open for the process.
   //    The  argument flags must include one of the following access modes: O_RDONLY, O_WRONLY, or
   //        O_RDWR.   These  request  opening  the  file   read-only,   write-only,   or   read/write,
   //        respectively.
   //    (the manual goes over multiple overloads functions for open, flags, and various other information 
   //        that is ommited to keep this document with brevity)
   //--------------------------------------------------------------------------------
   if((file=open("/dev/i2c-2", O_RDWR)) < 0){
      perror("failed to open the bus\n");
      return 1;
   }
   //--------------------------------------------------------------------------------
   // ioctl - control device
   // ioctl function defined by ubuntu manual pages
   // manual documentation : manpages.ubuntu.com/manpages/jammy/man2/ioctl.2
   // currently found though:  manpages.ubuntu.com -> Browse the repository -> jammy -> man2 -> ioctl.2
   // jammy used only because as of writing its current LSF.
   // Definition: 
   // int ioctl(int fd, unsigned long request, ...);
   //     The  ioctl()  function  manipulates the underlying device parameters of special files.  In
   //         particular, many operating characteristics of character special  files  (e.g.,  terminals)
   //         may be controlled with ioctl() requests.  The argument fd must be an open file descriptor.
   //     The  second argument is a device-dependent request code.  
   //     The third argument is an untyped pointer to memory.  It's traditionally 
   //         char *argp (from the days before void*  was  valid in C), 
   //         and will be so named for in Errors section (not in this file but in manual docs)
   //--------------------------------------------------------------------------------
   if(ioctl(file, I2C_SLAVE, 0x53) < 0){
      perror("Failed to connect to the sensor\n");
      return 1;
   }
   //--------------------------------------------------------------------------------
   // write - write to a file descriptor
   // write function defined by ubuntu manual pages
   // manual documentation : manpages.ubuntu.com/manpages/jammy/man2/write.2
   // currently found though:  manpages.ubuntu.com -> Browse the repository -> jammy -> man2 -> write.2
   // jammy used only because as of writing its current LSF.
   // Definition: 
   // ssize_t write(int fd, const void *buf, size_t count);
   //     writes  up to count bytes from the buffer starting at buf (here as a const void*)
   //         to the file referred to  by the file descriptor fd.
   //     The number of bytes written may be less than count if, for example, there is  insufficient
   //         space on the underlying physical medium, or the RLIMIT_FSIZE resource limit is encountered
   //         (see setrlimit(2)), or the call was interrupted by a signal handler after  having  written
   //         less than count bytes.
   //--------------------------------------------------------------------------------
   char writeBuffer[1] = {0x00};
   if(write(file, writeBuffer, 1)!=1){
      perror("Failed to reset the read address\n");
      return 1;
   }
   //--------------------------------------------------------------------------------
   // read - read from a file descriptor
   // read function defined by ubuntu manual pages (little difficult but found on 
   // ubuntu's manual page
   // manual documentation : manpages.ubuntu.com/manpages/jammy/man2/read.2
   // currently found though:  manpages.ubuntu.com -> Browse the repository -> jammy -> man2 -> read.2
   // jammy used only because as of writing its current LSF.
   // Definition: 
   // ssize_t read(int fd, void *buf, size_t count);
   //     read() attempts to read up to count bytes from file descriptor fd into the buffer starting
   //         at buf.
   //     If count is zero, read() may detect the errors described below.  In  the  absence  of  any
   //         errors,  or  if  read() does not check for errors, a read() with a count of 0 returns zero
   //         and has no other effects. If count is greater than SSIZE_MAX, the result is unspecified.
   //--------------------------------------------------------------------------------
   char readBuffer[BUFFER_SIZE];
   if(read(file, readBuffer, BUFFER_SIZE)!=BUFFER_SIZE){
      perror("Failed to read in the buffer\n");
      return 1;
   }
   printf("The Device ID is: 0x%02x\n", readBuffer[DEVID]);
   close(file);
   return 0;
}


//from kernel.org Documentation:

//#include<linux/i2c-dev.h>
////documentation calls out for us to use /sys/class/i2c-dev or i2cdetect -l to detect device on adapter
//    int main(){
//        int file;
//        int adapter_nr = 2; //this was used as an example. Go check /sys/class/i2c-dev or i2cdetect -labs
//        char filename[20];
//        
//        snprintf(filename, 19, "/dev/i2c-d%2", adapter_nr);
//        file = open(filename, ORWR);
//        if(file<0){
//            //error occured
//            exit();
//        }
//        //now we have passed this stage we can move forward to which device address
//        int addr = 0x40; //this was used as an example
//        if(ioctrl(file,I2C_SLAVE, addr) < 0 ){ 
//                           //ioctl(file, I2C_SLAVE, long addr)
//                           //Change slave address. The address is 
//                           //passed in the 7 lower bits of the argument
//                           //(except for 10 bit addresses, passed in the 10 lower bits in this case).
//            //Error handing
//            exit();
//        }
//        //Using I2C Write, equivalent of
//        //i2c_smbus_write_word_data(file, reg, 0x6543)
//        
//        buf[0] = reg;
//        buf[1] = 0x43;
//        buf[2] = 0x65;
//        if (write(file, buf, 3) != 3) {
//          // ERROR HANDLING: I2C transaction failed
//        }
//        
//        /* Using I2C Read, equivalent of i2c_smbus_read_byte(file) */
//        if (read(file, buf, 1) != 1) {
//          // ERROR HANDLING: I2C transaction failed 
//        } else {
//          // buf[0] contains the read byte
//        }
//        
//    }
















