#pragma once
using namespace std;
//Function to implement automatic offline testing and print the accuracy of the tested files


int testing()
{
    //char contacts[1000][50];
	FILE *fptr;
	fptr = fopen("contacts.txt","r");
	if(fptr == NULL)
		printf("File didnot open...\n");
	fscanf_s(fptr,"%d",&digits);
	
	fgets(contacts[0],50,fptr);
	for(int i = 0; i<digits; i++)
	{
		fgets(contacts[i],50,fptr);
		strtok(contacts[i],"\n");
		//printf("%s",contacts[i]);
	}
	fclose(fptr);
	read_codebook();
	int overall_count = 0;
	int count = 0;
	//loop to iterate over digits
	for(int i = 0; i<digits; i++)
	{
		count = 0;
		//loop to iterate over files of each digit
		for(int j = 1; j<=input_files; j++)
		{
			char filename[60];
			sprintf_s(filename,"Data/%s_%d.txt",contacts[i],j);            //"Data/224101010_E_%d_%d.txt",i,j+20
			printf("%s   ",filename);
			generate_observation_sequence(filename);
			long double max = 0;
			int digit;
			//loop to find the model giving the highest probability
			for(int k = 0; k<digits; k++)
			{
				read_model(k);
				long double probability = forward_procedure();
				if(max < probability)
				{
					max = probability;
					digit = k;
				}
			}
			if(digit == i)
			{
				count++;
				overall_count++;
			}
			printf("Predicted as : %s\n",contacts[digit]);
		}
		printf("Accuracy of digit %d : %.2Lf\n\n\n",i,(((double)count)/input_files)*100.0);
	}
	printf("\nOverall accuracy : %.2Lf\n",(((double)overall_count)/(input_files*digits))*100.0);
	return 0;
}

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
int trim_data(char *filename)
{
	FILE *fptr;
	fptr = fopen(filename,"r");
	if(fptr == NULL)
		printf("File didnot open...\n");
	int n = 0;
	long double a;
	while(fscanf_s(fptr,"%Lf",&a) != EOF)
	{
		data[++n] = a;
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
	for(int i = s; i>0; i--)
	{
		if(frame_energy[i] < 8*threshold)
		{
			l = i*320 + 1;
			break;
		}
	}
	for(int i = s; i<=f; i++)
	{
		if(frame_energy[i] < 8*threshold)
		{
			r = (i-1)*320 + 1;
			break;
		}
	}
	fptr = fopen(filename,"w");
	//loop to print the extracted data in text file
	for(int i = l; i<= r; i++)
		fprintf_s(fptr,"%Lf\n",data[i]);
	fclose(fptr);
	fflush(fptr);
	return 0;
}

int record_newContact()
{
	char name[50];
	FILE *fptr;
	fptr = fopen("new_contact_added.txt","r");
	if(fptr == NULL)
		printf("File didnot open...\n");
	fgets(name,50,fptr);
	fclose(fptr);
	char filename[50];
	int choice;
	char recording_module_path[] = "Recording_Module\\Recording_Module.exe";
	char recording_command[200];
	for(int i = 1; i<=input_files; i++)
	{
		system("cls");
		printf("\n\nRecording instance %d.\n",i);
		sprintf_s(filename,"Data\\%s_%d.txt",name,i);
		sprintf_s(recording_command,"%s 3 %s %s",recording_module_path,"Data\\output.wav",filename);
		do
		{
			system(recording_command);
			trim_data(filename);
			printf("\n Playing Sound: "); 
			PlaySound(TEXT("Data\\output.wav"), NULL, SND_SYNC );	//fucntion to play the recorded sound
			printf("\n\nPress 1 to re-record\n");
			printf("Press 2 to continue\n");
			scanf_s("%d",&choice);
		}while(choice == 1);
		printf("Files recorded successfully....\n");
	}
	return 0;
}

int live_testing()
{
	//stesting();
	read_codebook();
	int choice = 1;
	FILE *fptr;
	fptr = fopen("contacts.txt","r");
	if(fptr == NULL)
		printf("File didnot open...\n");
	fscanf_s(fptr,"%d",&digits);
	
	fgets(contacts[0],50,fptr);
	for(int i = 0; i<digits; i++)
	{
		fgets(contacts[i],50,fptr);
		strtok(contacts[i],"\n");
		//printf("%s",contacts[i]);
	}
	fclose(fptr);
	char wav_filename[] = "live_test_data\\output.wav";
	char txt_filename[] = "live_test_data\\output.txt";
	char recording_module_path[] = "Recording_Module\\Recording_Module.exe";
	char recording_command[200];
	sprintf_s(recording_command,"%s 3 %s %s",recording_module_path,wav_filename,txt_filename);
	//do
	//{
		system(recording_command);	//function to execute the live recording command
		printf("\n Playing Sound: "); 
		PlaySound(TEXT("live_test_data\\output.wav"), NULL, SND_SYNC );	//fucntion to play the recorded sound
		//printf("\n\nPress 1 to re-record\n");
		//printf("Press 2 to continue\n");
		//scanf("%d",&choice);
		//system("cls");
	//}while(choice == 1);
	trim_data("live_test_data\\output.txt");
	generate_observation_sequence(txt_filename);
	long double max = 0;
	int digit;
	//loop to find the model which gives highest probability
	for(int k = 0; k<digits; k++)
	{
		read_model(k);
		long double probability = forward_procedure();
		if(max < probability)
		{
			max = probability;
			digit = k;
		}
	}
	fptr = fopen("new_contact_added.txt","w");
	if(fptr == NULL)
		printf("File didnot open...\n");
	fprintf_s(fptr,"%s",contacts[digit]);
	fclose(fptr);
	printf("\n\nPredicted contact : %s\n",contacts[digit]);
	return digit;
}