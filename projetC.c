#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/shm.h>
#include <math.h>
#include <sys/sem.h>



typedef int (*compfn)(const void*, const void*);

struct car { 
     int number; 
     float position;
     float lap[100];

     float sector1[100];
     float sector2[100];
     float sector3[100];

     float total_time;
     float best_lap;
	 
     float best_sector1;
     float best_sector2;
     float best_sector3;

     int crash;
     int finish;

	 float last_sector1;
	 float last_sector2;
	 float last_sector3;
	 float last_lap;
};
struct car copie[20];




float racing_laps(int circuitSize){
     int racing_laps=round(330.0/circuitSize);
     return racing_laps;
}


// Partie gérant la comparation les différents secteurs des différentes cars
int sector1_comparator(struct car *x, struct car *y) {
	float time1 = x->best_sector1;
	float time2 = y->best_sector1;
	if (time1 < time2) return -1;
	else if (time1 > time2) return 1; return 0;
}

int sector2_comparator(struct car *x, struct car *y) {
	float time1 = x->best_sector2;
	float time2 = y->best_sector2;
	if (time1 < time2) return -1;
	else if (time1 > time2) return 1; return 0;
}

int sector3_comparator(struct car *x, struct car *y) {
	float time1 = x->best_sector3;
	float time2 = y->best_sector3;
	if (time1 < time2) return -1;
	else if (time1 > time2) return 1; return 0;
}

int compare_function(struct car *x, struct car *y) {
	float time1 = x->best_lap;
	float time2 = y->best_lap;
	if (time1 < time2) return -1;
	else if (time1 > time2) return 1; return 0;
}

int race_comparator(struct car *x, struct car *y) {
	float time1 = x->total_time;
	float time2 = y->total_time;
	if (time1 < time2) return -1;
	else if (time1 > time2) return 1; return 0;
}
//////////////////////////////////////////////////////////////////////////////



void * display_tests(int number_cars, struct car* array){
	system("clear");
	float ecart = 0.0;
	printf("N°\tDiff\t\tTpsSect1\tTpsSect2\tTpsSect3\tMeilleur tour\n");
		printf("==========================================================================================\n");

	qsort(array,number_cars,sizeof(struct car), (compfn)compare_function);
		for(int i=0;i<number_cars; i++){
			printf("%d", array[i].number);
			if (i == 0){
				printf("\tPremier\t");
			}
			else{
				ecart = array[i].best_lap-array[i-1].best_lap;
				printf("\t+%f",ecart);
			}
			printf("\t%f",array[i].best_sector1);
			printf("\t%f",array[i].best_sector2);
			printf("\t%f",array[i].best_sector3);
			if (array[i].best_lap < 200 & array[i].best_lap >74){
				printf("\t%f",array[i].best_lap);
			}
			else{
				printf("\t0.00000");				
			}
			if(array[i].last_lap>= 1000.0) {
				printf("\tOut\n");
			}
			else if(array[i].last_sector3>=1000.0){
				printf("\tS\n");
			}
			else{
				printf("\tIN\n");
			}
		}

}
void * race_display(int number_cars, struct car* array){
	system("clear");
	printf("N°\t\tDiff\t\t\tDernier tour\t\tTps Sect 1\t\tTps Sect 2\t\tTps Sect 3\t\t\n");
	printf("==========================================================================================================================\n");
	int array_out [20]={0};
	int car;
	qsort(array,number_cars,sizeof(struct car), (compfn)race_comparator);
		for(int i=0;i<number_cars; i++){
			if (i==0 && array[i].last_sector3>49.0){
				printf("%d", array[i].number);
				printf("\t\tPremier : Stand\t");
			}
			else if(i==0){
				printf("%d", array[i].number);
				printf("\t\tPremier\t");
			}
			else if(array[i].total_time>=9999.9){
				printf("%d", array[i].number);
				printf("\t\tOut\t");
			}
			else if (array[i].last_sector3>49.0){
				printf("%d", array[i].number);
				printf("\t\tStand\t");
			}
			else{
				float ecart =  array[i].total_time - array[i-1].total_time;
				printf("%d", array[i].number);
				printf("\t\t+%f", ecart);
			}
			printf("\t\t%f",array[i].last_lap);
			printf("\t\t%f",array[i].last_sector1);
			printf("\t\t%f",array[i].last_sector2);
			printf("\t\t%f\n",array[i].last_sector3);
			if (i == 19){

				qsort(array,number_cars,sizeof(struct car), (compfn)sector1_comparator);
				printf("Meilleur temps pour secteur 1 : voiture n°%d %f\n", array[0].number, array[0].best_sector1);
				qsort(array,number_cars,sizeof(struct car), (compfn)sector1_comparator);
				printf("Meilleur temps pour secteur 2 : voiture n°%d %f\n", array[0].number, array[0].best_sector2);
				qsort(array,number_cars,sizeof(struct car), (compfn)sector3_comparator);
				printf("Meilleur temps pour secteur 3 : voiture n°%d %f\n", array[0].number, array[0].best_sector3);
				qsort(array,number_cars,sizeof(struct car), (compfn)compare_function);
				printf("Meilleur tour de la course: voiture n°%d %f\n", array[0].number, array[0].best_lap);
			}
	}
}




