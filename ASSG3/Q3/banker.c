// Banker's Algorithm
#include <stdio.h>

int print_safe_sequence(int[]);
int is_safe(int[], int[]);
int can_grant(int, int[], int[]);

int n, m;
int alloc[100][100], max[100][100], need[100][100];

int main()
{
    printf("No. of processes: ");
	scanf("%d", &n); // Number of processes
    printf("No. of resources: ");
	scanf("%d", &m); // Number of resources

    int avail[m], req[m], seq[n];

    // take inputs
    printf("AllocTable:\n");
    for(int i=0; i<n; i++)
        for(int j=0; j<m; j++)
            scanf("%d", &alloc[i][j]);

    printf("MaxTable:\n");
    for(int i=0; i<n; i++)
        for(int j=0; j<m; j++)
            scanf("%d", &max[i][j]);

    printf("Total instances per resource type:\n");
    for(int i=0; i<m; i++)
        scanf("%d", &avail[i]);

    // calculate available resources
    for(int i=0; i<n; i++)
        for(int j=0; j<m; j++)
            avail[j] -= alloc[i][j];

    // calculate need matrix
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			need[i][j] = max[i][j] - alloc[i][j];

    char x;

    printf("\na. Print SAFE STATE sequence.\nb. Determine if sequence is SAFE.\nc. Check if request can be granted.\nq. Quit\n");
	do
    {
        scanf("%c", &x);
        switch(x)
        {
            case 'a':
                print_safe_sequence(avail) ? : printf("No safe sequence.\n");
                break;
            case 'b':
                printf("Sequence: ");
                for(int i=0; i<n; i++)
                    scanf("%d", &seq[i]);
                is_safe(avail, seq) ? printf("SAFE\n") : printf("UNSAFE\n");
                break;
            case 'c':
                int p;
                printf("Process no: ");
                scanf("%d",&p);
                printf("Request vector: ");
                for(int i=0; i<m; i++)
                    scanf("%d", &req[i]);
                can_grant(p, req, avail) ? printf("Request can be granted.\n") : printf("Request cannot be granted.\n");
        }
    } while(x != 'q');

	return (0);
}

int print_safe_sequence(int avail[])
{
    // banker's algorithm
    int f[n], seq[n], c = 0;
	for (int k = 0; k < n; k++)
		f[k] = 0;
    int temp[m];
    for (int i = 0; i < m ; i++)
        temp[i] = avail[i];
	while (c<n)
    {
        int flag = 0;
		for (int i = 0; i < n; i++)
        {
			if (!f[i])
            {
                int j;
				for (j = 0; j < m; j++)
					if (need[i][j] > temp[j])
						break;

				if (j == m)
                {
					seq[c++] = i;
					for (int y = 0; y < m; y++)
						temp[y] += alloc[i][y];
					f[i] = 1;
                    flag = 1;
				}
			}
		}
        if(!flag)
            return 0;
	}

	printf("SAFE Sequence:\n");
	for (int i=0; i<n; i++)
		printf("P%d -> ", seq[i]);
	printf("\b\b\b   \n");
    return 1;
}

int is_safe(int avail[], int seq[])
{
    int temp[m];
    for (int i = 0; i < m ; i++)
        temp[i] = avail[i];
    for (int i = 0; i<n ; i++)
    {
        int p = seq[i];
        for (int j = 0; j < m; j++)
            if (need[p][j] > temp[j])
                return 0;
        for (int y = 0; y < m; y++)
            temp[y] += alloc[p][y];
    }
    return 1;
}

int can_grant(int p, int req[], int avail[])
{
    // resource-request algorithm
    for (int j = 0; j < m; j++)
        if (req[j] > need[p][j] || req[j] > avail[j])
            return 0;
    for (int j = 0; j < m ; j++)
    {
        avail[j] -= req[j];
        alloc[p][j] += req[j];
        need[p][j] -= req[j];
    }
    if(!print_safe_sequence(avail))
    {
        // unsafe => revert changes
        for (int j = 0; j < m ; j++)
        {
            avail[j] += req[j];
            alloc[p][j] -= req[j];
            need[p][j] += req[j];
        }
        return 0;
    }
    return 1;
}