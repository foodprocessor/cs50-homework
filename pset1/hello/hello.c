#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // get user's name
    string userName = get_string("What's your name?\n");
    // tell the world that I am a bot
    printf("hello, %s\n", userName);
}