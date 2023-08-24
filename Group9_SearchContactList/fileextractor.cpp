#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>

int data[100000];
//function to find the short term energy of the given frame
long double short_term_energy(int l, int r)
{
	long double energy = 0;
	for(int i = l; i<=r; i++)
	{
		energy += data[i]*data[i];
	}
	return energy/(r-l+1);
}

//function to trim the live testing data and extract the useful information
int trim_data(char *filename, char *filename2)
{
	FILE *fptr;
	fptr = fopen(filename,"r");
	printf("%s\n",filename);
	if(fptr == NULL)
		printf("File didnot open...\n");
	int n = 0;
	int a;
	while(fscanf(fptr,"%d",&a) != EOF)
	{
		data[++n] = a;
		//printf("%d\n",data[n]);
	}
	fclose(fptr);
	fflush(fptr);
	long double threshold = short_term_energy(n-3*320,n-2*320);	//function call to find the threshold energy of the silence part
	int l = 0,r = n,s,f = 0;
	long double max = 0;
	long double frame_energy[1000];
	//loop to find the frame with largest short term energy
	//We move left of this frame to find the starting point of useful data and move right to find the ending point of useful data
	for(int i = 1; i+320<=n; i += 320)
	{
		frame_energy[++f] = short_term_energy(i,i+319);
		if(max < frame_energy[f])
		{
			max = frame_energy[f];
			s = f;
		}
	}
	printf("%d\n",s);
	for(int i = s; i>0; i--)
	{
		if(frame_energy[i] < 8*threshold)
		{
			l = i*320 + 1;
			break;
		}
	}
	printf("data read\n");
	for(int i = s; i<=f; i++)
	{
		if(frame_energy[i] < 8*threshold)
		{
			r = (i-1)*320 + 1;
			break;
		}
	}
	fptr = fopen(filename2,"w");
	//loop to print the extracted data in text file
	for(int i = l; i<= r; i++)
		fprintf(fptr,"%d\n",data[i]);
	fclose(fptr);
	fflush(fptr);
	return 0;
}


int main()
{
	
	FILE *fin = NULL;
	FILE *fout = NULL;
	char temp[10][100];
	int digit;
    char contacts[1000][50];
	FILE *fptr;
	fptr = fopen("contacts.txt","r");
	fscanf(fptr,"%d",&digit);
	int i,j,k;
	fgets(contacts[0],50,fptr);
	for(i = 0; i<digit; i++)
	{
		fgets(contacts[i],50,fptr);
		strtok(contacts[i],"\n");
		//printf("%s",contacts[i]);
	}
	fclose(fptr);
	for(i = 0; i<10; i++)
	{
		for(j = 1; j<=23; j++)
		{
			int a;
			char filename[100];
			char filename2[100];
			//sprintf(filename,"Data3/224101057_%s_%d.txt",contacts[i],j);
			//sprintf(filename2,"Data/%s_%d.txt",contacts[i],j);
			trim_data(filename,filename2);
		}
	}
	
	return 0;
}
