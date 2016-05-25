/*******************************************************************************
* useful_functions.c
* 
* This is a collection of miscellaneous useful functions that are part of the
* robotics cape library. These do not necessarily interact with hardware.
*******************************************************************************/

#include "../robotics_cape.h"
#include "../useful_includes.h"

/*******************************************************************************
* @ int null_func()
*
* A simple function that returns 0. This exists so function pointers can be 
* set to do nothing such as button and imu interrupt handlers.
*******************************************************************************/
int null_func(){
	return 0;
}

/*******************************************************************************
* @ saturate_float(float* val, float min, float max)
*
* Modifies val to be bounded between between min and max. Returns 1 if 
* saturation occurred, 0 if val was already in bound, and -1 if min was falsely
* larger than max.
*******************************************************************************/
int saturate_float(float* val, float min, float max){
	// sanity checks
	if (min>max){
		printf("ERROR: in saturate_float, min must be less than max\n");
		return -1;
	}
	// bound val
	if(*val>max){
		*val = max;
		return 1;
	}else if(*val<min){	
		*val = min;
		return 1;
	}
	return 0;
}


/*******************************************************************************
* @ char *byte_to_binary(char x)
* 
* This returns a string (char*) of '1' and '0' representing a character.
* For example, print "00101010" with printf(byte_to_binary(42));
*******************************************************************************/
char *byte_to_binary(unsigned char c){
    static char b[9];
	unsigned char x = (unsigned char)c; //cast to unsigned
    b[0] = '\0';
    unsigned char z;
    for (z = 128; z > 0; z >>= 1){
        if(x&z) strcat(b, "1");
		else strcat(b, "0");
    }
    return b;
}


/*******************************************************************************
* @ timespec timespec_diff(timespec start, timespec end)
* 
* Returns the time difference between two timespec structs as another timespec.
* Convenient for use with nanosleep() function and accurately timed loops.
* Unlike timespec_sub defined in time.h, timespec_diff does not care which came 
* first, A or B. A positive difference in time is always returned.
*******************************************************************************/
timespec timespec_diff(timespec A, timespec B){
	timespec temp;
	// check which is greater and flip if necessary.
	// For the calculation we want A>B
	if(B.tv_sec>A.tv_sec){
		temp = A;
		A = B;
		B = temp;
	} else if (B.tv_sec==A.tv_sec && B.tv_nsec>A.tv_nsec){
		temp = A;
		A = B;
		B = temp;
	}
	
	// now calculate the difference
	if((A.tv_nsec-B.tv_nsec)<0) {
		temp.tv_sec = A.tv_sec-B.tv_sec-1;
		temp.tv_nsec = 1000000000L+A.tv_nsec-B.tv_nsec;
	}else{
		temp.tv_sec = A.tv_sec-B.tv_sec;
		temp.tv_nsec = A.tv_nsec-B.tv_nsec;
	}
	return temp;
}

/*******************************************************************************
* @ uint64_t timespec_to_micros(timespec ts)
* 
* Returns a number of microseconds corresponding to a timespec struct.
* Useful because timespecs are annoying.
*******************************************************************************/
uint64_t timespec_to_micros(timespec ts){
	return (((uint64_t)ts.tv_sec*1000000)+(ts.tv_nsec/1000));
}

/*******************************************************************************
* @ uint64_t timeval_to_micros(timeval ts)
* 
* Returns a number of microseconds corresponding to a timespec struct.
* Useful because timespecs are annoying.
*******************************************************************************/
uint64_t timeval_to_micros(timeval tv){
	return (((uint64_t)tv.tv_sec*1000000)+tv.tv_usec);
}

/*******************************************************************************
* @ uint64_t micros_since_epoch()
* 
* handy function for getting current time in microseconds
* so you don't have to deal with timespec structs
*******************************************************************************/
uint64_t micros_since_epoch(){
	struct timeval tv;
	gettimeofday(&tv, NULL);  
	return timeval_to_micros(tv);
}


/*******************************************************************************
* @ int suppress_stdout(int (*func)(void))
*
* Executes a functiton func with all outputs to stdout suppressed. func must
* take no arguments and must return an integer. Adapt this source to your
* liking if you need to pass arguments. For example, if you have a function
* int foo(), call it with supressed output to stdout as follows:
* int ret = suppress_stdout(&foo);
*******************************************************************************/
int suppress_stdout(int (*func)(void)){
	int ret=0;
	int old_stdout;
	FILE  *null_out;

	// change stdout to null for this operation as the prussdrv.so
	// functions are noisy
	old_stdout = dup(STDOUT_FILENO);
	fflush(stdout);
	null_out = fopen("/dev/null", "w");
    dup2(fileno(null_out), STDOUT_FILENO);

	// execute the function
	ret=func();

	// put back stdout
	fflush(stdout);
	fclose(null_out);
    dup2(old_stdout,STDOUT_FILENO);
	close(old_stdout);

    return ret;
}

/*******************************************************************************
* @ int suppress_stderr(int (*func)(void))
* 
* executes a functiton func with all outputs to stderr suppressed. func must
* take no arguments and must return an integer. Adapt this source to your
* liking if you need to pass arguments. For example, if you have a function
* int foo(), call it with supressed output to stderr as follows:
* int ret = suppress_stderr(&foo);
*******************************************************************************/
int suppress_stderr(int (*func)(void)){
	int ret=0;
	int old_stderr;
	FILE  *null_out;

	// change stdout to null for this operation as the prussdrv.so
	// functions are noisy
	old_stderr = dup(STDERR_FILENO);
	fflush(stderr);
	null_out = fopen("/dev/null", "w");
    dup2(fileno(null_out), STDERR_FILENO);

	// execute the function
	ret=func();

	// put back stdout
	fflush(stderr);
	fclose(null_out);
    dup2(old_stderr,STDERR_FILENO);
	close(old_stderr);

    return ret;
}