//Partie de création et d'écriture de fichiers /////////////////////////////////////////////////
void write_external_file_sprint(char* file, int nbr_cars, struct car* array, float* time_value, int part){
    FILE * filedesc = fopen(file, "w");

    for(int i=0;i<nbr_cars;i++){
        if(part==3){
            fprintf(filedesc, "\n=================================================================================================================================\n");
                if(array[i].number < 10){
                    fprintf(filedesc, "|%d", 0);
                    fprintf(filedesc, "%d|", array[i].number);
                }
                else{
                    fprintf(filedesc, "|%d|", array[i].number);
                }
            fprintf(filedesc, "Position %d sur la grille de départ", i+1);
        }
    }
    fclose(filedesc);
}

void write_starting(struct car* array, int begin, int end){
	FILE * filed;
	filed = fopen("grille_depart_course.txt", "a");
	for(int h=begin;h>end;h--){
		if (array[h].number < 10){
			fprintf(filed, "===========================================\n|0%d", array[h].number);
			fprintf(filed, "|Position %d sur la grille\n", h+1);
		}
		else{
			fprintf(filed, "===========================================\n|%d", array[h].number);
			fprintf(filed, "|Position %d sur la grille\n", h+1);
		}
	}
	fclose(filed);
}

void write_starting_sprint(struct car* array, int begin, int end){
    FILE * filed;
    filed = fopen("grille_depart_sprint.txt", "a");
    for(int h=begin;h>end;h--){
        if (array[h].number < 10){
            fprintf(filed, "===========================================\n|0%d", array[h].number);
            fprintf(filed, "|Position %d sur la grille\n", h+1);
        }
        else{
            fprintf(filed, "===========================================\n|%d", array[h].number);
            fprintf(filed, "|Position %d sur la grille\n", h+1);
        }
    }
    fclose(filed);
}

