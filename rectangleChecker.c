#include <mpi.h>
#include <stdio.h>
#include <string.h>

void printGrid(int *gridPointer,int gridSize){
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
} 

void cancelFigure(int *gridPointer, int gridSize, int line, int column, int sign){
    if (gridPointer[line*gridSize+column]==1){
        gridPointer[line*gridSize+column] = sign;
        if (column -1 >= 0){
            cancelFigure(gridPointer,gridSize,line,column-1,sign);
        } 
        if (column +1 < gridSize){
            cancelFigure(gridPointer,gridSize,line,column+1,sign);
        } 
        if (line -1 >= 0){
            cancelFigure(gridPointer,gridSize,line-1,column,sign);
        } 
        if (line +1 < gridSize){
            cancelFigure(gridPointer,gridSize,line+1,column,sign);
        } 
    }
}

int main(int argc, char** argv) {
    //Initialize MPI
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		if (argv[1]){  
            FILE *file;
            file = fopen(argv[1], "r");
            if (file) {
                //getting the grid-size:
                int gridSize;
                //lengthGot is a bool for not having to read the complete file for getting the grid length
                int lengthGot = 0;
                int length = 0;
                int c;
                while (!lengthGot){
                    c = getc(file);
                    if (c == '\n'){
                        lengthGot = 1;
                    }else{
                        length++;
                    }
                }
                //half of the lines in textfiles are spaces
                gridSize = length/2;				
                
                //create array from read file
                rewind(file);
                int grid[gridSize][gridSize];
                int i,k;
                i = 0;
                k = 0;
                while ((c = getc(file)) != EOF){
                    if (c != '\n' && c !=' '){
                        grid[k][i] = c - '0';
                        if (i < gridSize-1){
                            i++;
                        }else{
                            k++;
                            i=0;
                        }
                    }
                }
                fclose(file);

                //processing the grid
                printf("THIS IS THE GRID:\n");
                printGrid(&grid[0][0],gridSize);

                //go from px to px
                for (i = 0; i < gridSize; i++){
                    for (k = 0; k < gridSize; k++){
                        if (grid[i][k]==1){
                            //check if the figure is a rectangle:
                            //keep in mind all connected pixels in one line
                            int startcolumn = k;
                            int endcolumn = k;
                            int startline = i;
                            int endline = i;
                            //get end of startline
                            while (endcolumn+1 < gridSize && grid[i][endcolumn+1]==1){
                                endcolumn += 1;
                            }
                            //DEBUG:printf("evaluated start: %d/%d  evaluated end:%d/%d\n",startline+1,startcolumn+1,startline+1,endcolumn+1);
                            //check for every pixel in startline if there is one or none in next line
                            int isRectangle = 1;
                            int rectangleChecked = 0;
                            while (isRectangle && !rectangleChecked && endline+1 < gridSize){
                                int lineEmpty = 1;
                                int l; 
                                for (l = startcolumn; l <= endcolumn; l++){
                                    if (l == startcolumn){
                                        if (grid[endline + 1][l]==1){
                                            lineEmpty = 0;
                                        }
                                    } else {
                                        if ((lineEmpty && grid[endline + 1][l]==1)||(!lineEmpty && grid[endline + 1][l]==0)){
                                            isRectangle = 0;
                                            if (lineEmpty) {
                                                printf("at poition %d/%d no rectangle. Reason: black pixel at %d/%d\n",i+1,k+1,endline+1+1,l+1);
                                            } else{
                                                printf("at poition %d/%d no rectangle. Reason: missing black pixel at %d/%d\n",i+1,k+1,endline+1+1,l+1);
                                            }
                                            break;
                                        }
                                    }
                                }
                                if (!lineEmpty){
                                    if ((startcolumn-1 >= 0 && grid[endline + 1][startcolumn-1]==1)||(endcolumn+1 < gridSize && grid[endline + 1][endcolumn+1]==1)) {
                                        isRectangle = 0;
                                        printf("at poition %d/%d no rectangle. Reason: black pixel befor start or end of startline.\n",i+1,k+1);
                                        break;
                                    } else {
                                        endline += 1;
                                    }
                                } else {
                                    rectangleChecked = 1;
                                }
                            }
                            if (!isRectangle){
                                cancelFigure(&grid[0][0],gridSize,i,k,2);
                                printf("at poition %d/%d no rectangle. Killing figure (2). Result:\n",i+1,k+1);
                                printGrid(&grid[0][0],gridSize);
                                printf("\n");
                            } else {
                                cancelFigure(&grid[0][0],gridSize,i,k,3);
                                printf("at poition %d/%d a rectangle figure. Marking figure (3). Result:\n",i+1,k+1);
                                printGrid(&grid[0][0],gridSize);
                                //To be done
                                //...
                            }
                        }
                        //else

                    }
                }
                printGrid(&grid[0][0],gridSize);

            } else{
                printf("could not open the data-file %s.\n",argv[1]);
            }
        }else{
            printf("please run the rectangleChecker with a data-file as parameter.\n");
        }
	}
	MPI_Finalize();
}