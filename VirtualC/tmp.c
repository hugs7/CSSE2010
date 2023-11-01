#include <stdio.h>

void loop(void) {
	int count = 0;
	int sum = 0;
	for(int i = 1; i <= 100; i++) {
		if (i % 3 && i % 5) {
			count++;
			sum += i;
		}
	}
	printf("%d %d", count, sum);
}

int main(void){
	loop();
	return 0;
}