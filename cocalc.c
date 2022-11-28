/*
 * Street synchronization problem code
 */

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/* Constants for simulation */

#define ALLOWED_CARS 3 /* Number of cars allowed on street at a time */
#define USAGE_LIMIT 7  /* Number of times the street can be used before repair */
#define MAX_CARS 1000  /* Maximum number of cars in the simulation */

#define INCOMING "Incoming"
#define OUTGOING "Outgoing"

/* Add your synchronization variables here */

/* These obvious variables are at your disposal. Feel free to remove them if you want */
static int cars_on_street;	  /* Total numbers of cars currently on the street */
static int incoming_onstreet; /* Total numbers of cars incoming on the street */
static int outgoing_onstreet; /* Total numbers of cars outgoing on the street */
static int cars_since_repair; /* Total numbers of cars entered since the last repair */
static int cars_entered;
static int cars_left;

typedef struct
{
	int arrival_time; // time between the arrival of this car and the previous car
	int travel_time;  // time the car takes to travel on the street
	char car_direction[20];
	int car_id;
} car;

/* Called at the starting of simulation.  Initialize all synchronization
 * variables and other global variables that you add.
 */
static int
initialize(car *arr, char *filename)
{

	cars_on_street = 0;
	incoming_onstreet = 0;
	outgoing_onstreet = 0;
	cars_since_repair = 0;

	/* Initialize your synchronization variables (and
	 * other variables you might use) here
	 */

	/* Read in the data file and initialize the car array */
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("Cannot open input file %s for reading.\n", filename);
		exit(1);
	}
	int i = 0;
	while ((fscanf(fp, "%d%d%s\n", &(arr[i].arrival_time), &(arr[i].travel_time), arr[i].car_direction) != EOF) && i < MAX_CARS)
	{
		i++;
	}
	fclose(fp);
	return i;
}

/* Code executed by street on the event of repair
 * Do not add anything here.
 */
static void
repair_street()
{
	printf("The street is being repaired now.\n");
	sleep(5);
}

// Code for the street which repairs it when necessary and is cyclic. Needs to be synchronized with the cars.

/* Code executed by the cars
 * Do not add anything here.
 */
static void
drive_on_street(car *my_car)
{
	printf("Car %d of type %s is driving on the street for %d units of time", my_car->car_id, my_car->car_direction, my_car->travel_time);

	sleep(my_car->travel_time);
}

void *street_thread(void *junk)
{

	/* YOUR CODE HERE. */
	// printf("The street is ready to use\n");

	/* Loop while waiting for cars to arrive. */
	while (1)
	{
		/* YOUR CODE HERE. */
		/* Repair the street if necessary. */
		if (cars_since_repair >= USAGE_LIMIT)
		{
			printf("cars_on_street: %d, incoming_onstreet: %d, outgoing_onstreet: %d, cars_since_repair: %d\n", cars_on_street, incoming_onstreet, outgoing_onstreet, cars_since_repair);
			repair_street();
			cars_since_repair = 0;
			printf("ok");
		}
		else
			break;
	}

	pthread_exit(NULL);
}

/* Code executed by an incoming car to enter the street.
 * You have to implement this.
 */
void incoming_enter()
{
	/* You might want to add synchronization for the simulations variables	*/
	// add synchronization for the simulations variables
	// wait to complete previous cars
	// pthread_mutex_lock(&lock);
	while (cars_on_street == ALLOWED_CARS || outgoing_onstreet > 0 || cars_since_repair >= USAGE_LIMIT)
	{
		pthread_cond_wait;
	}
	cars_entered++;
	cars_on_street++;
	incoming_onstreet++;
	cars_since_repair++;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
}

/* Code executed by an outgoing car to enter the street.
 * You have to implement this.
 */
void outgoing_enter()
{
	// You might want to add synchronization for the simulations variables

	// pthread_mutex_lock(&mutex);
	while (cars_on_street >= ALLOWED_CARS || incoming_onstreet > 0 || cars_since_repair >= USAGE_LIMIT)
	{
		pthread_cond_wait;
	}
	cars_entered++;
	cars_on_street++;
	outgoing_onstreet++;
	cars_since_repair++;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
}

// Code executed by a car to simulate the duration for travel
// You do not need to add anything here.
static void
travel(int t)
{
	sleep(t);
}

// Code executed by an incoming car when leaving the street. You need to implement this
static void
incoming_leave()
{
	// pthread_mutex_lock(&mutex);
	cars_left++;
	cars_on_street--;
	incoming_onstreet--;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
}

/* Code executed by an outgoing car when leaving the street.
 * You need to implement this.
 */
static void
outgoing_leave()
{
	// pthread_mutex_lock(&passedMutex);
	cars_left++;
	cars_on_street--;
	outgoing_onstreet--;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
}

