#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

// structure for message queue
struct msg_student {
	long id;
	int marks;
} msg;


int main()
{
	key_t key;
	int msgid;

	// ftok to generate unique key
	key = ftok("CI", 65);

	// msgget creates a message queue
	// and returns identifier
	msgid = msgget(key, 0666);

    int i = 0;
    while(i<4)
    {
        int pid = fork();
        if(!pid)
        {
            // student i+1
            msgrcv(msgid, &msg, sizeof(msg), i+1, 0);
            printf("Student %d received: %d\n", i+1, msg.marks);
            return 0;
        }
        i++;
    }

    // student 5
    msgrcv(msgid, &msg, sizeof(msg), i+1, 0);
    printf("Student %d received: %d\n", i+1, msg.marks);

    return 0;
}