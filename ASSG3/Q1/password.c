#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define LEN 10

typedef struct password
{
	char password[LEN];
	int a, s, strength;
} pass;

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

	// ftok to generate unique key
	key_t key = ftok("password", 0);

	// shmget returns an identifier in shmid
	int shmid = shmget(key, sizeof(pass), 0666 | IPC_CREAT);

	// create 3 semaphores
	sem_t *sem1 = sem_open("semaphore1",O_CREAT,0660,0);
	sem_t *sem2 = sem_open("semaphore2",O_CREAT,0660,0);
	sem_t *sem3 = sem_open("semaphore3",O_CREAT,0660,0);

	// shmat to attach to shared memory
	pass *ps = (pass *)shmat(shmid, (void *)0, 0);
	// initialise ps
	for (int i = 0; i < LEN; i++)
		ps->password[i] = 0;
	ps->strength = 0;
	ps->a = 0;
	ps->s = 0;
	int i = 0;

	int pid = fork();
	if (!pid)
	{
		int pid2 = fork();
		if (!pid2)
		{
			// P3 - calculate strength
			signal(SIGQUIT, exit);
			while (ps->a + ps->s < LEN)
			{
				sem_wait(sem3);
				if (ps->a >= ps->s)
					ps->strength = 0;
				else
					ps->strength = 1;
				sem_post(sem1);
			}
		}
		else
		{
			// P2 - count chars
			while (i < LEN && ps->password[i] != '\n')
			{
				sem_wait(sem2);
				if ((ps->password[i] >= 'A' && ps->password[i] <= 'Z') || (ps->password[i] >= 'a' && ps->password[i] <= 'z') || (ps->password[i] >= '0' && ps->password[i] <= '9'))
					ps->a++;
				else
					ps->s++;
				i++;
				sem_post(sem3);
			}
			kill(pid2, SIGQUIT);
		}
	}
	else
	{
		// P1 - read password and print strength
		char s[2][10] = {"WEAK", "STRONG"};
		while (i < LEN && (ps->password[i++] = getchar()) != '\n')
		{
			sem_post(sem2);
			sem_wait(sem1);
			printf("\r%s        %s   ", ps->password, s[ps->strength]);
		}
	}

	// close semaphores
	sem_close(sem1);
	sem_close(sem2);
	sem_close(sem3);
	sem_unlink("semaphore1");
	sem_unlink("semaphore2");
	sem_unlink("semaphore3");

	// detach from shared memory
	shmdt(ps);
	shmctl(shmid, IPC_RMID, NULL);

	/*restore the old settings*/
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}
