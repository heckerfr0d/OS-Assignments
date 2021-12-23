#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

// structures for message queue
struct msg_student {
	long id;
	int marks;
} student;

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
	msgid = msgget(key, 0666 | IPC_CREAT);


    // CI
    for (int i=0; i<5; i++)
    {
        printf("Student %d marks: ", i+1);
        scanf("%d", &student.marks);
        student.id = i+1;
        // send twice to ensure that both the student and the TA gets the message
        msgsnd(msgid, &student, sizeof(student), 0);
        msgsnd(msgid, &student, sizeof(student), 0);
    }
    // receive result from TA
    msgrcv(msgid, &TA, sizeof(TA), 100, 0);

    printf("\n+----------+-------+\n");
    printf("| Student  | Grade |\n");
    printf("+----------+-------+\n");
    for(int i = 0; i < 5; i++)
        printf("|   %d      |  %c    |\n", i+1, TA.grade[i]);
    printf("+----------+-------+\n");
    printf("| Average  | %.2f |\n", TA.avg);
    printf("+----------+-------+\n");

    // to destroy the message queue
	msgctl(msgid, IPC_RMID, NULL);

	return 0;
}
