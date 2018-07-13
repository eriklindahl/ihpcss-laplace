/*************************************************
 * Laplace hybrid MPI OpenACC Version
 *
 * Temperature is initially 0.0
 * Boundaries are as follows:
 *
 *      0         T         0
 *   0  +-------------------+  0
 *      |                   |
 *      |                   |
 *      |                   |
 *   T  |                   |  T
 *      |                   |
 *      |                   |
 *      |                   |
 *   0  +-------------------+ 100
 *      0         T        100
 *
 *  Modified by Kevin Ham from original by John Urbanic, PSC 2014
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

// size of plate: small 672  large 10752
#define COLUMNS       10752        // 
#define ROWS_GLOBAL   10752        // this is a "global" row count
#define NPES 28
#define ROWS (ROWS_GLOBAL/NPES)
#define UP 100
#define DOWN 101

// Use 10752 (16 times bigger) for large challenge problem
// All chosen to be easily divisible by Bridges' 28 cores per node

// largest permitted change in temp (This value takes 3264 steps)
#define MAX_TEMP_ERROR 0.01

double Temperature[ROWS+2][COLUMNS+2];      // temperature grid
double Temperature_last[ROWS+2][COLUMNS+2]; // temperature grid from last iteration

//   helper routines
void initialize(int npes, int my_PE_num);
void track_progress(int iter, double dt);


int main(int argc, char** argv) {
    int my_PE_num;
    int i, j, npes;                                            // grid indexes
    int max_iterations;                                  // number of iterations
    int iteration=1;                                     // current iteration
    double dt;                                       // largest change in t
    double dt_global=100;
    struct timeval start_time, stop_time, elapsed_time;  // timers
    //double start_time, stop_time, elapsed_time; 
    MPI_Status status;                                    // status returned by MPI call
    // the usual MPI startup routines
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);

    if (my_PE_num == 0)
      {
        printf("Running on %d MPI processes\n\n", npes);
      }

    // verify only NPES PEs are being used
    if(npes != NPES) {
      if(my_PE_num==0) {
        printf("This code must be run with %d PEs\n", NPES);
      }
      MPI_Finalize();
      exit(1);
    }

    // PE 0 asks for input
    if(my_PE_num==0) {
      //      printf("Maximum iterations [100-4000]?\n");
      //      fflush(stdout); // Not always necessary, but can be helpful
      //      scanf("%d", &max_iterations);
      max_iterations = 4000;
      printf("Maximum iterations = %d\n", max_iterations);

    }

    // bcast max iterations to other PEs
    MPI_Bcast(&max_iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //if (my_PE_num==0) start_time = MPI_Wtime();

    //    printf("Maximum iterations [100-4000]?\n");
    //    scanf("%d", &max_iterations);
    //max_iterations = 4000;
    //printf("Maximum iterations = %d\n", max_iterations);

    gettimeofday(&start_time,NULL); // Unix timer

    initialize(npes, my_PE_num);                   // initialize Temp_last including boundary conditions

    // do until error is minimal or until max steps
    #pragma acc data copy(Temperature_last), create(Temperature)
    while ( dt_global > MAX_TEMP_ERROR && iteration <= max_iterations ) {

        // main calculation: average my four neighbors
        #pragma acc kernels
        for(i = 1; i <= ROWS; i++) {
            for(j = 1; j <= COLUMNS; j++) {
                Temperature[i][j] = 0.25 * (Temperature_last[i+1][j] + Temperature_last[i-1][j] +
                                            Temperature_last[i][j+1] + Temperature_last[i][j-1]);
            }
        }
         #pragma acc update host(Temperature[1:1][1:COLUMNS],Temperature[ROWS:1][1:COLUMNS])
        // COMMUNICATION PHASE: send ghost rows for next iteration

        // send bottom real row down
        if(my_PE_num != npes-1){             //unless we are bottom PE
            MPI_Send(&Temperature[ROWS][1], COLUMNS, MPI_DOUBLE, my_PE_num+1, DOWN, MPI_COMM_WORLD);
        }

        // receive the bottom row from above into our top ghost row
        if(my_PE_num != 0){                  //unless we are top PE
            MPI_Recv(&Temperature_last[0][1], COLUMNS, MPI_DOUBLE, my_PE_num-1, DOWN, MPI_COMM_WORLD, &status);
        }

        // send top real row up
        if(my_PE_num != 0){                    //unless we are top PE
            MPI_Send(&Temperature[1][1], COLUMNS, MPI_DOUBLE, my_PE_num-1, UP, MPI_COMM_WORLD);
        }

        // receive the top row from below into our bottom ghost row
        if(my_PE_num != npes-1){             //unless we are bottom PE
            MPI_Recv(&Temperature_last[ROWS+1][1], COLUMNS, MPI_DOUBLE, my_PE_num+1, UP, MPI_COMM_WORLD, &status);
        }

        #pragma acc update device(Temperature_last[0:1][1:COLUMNS], Temperature_last[ROWS+1:1][1:COLUMNS])
        dt = 0.0; // reset largest temperature change

        // copy grid to old grid for next iteration and find latest 
        #pragma acc kernels
        for(i = 1; i <= ROWS; i++){
            for(j = 1; j <= COLUMNS; j++){
              dt = fmax( fabs(Temperature[i][j]-Temperature_last[i][j]), dt);
              Temperature_last[i][j] = Temperature[i][j];
            }
        }
        // find global dt                                                        
        MPI_Reduce(&dt, &dt_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Bcast(&dt_global, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // periodically print test values
        if((iteration % 100) == 0) {
          #pragma acc update host(Temperature)
          track_progress(iteration, dt);
	// if plate size = 672 use 504 and 622
	// if plate size = 10752 use 8064 and 10702
          if (8064/ROWS == my_PE_num+1){
               printf("PE %d: T(8064,10702) = %f\n", my_PE_num,Temperature[ROWS][10702]);
           }    
        }

        iteration++;
    }
    // Slightly more accurate timing and cleaner output 
    MPI_Barrier(MPI_COMM_WORLD);

    // PE 0 finish timing and output values
    if (my_PE_num==0){
        //stop_time = MPI_Wtime();
        //elapsed_time = stop_time - start_time;
        gettimeofday(&stop_time,NULL);
        timersub(&stop_time,&start_time,&elapsed_time); // Unix time subtract routine
        printf("\nMax error at iteration %d was %20.15g\n", iteration-1, dt_global);
       // printf("Total time was %f seconds.\n", elapsed_time);
       printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    }

    MPI_Finalize();

    //printf("\nMax error at iteration %d was %20.15g\n", iteration-1, dt);
    //printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

}


// initialize plate and boundary conditions
// Temp_last is used to to start first iteration
void initialize(int npes, int my_PE_num){

    double tMin, tMax;  //Local boundary limits
    int i,j;

    for(i = 0; i <= ROWS+1; i++){
        for (j = 0; j <= COLUMNS+1; j++){
            Temperature_last[i][j] = 0.0;
        }
    }

    // Local boundry condition endpoints
    tMin = (my_PE_num)*100.0/npes;
    tMax = (my_PE_num+1)*100.0/npes;

    // Left and right boundaries
    for (i = 0; i <= ROWS+1; i++) {
      Temperature_last[i][0] = 0.0;
      Temperature_last[i][COLUMNS+1] = tMin + ((tMax-tMin)/ROWS)*i;
    }

    // Top boundary (PE 0 only)
    if (my_PE_num == 0)
      for (j = 0; j <= COLUMNS+1; j++)
        Temperature_last[0][j] = 0.0;

    // Bottom boundary (Last PE only)
    if (my_PE_num == npes-1)
      for (j=0; j<=COLUMNS+1; j++)
        Temperature_last[ROWS+1][j] = (100.0/COLUMNS) * j;

}

// only called by last PE
// print diagonal in bottom right corner where most action is
void track_progress(int iteration, double dt) {

    int i;

    printf("---- Iteration %d, dt = %f ----\n", iteration, dt);
    for(i = 5; i >= 3; i--) {
      printf("[%d,%d]: %5.2f  ", ROWS_GLOBAL-i, COLUMNS-i, Temperature[ROWS-i][COLUMNS-i]);
    }
    printf("\n");

}

