#ifndef SHELL_H
#define SHELL_H
#include "globalheader.h"
int prompt(); // prints prompt according to pwd
int getinput(); //formats the command entered by the user, passing it off to the command handler
int main(); //keeps calling prompt() and getinput())
#endif