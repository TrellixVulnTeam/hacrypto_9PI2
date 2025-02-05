#include <stdio.h>
#include <stdlib.h>

int compare_results(unsigned char *res1, unsigned char *res2, int length)
{
	int i;
	for(i=0; i<length; i++)
	{
		if(res1[i]!=res2[i]){
			return 0;
		}
	}
	return 1;
}

int check_test(unsigned char *result0, unsigned char *result1, int length, unsigned char *errormsg){
	if(!compare_results(result0, result1, length)){
		printf(errormsg);
		return 0;
	}
	return 1;
}

int check_KAT(unsigned char *result,
        unsigned char *expected_result, int length,
        unsigned char *testname){
	if(!compare_results(result, expected_result, length)){
		printf("%s failed!\n",testname);
		return 1;
	}
	return 0;
}

void write_hex(long long length, unsigned char *bytes, FILE *f){
	long long i;
	for(i=0; i<length; i++){
		printf("%02h", bytes[i]);
		fprintf(f, "%02h", bytes[i]);
	}
}

void write_rsp(long long inlength,unsigned char *input,unsigned char *output, long long outlength, FILE *frsp){
	fprintf(frsp, "Len = %lu\nMsg = ", inlength);
	write_hex(inlength, input, frsp);
	fprintf(frsp, "\nMD = ");
	write_hex(outlength, output, frsp);
	fprintf(frsp, "\n\n");
}

//specialized to one input... may need versions for more
//not sure if it is worth making it fully generic with
//respect to number of inputs
//TODO This function expects a very correct file for SHA256. Maybe it can/should be generalized?
int do_comparison1(char *infile, char *outfile,
		int (*funcs[])(unsigned char *,unsigned char *, unsigned long long), int funcslength, char *rsps[]){
	setbuf(stdout,NULL);
	FILE *fpin = fopen(infile, "r");
	FILE *fpout = fopen(outfile, "r");

	int i;
	FILE** frsps = malloc(funcslength * sizeof(FILE*));
	for(i=0; i<funcslength; i++){
		frsps[i] = fopen(rsps[i], "w+");
		if (frsps[i] == NULL) {
				fprintf(stderr, "Can't open rsp file %s\n", rsps[i]);
				exit(1);
		}
	}

	if (fpin == NULL) {
		fprintf(stderr, "Can't open input file %s!\n", infile);
		exit(1);
	}

	if (fpout == NULL) {
		fprintf(stderr, "Can't open output file %s\n", outfile);
		exit(1);
	}



	unsigned long inlength, outlength;
	int testno;
	unsigned char nextstring[50]; //should be safe unless comments are malicious...
	testno=1;
	while(fscanf(fpin, "%s", nextstring) != EOF){
		while(nextstring[0] == '#' || nextstring[0] == '[' ){
			fscanf(fpin, "%*[^\n]\n", NULL); //skip a line, it is a comment or additional data
			fscanf(fpin, "%s", nextstring); //should be Len when loop stops
		}

		fscanf(fpin, "%s", nextstring); //read the "="
		fscanf(fpin, "%lu", &inlength);

		inlength = inlength/8; //convert from size in bits to size in bytes

		unsigned char input0[0];
		unsigned char* input = malloc(inlength * sizeof(char));

		fscanf(fpin, "%s", nextstring); //read msg
		fscanf(fpin, "%s", nextstring); //read "="

		if(inlength == 0){
			fscanf(fpin, "%*2hhx");
			input = input0;
		}

		else{
			for(i=0; i<inlength; i++){
				fscanf(fpin, "%2hhx", &input[i]);
			}
		}

		fscanf(fpout, "%lu", &outlength);
		unsigned char* output1 = malloc(outlength * sizeof(char));
		unsigned char* output2 = malloc(outlength * sizeof(char));

		for(i=0; i<outlength; i++){
			fscanf(fpout, "%2hhx", &output1[i]);
		}

		for(i=0; i<funcslength; i++){
			funcs[i](input, output2, inlength);
			if (!compare_results(output1, output2, outlength)){
				printf("Test %d of file %s failed for function %d\n", testno, infile, i);
			}
			write_rsp(inlength, input, output2, outlength, frsps[i]);
		}


		if(inlength != 0){
			free(input);
		}
		free(output1);
		free(output2);
		testno++;
	}
	fclose(fpin);
	fclose(fpout);
	return 0;
}
