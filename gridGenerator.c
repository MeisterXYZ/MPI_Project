#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void printGrid(int *gridPointer,long gridSize){
	if (gridSize<=100){
		int i,k;
		//print columns-header
		printf("   | ");
		for (i = 0; i < gridSize; i++){
			printf("%-2d",i+1);
		}
		printf("\n---|-");
		for (i = 0; i < gridSize; i++){
			printf("--");
		}
		printf("\n");
		//print the lines
		for (i = 0; i < gridSize; i++){
			for (k = 0; k < gridSize; k++){
				//print line index
				if (k == 0){
					printf("%-3d| ",i+1);
				}
				//print pixels
				switch (gridPointer[i*gridSize + k]) {
					case 1:{
						printf("# ");
						break;
					}
					default: {
						printf("%d ",gridPointer[i*gridSize + k]);
						break;
					}
				}	
			}
			printf("\n");
		}
	}else{
		printf("grid to big for printing it.\n");
	}
} 

void togglePixel(int *gridPointer,long gridSize, int line, int column){
	
	if (gridPointer[line*gridSize + column] == 0){
		gridPointer[line*gridSize + column] = 1;
	} else{
		gridPointer[line*gridSize + column] = 0;
	}
}

void toggleRectangle(int *gridPointer,long gridSize, int leftUpLine, int leftUpColumn, int rightLowLine, int rightLowColumn){
	
	int i,k;
	for (i = leftUpLine; i <= rightLowLine; i++){
		for (k = leftUpColumn; k <= rightLowColumn; k++){
			if (gridPointer[i*gridSize + k] == 0){
				gridPointer[i*gridSize + k] = 1;
			} else{
				gridPointer[i*gridSize + k] = 0;
			}
		}
	}
}

void randomizeRectangle(int *gridPointer,long gridSize, int leftUpLine, int leftUpColumn, int rightLowLine, int rightLowColumn){
	
	int i,k;
	for (i = leftUpLine; i <= rightLowLine; i++){
		for (k = leftUpColumn; k <= rightLowColumn; k++){
			gridPointer[i*gridSize + k] = rand() % 2;
		}
	}
}

void clearRectangle(int *gridPointer,long gridSize, int leftUpLine, int leftUpColumn, int rightLowLine, int rightLowColumn){
	
	int i,k;
	for (i = leftUpLine; i <= rightLowLine; i++){
		for (k = leftUpColumn; k <= rightLowColumn; k++){
			gridPointer[i*gridSize + k] = 0;
		}
	}
}

void chessRectangle(int *gridPointer,long gridSize, int leftUpLine, int leftUpColumn, int rightLowLine, int rightLowColumn){
	
	int i,k;
	for (i = leftUpLine; i <= rightLowLine; i++){
		for (k = leftUpColumn; k <= rightLowColumn; k++){
			gridPointer[i*gridSize + k] = (i%2+k) %2;
		}
	}
}

