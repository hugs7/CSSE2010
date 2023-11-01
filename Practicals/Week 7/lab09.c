#include <stdio.h>
#include <string.h>

int main(void) {
	char str[81];
	int i, len;
	/* Read a line of text from the user. Accepts up to
	** 80 characters and then null terminates the string.
	** (Newline remains at the end of the line. If the user
	** entered too many characters then only the first 80
	** are returned.)
	*/
	printf("Enter string:\n");
	fgets(str, 81, stdin);

	/* Determine length of string */
	len = strlen(str);

	/* If last character in string is a newline, replace it with 
	** a null character.
	*/
	if(str[len-1] == '\n') {
		str[len-1] = 0; /* Null character - we could also have said '\0' */
	}

	/* Iterate over the string until we reach a null character
	** (i.e. str[i] == 0) 
	*/
	for(i=0; str[i]; i++) {
		if(str[i] >= 'A' && str[i] <= 'Z') {
			/* Character is uppercase. Convert it to
			** lowercase by adding 32 - the difference
			** in ASCII codes between lowercase and uppercase
			** characters. Could also say str[i] += 'a' - 'A'; */
			str[i] += 32;
		}
	}
	printf("%s\n", str);
	return 0;
}
