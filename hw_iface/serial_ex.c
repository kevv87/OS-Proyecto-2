#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

/*
 * Example taken from: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 * */

int set_interface_attribs(int fd, int speed){
    struct termios tty;
    if(tcgetattr(fd, &tty) < 0){
        printf("ERror from tcgetattr: %s\n", strerror(errno));
    }

    cfsetospeed(&tty, (speed_t) speed);
    cfsetispeed(&tty, (speed_t) speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8 bit characters
    tty.c_cflag &= ~PARENB; // No parity bit
    tty.c_cflag &= ~CSTOPB; // Only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS; // No hw flowcontrol

    // Non canonical mode
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    // fetch bytes as they become available
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if(tcsetattr(fd, TCSANOW, &tty) != 0){
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount){
    struct termios tty;
    if(tcgetattr(fd, &tty) < 0){
        printf("Error fromm tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5; // Half second timer

    if(tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

int main(){
    return 0;
}