void writeToFile(char* name, int *gridPointer, long gridSize){
    FILE *fp;
	fp = fopen(name, "w");
    int i,k;
	for (i = 0; i < gridSize; i++){
		for (k = 0; k < gridSize; k++){
			//printf("%d ",gridPointer[i*gridSize + k]);							
			fprintf(fp, "%d ",gridPointer[i*gridSize + k]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	
}

int main(int argc, char** argv) {

	//Initialize MPI
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		char filename[100];
		long gridSize;

		if (argv[1]){
			strcpy(filename,argv[1]);
			printf("Opened: %s\n\n",filename);
		    int c;
			int lengthGot = 0;
			int length = 0;
			FILE *file;
			file = fopen(filename, "r");
			if (file) {
				while (!lengthGot){
					c = getc(file);
					if (c == '\n'){
						lengthGot = 1;
					}else{
						length++;
					}
				}
				gridSize = length/2;				
				fclose(file);
			}
			
		} else{
			printf("enter a filename (without file-suffix):\n");
			scanf("%s",filename);
			printf("enter size of grid you want to create.:\n");
			scanf("%ld",&gridSize);
			
		}
		
		//create array
		int *grid;
		grid = malloc (sizeof(int)*gridSize*gridSize);
		int i,k;

		if (argv[1]){
			//save the existing grid to array
			int c;
			i = 0;
			k = 0;
			FILE *file;
			file = fopen(filename, "r");
			if (file) {
				while ((c = getc(file)) != EOF){
					if (c != '\n' && c !=' '){
						grid[k*gridSize+i] = c - '0';
						if (i < gridSize-1){
							i++;
						}else{
							k++;
							i=0;
						}
					}
				}
				fclose(file);
			}

		} else {
			for (i = 0; i < gridSize; i = i+1){
				for (k = 0; k < gridSize; k = k + 1){
					grid[i*gridSize+k] = 0;
				}
			}
		}

		int userInput;
		int running = 1;

		do {
			printf("This is the grid:\n");
			printGrid(&grid[0],gridSize);
		
			printf("\nWhat do you want to do?\n1: Toggle a pixel\n2: Toggle a rectangle\n3: Toggle all pixels\n4: Randomize a rectangle\n5: Randomize all pixels\n6: generate chess grid\n7: clear grid\n8: Save Grid to File and exit.\n");
			scanf("%d",&userInput);

			switch (userInput) {

				case 1: {
					//toggle a pixel
					int togglePxLine, togglePxColumn;
					printf("Input line and column of element you want to toggle (1-based). i.e. 1/2 for element in first row,second column.\nToggle pixel in Line: \n");
					scanf("%d",&togglePxLine);
					printf("Column: \n");
					scanf("%d",&togglePxColumn);
					togglePixel(&grid[0],gridSize,togglePxLine-1,togglePxColumn-1);
					break;
				}
				case 2: {
					//toggle a rectangle
					int toggleReTopLine, toggleReTopColumn, toggleReBotLine, toggleReBotColumn;
					printf("Input line and column of the left upper element of the rectangle you want to toggle (1-based). \ni.e. 1/2 for element in first row,second column.\nLeft Upper pixel in Line: \n");
					scanf("%d",&toggleReTopLine);
					printf("Left Upper pixel in Column: \n");
					scanf("%d",&toggleReTopColumn);
					printf("Input line and column of the right lower element of the rectangle you want to toggle (1-based). \nRigt lower pixel in Line: \n");
					scanf("%d",&toggleReBotLine);
					printf("Rigt lower pixel in Column: \n");
					scanf("%d",&toggleReBotColumn);
					toggleRectangle(&grid[0],gridSize,toggleReTopLine-1,toggleReTopColumn-1,toggleReBotLine-1,toggleReBotColumn-1);
					break;
				}
				case 3:{
					//toggle all pixels
					toggleRectangle(&grid[0],gridSize,0,0,gridSize-1,gridSize-1);
					break;
				}
				case 4:{
					//Randomize Rectangle
				    int rndmReTopLine, rndmReTopColumn, rndmReBotLine, rndmReBotColumn;
					printf("Input line and column of the left upper element of the rectangle you want to randomize (1-based). \ni.e. 1/2 for element in first row,second column.\nLeft Upper pixel in Line: \n");
					scanf("%d",&rndmReTopLine);
					printf("Left Upper pixel in Column: \n");
					scanf("%d",&rndmReTopColumn);
					printf("Input line and column of the right lower element of the rectangle you want to toggle (1-based). \nRigt lower pixel in Line: \n");
					scanf("%d",&rndmReBotLine);
					printf("Rigt lower pixel in Column: \n");
					scanf("%d",&rndmReBotColumn);
					randomizeRectangle(&grid[0],gridSize,rndmReTopLine-1,rndmReTopColumn-1,rndmReBotLine-1,rndmReBotColumn-1);
					break;
				}
				case 5:{
					//Randomize all pixels
					randomizeRectangle(&grid[0],gridSize,0,0,gridSize-1,gridSize-1);
					break;
				}
				case 6:{
					chessRectangle(&grid[0],gridSize,0,0,gridSize-1,gridSize-1);
					break;
				}
				case 7:{
					clearRectangle(&grid[0],gridSize,0,0,gridSize-1,gridSize-1);
					break;
				}
				case 8:{
					//save to file
					running = 0;
					if (argv[1]){
						writeToFile(filename,&grid[0],gridSize);
					} else {
						writeToFile(strcat(filename,".txt"),&grid[0],gridSize);
					}
					printf("File was saved as %s\n",filename);
					break;	
				}
				default: {
					printf("No correct input.");
					break;
				}

			}
		} while (running);

	}
	MPI_Finalize();
}