void write_external_file(char* file, int nbr_cars, struct car* array, float* time_value, int part){
	FILE * filedesc = fopen(file, "w");
	fprintf(filedesc, "Meilleurs temps:\n");
	fprintf(filedesc, "\nMeilleur temps pour le secteur 1:%f", time_value[1]);
	fprintf(filedesc, "\nMeilleur temps pour le secteur 2:%f", time_value[2]);
	fprintf(filedesc, "\nMeilleur temps pour le secteur 3:%f\n", time_value[3]);
	fprintf(filedesc, "\nMeilleur temps pour un tour complet:%f", time_value[0]);

	if(part==3){
		fprintf(filedesc, "\nTemps pour chaques voitures:\nN°| MeilTour | MeilS1  |  MeilS2 |  MeilS3 | TpsTot | DiffADern | Crash ");
	}
	else{
		fprintf(filedesc, "\nTemps pour chaques voitures:\nN°| MeilTour | MeilS1  |  MeilS2 |  MeilS3 | DiffADern | Crash");
	}
	for(int i=0;i<nbr_cars;i++){
		if(part==3 & array[i].crash==0){
			fprintf(filedesc, "\n============================================================================\n");
		    	if(array[i].number < 10){
		    		fprintf(filedesc, "%d", 0);
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
		    	else{
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
			fprintf(filedesc, "%f|", array[i].best_lap);
			fprintf(filedesc, "%f|", array[i].best_sector1);
			fprintf(filedesc, "%f|", array[i].best_sector2);
			fprintf(filedesc, "%f|", array[i].best_sector3);
			fprintf(filedesc, "%f|", array[i].total_time);
			if(i>0){
				fprintf(filedesc, "%f", (array[i].total_time - array[i-1].total_time));
			}
				else{
				fprintf(filedesc, "%f", 00.00);
			}
			fprintf(filedesc, "| Non");
		}
		else if(part!=3){
			fprintf(filedesc, "\n==========================================================================================\n");
		    	if(array[i].number < 10){
		    		fprintf(filedesc, "%d", 0);
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
		    	else{
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
			fprintf(filedesc, "%f|", array[i].best_lap);
			fprintf(filedesc, "%f|", array[i].best_sector1);
			fprintf(filedesc, "%f|", array[i].best_sector2);
			fprintf(filedesc, "%f|", array[i].best_sector3);
			if(i>0){
				fprintf(filedesc, "%f", (array[i].best_lap - array[i-1].best_lap));
			}
			else{
				fprintf(filedesc, "%f", 00.00);
			}
			if(array[i].crash){
				fprintf(filedesc, "| Oui");
			}
			else{
				fprintf(filedesc, "| Non");
			}
		}
	}
	for(int i=0;i<nbr_cars;i++){
		if(part==3 & array[i].crash==1){
		    	fprintf(filedesc, "\n=================================================================================================================================\n");
		    	if(array[i].number < 10){
		    		fprintf(filedesc, "%d", 0);
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
		    	else{
		    		fprintf(filedesc, "%d|", array[i].number);
		    	}
			fprintf(filedesc, "%f|", array[i].best_lap);
			fprintf(filedesc, "%f|", array[i].best_sector1);
			fprintf(filedesc, "%f|", array[i].best_sector2);
			fprintf(filedesc, "%f|", array[i].best_sector3);
			fprintf(filedesc, "\tOut\t|");
			fprintf(filedesc, "--------");
			fprintf(filedesc, "| Oui");
		}
	}
	fclose(filedesc);
}

//////////////////////////////////////////////////////////////////////////////




void cars_qualification_attribute(int i, struct car* array, int* array_number){
	array[i].number=array_number[i];
}

void cars_test_attribute(int i, struct car* array){
	int number[20]={44, 63, 1, 11, 55, 16, 4, 3, 14, 31, 10, 22, 18, 5, 6, 23, 77, 24, 47, 9};
	array[i].number=number[i];
}

void race_car_attribute(int i, struct car* array, int* array_number){
    array[i].number=array_number[i];
}






//Partie avec la génération des temps
void * times_generator(int number_race_laps, struct car* car1){
    float n=0;
    float nombre=0;
    int a=0;
    for (int i = 1; i < 3*number_race_laps+1; i++){
        nombre=rand() % (15)+25;
        n=(float)rand()/RAND_MAX;
        if(i%3==1){
            car1->sector1[a]=n+nombre;
        }
         else if(i%3==2){
            car1->sector2[a]=n+nombre;
        }
         else if(i%3==0){
            car1->sector3[a]=n+nombre;
            car1->lap[a]=car1->sector1[a]+car1->sector2[a]+car1->sector3[a];
            a++;
        }
    }
}

void * time_generator_race(struct car* car1, int racing_laps){

    float n=0;
    float nombre=0;
    int a=0;
    int arret_max = 3;
    car1->crash = 0;
    int number_race_laps = racing_laps;
    car1->finish = 1;
	car1->crash = 0;
	car1->best_lap=3565656.0;
	car1->best_sector1=3565656.0;
	car1->best_sector2=3565656.0;
	car1->best_sector3=3565656.0;


    for (int i = 1; i < 3*number_race_laps+1; i++){
        nombre=rand() % (15)+25;
        n=(float)rand()/RAND_MAX;

		float multiplicateur = 1.0 + ((float)number_race_laps / 10.0);

        if(i%3==1){
            if (i==1){
            	car1->sector1[a]=n+nombre+((car1->position)*0.5) / multiplicateur;
				car1->last_sector1=n+nombre+((car1->position)*0.5) / multiplicateur;
            }
            else{
            	car1->sector1[a]=n+nombre / multiplicateur;
				car1->last_sector1=n+nombre / multiplicateur;
            }
			if(car1->last_sector1 < car1->best_sector1){
				car1->best_sector1=car1->last_sector1;
			}
        }
         else if(i%3==2){
            car1->sector2[a]=n+nombre / multiplicateur;
			car1->last_sector2=n+nombre / multiplicateur;
			if(car1->last_sector2 < car1->best_sector2){
				car1->best_sector2=car1->last_sector2;
			}
        }
         else if(i%3==0){
         	car1->sector3[a]=n+nombre / multiplicateur;
			car1->last_sector3=n+nombre / multiplicateur;
         	int nombre1 = rand() % (10);
        	if(nombre1 == 1 & arret_max>0){
            		float nombre2=rand() % (4)+25;
            		car1->sector3[a] = car1->sector3[a] + nombre2;
					car1->last_sector3 = car1->sector3[a];
           		arret_max--;
        	}
            	car1->lap[a]=car1->sector1[a]+car1->sector2[a]+car1->sector3[a];
				car1->last_lap=car1->sector1[a]+car1->sector2[a]+car1->sector3[a];
            	car1->total_time += car1->lap[a];
			if(car1->last_sector3 < car1->best_sector3){
				car1->best_sector3=car1->last_sector3;
			}
			if(car1->last_lap<car1->best_lap){
				car1->best_lap=car1->last_lap;
			}
            	a++;
        }
        int proba_acc = rand() % (1000);
        if(proba_acc == 1){
            i = 3*number_race_laps+1;
            car1->crash = 1;
			car1->total_time = 10000.0;
        }
		usleep(333333);
    }
    car1->finish = 0;
}

float best_time(float *lap, int number_race_laps){
    float best=lap[0];
    for (int i = 0; i < number_race_laps ; i++){
        if(best>lap[i] & lap[i]!=0.00){
            	best=lap[i];
        }
    }
    
    return best;
}

void best_time_race_attribute(int i, struct car* array, int* array_number, int number_race_laps){
	array[i].best_lap=best_time(array[i].lap, number_race_laps);
    array[i].best_sector1=best_time(array[i].sector1, number_race_laps);
    array[i].best_sector2=best_time(array[i].sector2, number_race_laps);
    array[i].best_sector3=best_time(array[i].sector3, number_race_laps); 
}

float best_time_circuit_test(struct car* array, float* time_value, int numberCars){
	float bestTempsTour = 3565656.0;
	float best_time_s1 = 3565656.0;
	float best_time_s2 = 3565656.0;
	float best_time_s3 = 3565656.0;
	qsort(array,numberCars,sizeof(struct car), (compfn)compare_function);
	for(int i=0;i<numberCars;i++){	
		if (array[i].best_sector1 < best_time_s1){
			best_time_s1 = array[i].best_sector1;	
		}
		if (array[i].best_sector2 < best_time_s2){
			best_time_s2 = array[i].best_sector2;	
		}
		if (array[i].best_sector3 < best_time_s3){
			best_time_s3 = array[i].best_sector3;	
		}
	}
	time_value[0] = array[0].best_lap;
	time_value[1] = best_time_s1;
	time_value[2] = best_time_s2;
	time_value[3] = best_time_s3;
	return *time_value;	
}

int all_finishes_race(struct car* array[20], int nbr_cars){
	int is_true = 0;
	for (int i=0; i<nbr_cars; i++){
		if (array[i]->finish == 1){
			is_true == 1;
		}
	}
	if (is_true){ return 0; }
	else{ return 1; }
}
//////////////////////////////////////////////////////////////////////////////











void *set_position_race(struct car* car1, int i){
	car1->position = (float)i;
}

void * lap(float time_race, struct car* car1, struct car* array, int qpart){
    float tempsTot = 0.0;
    float tempsActuel = 0.0;
    float bestTemps = 3565656.0;
    float bestS1 = 3565656.0;
    float bestS2 = 3565656.0;
    float bestS3 = 3565656.0;
	car1->best_lap = 3565656.0;
    int nombre1 = 0;
    int nombretest = 0;
    nombretest = rand() % (15);
	int temps;
	if (qpart == 1){
		temps = 950000;
	}
	else if(qpart == 2){
		temps = 1100000;
	}
	else if(qpart == 3){
		temps = 1400000;
	}
	else if(qpart == 4){
		temps = 300000;
	}

    while(tempsActuel < time_race && car1->crash == 0){
		car1->last_sector3 = 0.0;
        nombre1 = rand() % (10);
        if(nombre1 == 1 & nombretest>0){
            float nombre2=rand() % (int)(time_race - tempsActuel);
            tempsActuel = tempsActuel + nombre2;
            nombretest--;
			car1->last_sector3 = 9999.99;
        }
        else{
            times_generator(1, car1);
            if(car1->lap[0] <  car1->best_lap ){
                car1->best_lap = (car1->lap[0]);
				car1->best_sector1 = car1->sector1[0];
				car1->best_sector2 = car1->sector2[0];
				car1->best_sector3 = car1->sector3[0];
				car1->best_lap=car1->lap[0];
            }
            tempsActuel = tempsActuel + (car1->lap[0]);
        }
        int proba_acc = rand() % (300);
        if(proba_acc == 1){
            tempsActuel = time_race + time_race;
            car1->crash = 1;
			car1->last_lap = 9999.99;
        }
		usleep(temps);
    }
    srand(time(NULL));
}









//Partie liée aux essais, qualificatins, sprint et course
void circuit_test(float time_test, char* file, int numberCars){
	struct car *array;
	int shmid = shmget(16, 20*sizeof(struct car), IPC_CREAT|0666);
	array = shmat(shmid,0,0);

	int child_process_id = fork();
	if (child_process_id) {
		for(int i=0;i<20;i++){
			if(fork()==0){
				struct car *array;
				array = (struct car*)shmat(shmid,0,0);
				srand(time(NULL) ^ getpid());
				cars_test_attribute(i, array);
				lap(time_test, &array[i], array, 4);
				shmdt(array);
				exit(0);
			}
		}
	}
	else {
		int j = 0;
		while(j<20){
		display_tests(20, array);
		sleep(1);
		j++;
		}
		exit(0);
	}
	for(int i=0;i<21;i++){
		wait(NULL);
	}

	float time_value[4];
	best_time_circuit_test(array, time_value, 20);
	write_external_file(file, 20, array, time_value, 1);
	shmdt(array);
	shmctl(shmid,IPC_RMID,NULL);
}

void qualification(float time_qualif, char* file_to_write, char* file_to_read, int nbr_cars_to_write, int number_of_cars, int qpart){
		int num_list[number_of_cars];
		if(qpart == 2 | qpart == 3){
			int CONTENT_SIZE = 10000;
			char contenu[CONTENT_SIZE];
			const char * separators = "|\n";
			int fd = open(file_to_read, O_RDONLY);
			int size_read = read(fd, contenu, CONTENT_SIZE);
			char * ligne = strtok (contenu, separators);
			int i = 1;
			int pos = 0;
            	while ( ligne != NULL ){
					if((i)%8==7 & (i!=7)){
						int number = atoi(ligne);
						num_list[pos] = number;
						pos++;
            			}
            		ligne = strtok ( NULL, "|\n" );
           			i++;
            		}
            		close(fd);
		}
		struct car *array;
		int shmid = shmget(16, number_of_cars*sizeof(struct car), IPC_CREAT|0666);
		array = shmat(shmid,0,0);
		int child_process_id = fork();
		if (child_process_id) {
			for(int i=0;i<=number_of_cars;i++){
				if(fork()==0){
					struct car *array;
					array = (struct car*)shmat(shmid,0,0);
					srand(time(NULL) ^ getpid());
					if(qpart == 1){
						cars_test_attribute(i, array);
					}
					if(qpart == 2 | qpart == 3){
						cars_qualification_attribute(i, array, num_list);			
					}
					lap(time_qualif, &array[i], array, qpart);
					shmdt(array);
					exit(0);
				}
			}
		}
		else {
			int j = 0;
			if(qpart == 3){
				while(j<7){
					display_tests(number_of_cars, array);
					sleep(1);
					j++;
				}
			}
			else{
				while(j<10){
					display_tests(number_of_cars, array);
					sleep(1);
					j++;
				}
			}

			float time_value[4];
			best_time_circuit_test(array, time_value, number_of_cars);
			write_external_file(file_to_write, nbr_cars_to_write, array, time_value, 2);
			if(qpart == 3){
				write_starting(array, number_of_cars-1, -1);
			}
			else{
				write_starting(array, number_of_cars-1, number_of_cars-6);
			}
			shmdt(array);
			shmctl(shmid,IPC_RMID,NULL);
			}
		for(int i=0;i<50;i++){
			wait(NULL);
		}
}

void race(int circuit_size, char* end_file, char* grid){
	int CONTENT_SIZE = 10000;
	int racing_laps = round(320/(circuit_size));
	char contenu[CONTENT_SIZE];
	const char * separators = "|\n";
	int fd = open(grid, O_RDONLY);
	int size_read = read(fd, contenu, CONTENT_SIZE);
	char * ligne = strtok (contenu, separators);
	int i = 1;
	int pos = 20;
	int num_list[20];
        while ( ligne != NULL ){
		if((i)%3==2){
			int number = atoi(ligne);
			num_list[pos] = number;
			pos--;
            	}
            	ligne = strtok ( NULL, "|\n" );
           	i++;
	}
	close(fd);
	struct car *array;
	int shmid = shmget(16, 20*sizeof(struct car), IPC_CREAT|0666);
	array = shmat(shmid,0,0);
	

	printf("Taille du circuit : %d\n", circuit_size);
	printf("Nombre de tours : %d\n", racing_laps);
	sleep(1.5);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);
	
	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");
	
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");	
	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");
	sleep(1);

	int child_process_id = fork();
	if (child_process_id) {
		for(int i=0;i<20;i++){
			if(fork()==0){
				struct car *array;
				array = (struct car*)shmat(shmid,0,0);
				srand(time(NULL) ^ getpid());
				set_position_race(&array[i], i);
				race_car_attribute(i, array, num_list);
				time_generator_race(&array[i],racing_laps);
				best_time_race_attribute(i, array, num_list, racing_laps);
				shmdt(array);
				exit(0);
			}
		}
		for(int i=0;i<21;i++){
			wait(NULL);
		}
	}
	else{
		int j = 0;
		while(j<55){
			memcpy(copie, array, sizeof(copie) );
			race_display(20, copie);
			sleep(1);
			j++;
		}
		float time_value[4];
		best_time_circuit_test(array, time_value, 20);
		
		memcpy(copie, array, sizeof(copie) );
		qsort( copie,20,sizeof(struct car), (compfn)race_comparator);
		write_external_file(end_file, 20, copie, time_value, 3);
		
		shmdt(array);
		shmctl(shmid,IPC_RMID,NULL);
	}
}



void sprint(int circuit_size, char* end_file, char* grid){
	int CONTENT_SIZE = 10000;
	int racing_laps = round((100/(circuit_size)));
	char contenu[CONTENT_SIZE];
	const char * separators = "|\n";
	int fd = open(grid, O_RDONLY);
	int size_read = read(fd, contenu, CONTENT_SIZE);
	char * ligne = strtok (contenu, separators);
	int i = 1;
	int pos = 20;
	int num_list[20];
        while ( ligne != NULL ){
		if((i)%3==2){
			int number = atoi(ligne);
			num_list[pos] = number;
			pos--;
            	}
            	ligne = strtok ( NULL, "|\n" );
           	i++;
	}
	close(fd);
	struct car *array;
	int shmid = shmget(16, 20*sizeof(struct car), IPC_CREAT|0666);
	array = shmat(shmid,0,0);

	printf("Taille du circuit : %d\n", circuit_size);
	printf("Nombre de tours : %d\n", racing_laps);
	sleep(1);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);
	
	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");
	
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	printf("\x1b[31m\u25CF\x1b[0m\n");
	sleep(1.5);

	system("clear");

	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");	
	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");
	printf("\x1b[32m\u25CF\x1b[0m\n");
	sleep(1);
	

	int child_process_id = fork();
	if (child_process_id) {
		for(int i=0;i<20;i++){
			if(fork()==0){
				struct car *array;
				array = (struct car*)shmat(shmid,0,0);
				srand(time(NULL) ^ getpid());
				set_position_race(&array[i], i);
				race_car_attribute(i, array, num_list);
				time_generator_race(&array[i],racing_laps);
				best_time_race_attribute(i, array, num_list, racing_laps);

				shmdt(array);
				exit(0);
			}
		}
		for(int i=0;i<21;i++){
			wait(NULL);
		}
	}
	else{
		int j = 0;
		while(j<55){
			memcpy(copie, array, sizeof(copie) );
			race_display(20, copie);
			sleep(1);
			j++;
		}
		float time_value[4];
		best_time_circuit_test(array, time_value, 20);
		
		memcpy(copie, array, sizeof(copie) );
		qsort( copie,20,sizeof(struct car), (compfn)race_comparator);
		write_external_file(end_file, 20, copie, time_value, 3);
		write_external_file_sprint("grille_depart_sprint.txt",20,copie,time_value, 3);
		shmdt(array);
		shmctl(shmid,IPC_RMID,NULL);
		}
}
//////////////////////////////////////////////////////////////////////////////




int main(int argc, char *argv[]){
	//c pour championnat
    if(*argv[1] == 'c'){

        circuit_test(3600.00, "circuit_test_1.txt", 20);
        circuit_test(3600.00, "circuit_test_2.txt", 20);
        circuit_test(3600.00, "circuit_test_3.txt", 20);

        FILE * filedesc_reset_file = fopen("grille_depart_course.txt", "w");
        fprintf(filedesc_reset_file, "===========================================\ngrille de départ de la course:\n===========================================\n\n");
        fclose(filedesc_reset_file);
        qualification(1080.00, "qualification1.txt", "", 15, 20, 1);
        qualification(900.00, "qualification2.txt", "qualification1.txt",10, 15, 2);
        qualification(720.00, "qualification3.txt", "qualification2.txt", 10, 10, 3);

        int kmRace = atoi(argv[2]);
        race(kmRace, "resultat_championnat.txt", "grille_depart_course.txt");


    }
	//e pour essais
    else if(*argv[1] == 'e'){
		if(*argv[2] == '1'){
			circuit_test(5400.00, "test_circuit1.txt", 20);
		}
		else if(*argv[2] == '2'){
			circuit_test(5400.00, "test_circuit2.txt", 20);
		}
		else if(*argv[2] == '3'){
			circuit_test(3600.00, "test_circuit3.txt", 20);
		}
	}
    //q pour qualification
	else if(*argv[1] == 'q'){
		if(*argv[2] == '1'){
			FILE * filedesc_reset_file = fopen("grille_depart.txt", "w");
			fprintf(filedesc_reset_file, "-------------------------------\nGrille de départ de la course:\n-------------------------------\n\n");
			fclose(filedesc_reset_file);
			qualification(1080.00, "qualification_normale1.txt", "", 15, 20, 1);
		}
		else if(*argv[2] == '2'){
			qualification(900.00, "qualification_normale2.txt", "qualification_normale1.txt",10, 15, 2);
		}
		else if(*argv[2] == '3'){
			qualification(720.00, "qualification_normale3.txt", "qualification_normale2.txt", 10, 10, 3);
		}		
		
	}
    //f pour final
	else if(*argv[1] == 'f'){
        int kmRace = atoi(argv[2]);
		race(kmRace, "resultat_course_normal.txt", "grille_depart.txt");
	}
    else if(*argv[1] == 's'){
		//s pour sprint
        circuit_test(3600.00, "circuit_test_1.txt", 20);

        FILE * filedesc_reset_file = fopen("grille_depart_course.txt", "w");
        fprintf(filedesc_reset_file, "===========================================\ngrille de départ de la course:\n===========================================\n\n");
        fclose(filedesc_reset_file);
        qualification(1080.00, "qualification1.txt", "", 15, 20, 1);
        qualification(900.00, "qualification2.txt", "qualification1.txt",10, 15, 2);
        qualification(720.00, "qualification3.txt", "qualification2.txt", 10, 10, 3);

        circuit_test(3600.00, "circuit_test_2.txt", 20);

		int kmRace = atoi(argv[2]);
		sprint(kmRace, "resultat_sprint_qualification.txt", "grille_depart_course.txt");
		sleep(2);
		race(kmRace, "resultat_sprint_course.txt", "grille_depart_sprint.txt");
    }
}