// Main code for incoming car threads. You do not need to change anything here, but you can add debug statements to help you during development/debugging
void *
incoming_thread(void *arg)
{
	car *car_info = (car *)arg;
	sleep(car_info->arrival_time);

	/* enter street */
	printf("Incoming car %d arrives at the street.\n", car_info->car_id);
	incoming_enter();
	// street_thread(NULL);
	/* Car travel --- do not make changes to the 3 lines below*/
	printf("Incoming car %d has entered and travels for %d minutes\n", car_info->car_id, car_info->travel_time);
	travel(car_info->travel_time);
	printf("Incoming car %d has travelled and prepares to leave\n", car_info->car_id);

	/* leave street */
	incoming_leave();

	pthread_exit(NULL);
}

// Main code for outgoing car threads. You do not need to change anything here, but you can add debug statements to help you during development/debugging
void *
outgoing_thread(void *arg)
{
	car *car_info = (car *)arg;

	/* enter street */
	outgoing_enter();
	// street_thread(NULL);
	/* Car travel --- do not make changes to the 3 lines below*/
	printf("Outgoing car %d has entered and travels for %d minutes\n", car_info->car_id, car_info->travel_time);
	travel(car_info->travel_time);
	printf("Outgoing car %d has travelled and prepares to leave\n", car_info->car_id);

	/* leave street */
	outgoing_leave();

	pthread_exit(NULL);
}

// Main function sets up simulation and prints report at the end
int main(int argc, char *argv[])
{
	int i, num_cars;
	pthread_t street, incoming_cars[MAX_CARS], outgoing_cars[MAX_CARS];
	car car_info[MAX_CARS];
	int result;

	if (argc != 2)
	{
		printf("Usage: %s <input file>\n", argv[0]);
		exit(1);
	}

	/* Initialize synchronization variables and other global variables */
	num_cars = initialize(car_info, argv[1]);

	if (num_cars > MAX_CARS || num_cars <= 0)
	{
		printf("Error:  Bad number of car threads. Maybe there was a problem with your input file?\n");
		return 1;
	}

	printf("Beginning traffic simulation with %d cars ...\n", num_cars);

	/* Create street thread */
	result = pthread_create(&street, NULL, street_thread, NULL);
	if (result)
	{
		printf("traffic:  pthread_create failed for street: %s\n", strerror(result));
		exit(1);
	}

	/* Create incoming car threads */
	for (i = 0; i < num_cars; i++)
	{
		car_info[i].car_id = i;
		if (strcmp(car_info[i].car_direction, "incoming") == 0)
		{
			// pthread_create(&incoming_cars[i], NULL, street_thread, (void *)&car_info[i]);
			result = pthread_create(&incoming_cars[i], NULL, incoming_thread, (void *)&car_info[i]);
		}
		if (strcmp(car_info[i].car_direction, "outgoing") == 0)
		{
			// pthread_create(&outgoing_cars[i], NULL, street_thread, (void *)&car_info[i]);
			result = pthread_create(&outgoing_cars[i], NULL, outgoing_thread, (void *)&car_info[i]);
		}
	}


	/* Wait for all threads to complete */
	for (i = 0; i < num_cars; i++)
	{
		if (car_info[i].car_direction == "incoming")
		{
			result = pthread_join(incoming_cars[i], NULL);
		}
		if (result)
		{
			printf("traffic:  pthread_join failed for car %d: %s\n", i, strerror(result));
			exit(1);
		}
	}
	for (i = 0; i < num_cars; i++)
	{
		if (car_info[i].car_direction == "outgoing")
		{
			result = pthread_join(outgoing_cars[i], NULL);
		}
		if (result)
		{
			printf("traffic:  pthread_join failed for car %d: %s\n", i, strerror(result));
			exit(1);
		}
	}
	if (result)
	{
		printf("traffic: thread_fork failed for car %d: %s\n", i, strerror(result));
		exit(1);
	}

	/* wait for all car threads to finish */
	for (i = 0; i < num_cars; i++)
		pthread_join(street, NULL);

	/* terminate the street thread. */
	pthread_cancel(street);

	// /* Print report */
	// printf("Simulation complete.  Here is the report:\n");
	// printf("Total number of cars that entered the street: %d\n", cars_entered);
	// printf("Total number of cars that left the street: %d\n", cars_left);
	// printf("Total number of cars that are still on the street: %d\n", cars_on_street);
	// printf("Total number of cars that are still on the street and going in the incoming direction: %d\n", incoming_onstreet);
	// printf("Total number of cars that are still on the street and going in the outgoing direction: %d\n", outgoing_onstreet);

	// printf("Simulation has completed.\n");
	pthread_exit(NULL);
	return 0;
}