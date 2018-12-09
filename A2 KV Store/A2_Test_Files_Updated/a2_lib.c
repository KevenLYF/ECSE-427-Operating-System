#include "a2_lib.h"

int kv_store_create(char *kv_store_name){
	// implement your create method code here

	// Which value was the earliest one to be added
	// Which value is the next one to be read
	// How many free slots are there in the pod
	// How many readers are currently working with the shared mem

	// initialize bookkeeping information
	int early_index = 0;
	int next_index = 1;
	int num_spots = NUM_PAIRS_PER_POD;

	int size_per_pod = (KEY_SIZE + VALUE_SIZE) * NUM_PAIRS_PER_POD + 3 * sizeof(int);
	int total_size = NUM_PODs * size_per_pod; 

	int shm_fd = shm_open(__KV_STORE_NAME__, O_CREAT|O_RDWR|O_EXCL, S_IRWXU);
	if (shm_fd == -1) {
		printf("Error creating shared memory!\n");
		printf("%s\n", strerror(errno));
		return KV_EXIT_FAILURE;
	}
	
	ftruncate(shm_fd, total_size);

	char *shm_addr = mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_addr == MAP_FAILED) {
		printf("Error mapping shared memory!\n");
		printf("%s\n", strerror(errno));
		return KV_EXIT_FAILURE;
	}

	close(shm_fd);

	// store earliest index
	for (char *c = shm_addr; c < shm_addr + total_size; c += size_per_pod) {
		memcpy(c, &early_index, sizeof(int));
	}
	// store next index
	for (char *c = shm_addr + sizeof(int); c < shm_addr + total_size; c += size_per_pod) {
		memcpy(c, &next_index, sizeof(int));
	}
	// store number of spots left
	for (char *c = shm_addr + 2*sizeof(int); c < shm_addr + total_size; c += size_per_pod) {
		memcpy(c, &num_spots, sizeof(int));
	}

	// unmap 
	munmap((void*)shm_addr, total_size);
	return KV_EXIT_SUCCESS;
}

int kv_store_write(char *key, char *value){
	// implement your create method code here
	return 0;
}

char *kv_store_read(char *key){
	// implement your create method code here
	struct stat s;

	int shm_fd = shm_open(__KV_STORE_NAME__, O_RDWR, 0);
	if (shm_fd == -1) {
		printf("Error opening shared memory!\n");
		printf("%s\n", strerror(errno));
		return NULL;
	}

	if (fstat(shm_fd, &s) == -1) {
		printf("Error fstat\n");
		printf("%s\n", strerror(errno));
		return NULL;
	}
	
	int size_per_pod = (KEY_SIZE + VALUE_SIZE) * NUM_PAIRS_PER_POD + 3 * sizeof(int);
	int total_size = NUM_PODs * size_per_pod; 

	char *shm_addr = mmap(NULL, total_size, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_addr == MAP_FAILED) {
		printf("Error mapping shared memory!\n");
		printf("%s\n", strerror(errno));
		return NULL;
	}


	int n;
	memcpy(&n, shm_addr, sizeof(int));
	printf("%d\n",n);
	close(shm_fd);
	
	return shm_addr;
}

char **kv_store_read_all(char *key){
	// implement your create method code here
	return NULL;
}


/* -------------------------------------------------------------------------------
	MY MAIN:: Use it if you want to test your impementation (the above methods)
	with some simple tests as you go on implementing it (without using the tester)
	-------------------------------------------------------------------------------
*/
int main() {
	kv_store_create(__KV_STORE_NAME__);

	kv_store_read("Fuck OS!");

	return EXIT_SUCCESS;
}
