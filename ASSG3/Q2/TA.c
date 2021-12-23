#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

// structure for message queue
struct msg_student {
	long id;
	int marks;
} msg;

struct msg_TA {
    long msg_type;
    float avg;
    char grade[5];
} TA;

int main()
{
	key_t key;
	int msgid;

	// ftok to generate unique key
	key = ftok("CI", 65);

	// msgget creates a message queue
	// and returns identifier
	msgid = msgget(key, 0666);

    for(int i = 0; i < 5; i++)
    {
        // receive from CI
        msgrcv(msgid, &msg, sizeof(msg), i+1, 0);
        TA.avg += msg.marks;
        TA.grade[i] = msg.marks >= 40 ? 'S' : msg.marks >= 30 ? 'A' : msg.marks >= 20 ? 'B' : 'E';
        printf("Student %d: %d - %c\n", msg.id, msg.marks, TA.grade[i]);
    }
    TA.avg /= 5;
    printf("Average  : %.2f\n", TA.avg);
    TA.msg_type = 100;
    // send to CI
    msgsnd(msgid, &TA, sizeof(TA), 0);

    return 0;
}
