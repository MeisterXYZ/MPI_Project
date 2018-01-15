#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printGrid(char *gridPointer,long gridSize){
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
    				case 2:{
    					printf("x ");
    					break;
    				}
    				case 3:{
    					printf("# ");
    					break;
    				}
    				case 4:{
    					printf("? ");
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
	    printf("grid to large to display.\n");
	}
}

void printSmallGrid(char *gridPointer,int gridLines, int gridColumns){
	if (gridLines <= 100){
    	int i,k;
    	//print columns-header
    	printf("   | ");
    	for (i = 0; i < gridColumns; i++){
    		printf("%-2d",i+1);
    	}
    	printf("\n---|-");
    	for (i = 0; i < gridColumns; i++){
    		printf("--");
    	}
    	printf("\n");
    	//print the lines
    	for (i = 0; i < gridLines; i++){
    		for (k = 0; k < gridColumns; k++){
    			//print line index
    			if (k == 0){
    				printf("%-3d| ",i+1);
    			}
    			//print pixels
    			switch (gridPointer[i*gridColumns + k]) {
    				case 1:{
    					printf("# ");
    					break;
    				}
    				default: {
    					printf("%d ",gridPointer[i*gridColumns + k]);
    					break;
    				}
    			}	
    		}
    		printf("\n");
    	}
	}else{
	    printf("grid to large to display.\n");
	}
}

void printCharArray(char *arr, int size){
    int i;
    for (i = 0; i < size; i++){
        printf("%d",arr[i]);
    }
    printf("\n");
}  

void cancelFigure(char *gridPointer, int gridLines, int gridColumns, int line, int column, int sign){
    if (!(gridPointer[line*gridColumns+column] == sign) && !(gridPointer[line*gridColumns+column] == 0)){
        gridPointer[line*gridColumns+column] = sign;
        if (column -1 >= 0){
            cancelFigure(gridPointer,gridLines,gridColumns,line,column-1,sign);
        } 
        if (column +1 < gridColumns){
            cancelFigure(gridPointer,gridLines,gridColumns,line,column+1,sign);
        } 
        if (line -1 >= 0){
            cancelFigure(gridPointer,gridLines,gridColumns,line-1,column,sign);
        } 
        if (line +1 < gridLines){
            cancelFigure(gridPointer,gridLines,gridColumns,line+1,column,sign);
        } 
    }
}

void processSubgrid(char *gridPointer, long gridLines, long gridColumns, int subgridAbove, int subgridBelow){

    //go from px to px
    int i,k;
    for (i = 0; i < gridLines; i++){
        for (k = 0; k < gridColumns; k++){
            if (gridPointer[ i * gridColumns + k] == 1){
                //check if the figure is a rectangle:
                //keep in mind all connected pixels in one line
                int startcolumn = k;
                int endcolumn = k;
                int startline = i;
                int endline = i;
                //get end of startline
                while (endcolumn+1 < gridColumns && gridPointer[i*gridColumns + endcolumn + 1] == 1){
                    endcolumn += 1;
                }
                //check for every pixel in startline if there is one or none in next line
                int isRectangle = 1;
                int rectangleChecked = 0;
                while (isRectangle && !rectangleChecked && endline+1 < gridLines){
                    int lineEmpty = 1;
                    int l; 
                    for (l = startcolumn; l <= endcolumn; l++){
                        //first column in lines below checks if line should be empty or full
                        if (l == startcolumn){
                            if (gridPointer[(endline + 1)*gridColumns + l] == 1){
                                lineEmpty = 0;
                            }
                        } else {
                            //check for the following columns if conditions for a full or empty lien are complied
                            if ((lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 1) || (!lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 0)){
                                isRectangle = 0;
                                if (lineEmpty) {
                                    //printf("at poition %d/%d no rectangle. Reason: black pixel at %d/%d\n",i+1,k+1,endline+1+1,l+1);
                                } else{
                                    //printf("at poition %d/%d no rectangle. Reason: missing black pixel at %d/%d\n",i+1,k+1,endline+1+1,l+1);
                                }
                                break;
                            }
                        }
                    }
                    //if the line is not empty check if there is another pixel before startcolumn or after endcolumn. In this case shape is not a rectangle.
                    if (!lineEmpty){
                        if ((startcolumn-1 >= 0 && gridPointer[(endline + 1)*gridColumns + startcolumn-1] == 1)||(endcolumn+1 < gridColumns && gridPointer[(endline + 1)*gridColumns + endcolumn + 1] == 1)) {
                            isRectangle = 0;
                            //printf("at poition %d/%d no rectangle. Reason: black pixel befor start or end of startline.\n",i+1,k+1);
                            break;
                        } else {
                            endline += 1;
                        }
                    } else {
                        rectangleChecked = 1;
                    }
                }
                if (!isRectangle){
                    cancelFigure(gridPointer,gridLines,gridColumns,i,k,2);
                    //printf("at poition %d/%d no rectangle. Killing figure (2). Result:\n",i+1,k+1);
                    //printSmallGrid(gridPointer,gridLines,gridColumns);
                    //printf("\n");
                } else {
                    if ((subgridAbove && i == 0) || (subgridBelow && endline + 1 == gridLines)) {
                        //Just a potential rectangle
                        cancelFigure(gridPointer,gridLines,gridColumns,i,k,4);
                        //printf("at poition %d/%d a potential rectangle figure. Marking figure (4). Result:\n",i+1,k+1);
                    } else{
                        //a save rectangle
                        cancelFigure(gridPointer,gridLines,gridColumns,i,k,3);
                        //printf("at poition %d/%d a rectangle figure. Marking figure (3). Result:\n",i+1,k+1);
                    }
                    //printSmallGrid(gridPointer,gridLines,gridColumns);
                }
            }
        }
    }

}

void processSubgridWithOutput(char *gridPointer, int gridLines, int gridColumns, int subgridAbove, int subgridBelow){

    //go from px to px
    int i,k;
    for (i = 0; i < gridLines; i++){
        for (k = 0; k < gridColumns; k++){
            if (gridPointer[ i * gridColumns + k] == 1){
                //check if the figure is a rectangle:
                //keep in mind all connected pixels in one line
                int startcolumn = k;
                int endcolumn = k;
                int startline = i;
                int endline = i;
                //get end of startline
                while (endcolumn+1 < gridColumns && gridPointer[i*gridColumns + endcolumn + 1] == 1){
                    endcolumn += 1;
                }
                //check for every pixel in startline if there is one or none in next line
                int isRectangle = 1;
                int rectangleChecked = 0;
                while (isRectangle && !rectangleChecked && endline+1 < gridLines){
                    int lineEmpty = 1;
                    int l; 
                    for (l = startcolumn; l <= endcolumn; l++){
                        //first column in lines below checks if line should be empty or full
                        if (l == startcolumn){
                            if (gridPointer[(endline + 1)*gridColumns + l] == 1){
                                lineEmpty = 0;
                            }
                        } else {
                            //other lines are checking if conditions are complied 
                            if ((lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 1) || (!lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 0)){
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
                        if ((startcolumn-1 >= 0 && gridPointer[(endline + 1)*gridColumns + startcolumn-1] == 1)||(endcolumn+1 < gridColumns && gridPointer[(endline + 1)*gridColumns + endcolumn + 1] == 1)) {
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
                    cancelFigure(gridPointer,gridLines,gridColumns,i,k,2);
                    printf("at poition %d/%d no rectangle. Killing figure (2). Result:\n",i+1,k+1);
                    printSmallGrid(gridPointer,gridLines,gridColumns);
                    printf("\n");
                } else {
                    if ((subgridAbove && i == 0) || (subgridBelow && endline + 1 == gridLines)) {
                        //Just a potential rectangle
                        cancelFigure(gridPointer,gridLines,gridColumns,i,k,4);
                        printf("at poition %d/%d a potential rectangle figure. Marking figure (4). Result:\n",i+1,k+1);
                    } else{
                        //a save rectangle
                        cancelFigure(gridPointer,gridLines,gridColumns,i,k,3);
                        printf("at poition %d/%d a rectangle figure. Marking figure (3). Result:\n",i+1,k+1);
                    }
                    printSmallGrid(gridPointer,gridLines,gridColumns);
                    //To be done
                    //...
                }
            }
            //else
            //nothing to do here
        }
    }

}

void checkPotentialRectangle(char *gridPointer, long gridColumns, int line, int isUpperLine){
    int i;
    
    //for checking in both directions
    int lineOffset;
    if (isUpperLine){
        lineOffset = 1;
    } else {
        lineOffset = -1;
    }
    for (i=0; i < gridColumns; i++){
        if (gridPointer[gridColumns * line + i] == 4){
            int startcolumn = i;
            int endcolumn = i;
            //get end of startline
            while (endcolumn+1 < gridColumns && gridPointer[gridColumns * line + endcolumn + 1] == 4){
                endcolumn += 1;
            }
            //printf("evaluated endcolumn: %d\n",endcolumn);
            //check for every pixel in startline if there is one or none in next line
            int isRectangle = 1;
            int lineEmpty = 1;
            int l;
            for (l=startcolumn; l<= endcolumn; l++){
                //printf("considered pixel: %d/%d => %c\n",line +1,l,gridPointer[gridColumns * (line + 1) + l]);
                //if following line contains a shape which is not a rectangle the considered shape can't be a rectangle
                if (gridPointer[gridColumns * (line + lineOffset) + l] == 2) {
                    isRectangle = 0;
                    break;
                } else {
                    if (l == i){
                        //if the first column contains a potential rectangle shape the line must be completely filled for the shape beeing a rectangle. Value can just be 4 or 0
                        lineEmpty = (gridPointer[gridColumns * (line + lineOffset) + l] == 0);
                        //printf("set lineEmpty to: %d\n",(gridPointer[gridColumns * (line + 1) + l] == 0));
                    } else {
                        //for all other columns check if line is completely filled or is completely empty. otherwise considered shape can't be a rectangle
                        if ((lineEmpty && !(gridPointer[gridColumns * (line + lineOffset) + l] == 0)) || (!lineEmpty && gridPointer[gridColumns * (line + lineOffset) + l] == 4)) {
                            isRectangle = 0;
                            break;
                        }
                    }   
                }
            }
            //if the line is not empty check if there is another pixel before startcolumn or after endcolumn. In this case shape is not a rectangle.
            if (!lineEmpty){
                if ((startcolumn-1 >= 0 && gridPointer[gridColumns * (line + lineOffset) + startcolumn-1] == 4)||(endcolumn+1 < gridColumns && gridPointer[gridColumns * (line + lineOffset) + endcolumn + 1] == 4)) {
                    isRectangle = 0;
                    //printf("at poition %d/%d no rectangle. Reason: black pixel befor start or end of startline.\n",line+1+1,i+1);
                }
            }
            if (!isRectangle){
                cancelFigure(gridPointer,gridColumns,gridColumns,line,i,2);
            }else{
                cancelFigure(gridPointer,gridColumns,gridColumns,line,i,3);
            }
            
        }  
    }
}

void outputRectangleList(char *gridPointer,long gridSize){
    int k,i,rectangleCounter;
    int recEndline, recEndcolumn;
    rectangleCounter=0;
    for (k=0; k < gridSize; k++){
        for (i=0; i < gridSize; i++){
            if (gridPointer[k*gridSize + i]==3){
                rectangleCounter++;
                recEndline = k;
                recEndcolumn = i;
                while(gridPointer[(recEndline+1)*gridSize + i]==3){
                    recEndline++;
                }
                while (gridPointer[recEndline*gridSize + recEndcolumn + 1]==3){
                    recEndcolumn++;
                }
                if (recEndline == k && recEndcolumn == i){
                    printf("Rectangle %d at %d/%d\n",rectangleCounter,k+1,i+1);
                }else{
                    printf("Rectangle %d from %d/%d to %d/%d\n",rectangleCounter,k+1,i+1, recEndline+1,recEndcolumn+1);
                }
                cancelFigure(gridPointer,gridSize,gridSize,k,i,5);
                //printf("%d/%d\n",k+1,i+1);

            }
        }
    }
}


int main(int argc, char** argv) {
    //MPI variables
	int size, rank;
    MPI_Status status;

    //Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		long gridSize;
        char *grid;
        int reciever;
        if (argv[1]){
            FILE *file;
            file = fopen(argv[1], "r");
            if (file) {
                //import the grid to a char array
                {
                    //getting the grid-size:                
                    //lengthGot is a bool for not having to read the complete file for getting the grid length
                    int lengthGot = 0;
                    int length = 0;
                    int c,i,k;
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
                    //create char array from read file
                    //char grid[gridSize*gridSize];
                    grid = malloc(sizeof(int)*gridSize*gridSize);
                    //save the grid to the char array
                    rewind(file);
                    i = 0;
                    while ((c = getc(file)) != EOF){
                        if (c != '\n' && c !=' '){
                            grid[i] = c - '0';
                            i++;
                        }
                    }
                    //file is not longer needed
                    fclose(file);
                }


                //processing the grid

                //Output for overview:
                printf("THIS IS THE GRID:\n");
                printGrid(&grid[0],gridSize);
                printf("Grid: \n");
                //printCharArray(&grid[0],gridSize*gridSize);
                //send it to other processes
                //calculation how to split up array:

                div_t gridSegmentSize;
                int workingProcesses;
                
                if (size - 1 > gridSize){
                    gridSegmentSize = div(gridSize,gridSize);
                    workingProcesses = gridSize;
                } else {
                    gridSegmentSize = div(gridSize,size-1);
                    workingProcesses = size-1;
                }
                 
                int subgridLines;
                
                //send first information of grid size -> in case there are more processors then lines these can quit
                for (reciever = 1; reciever < size; reciever++){
                    MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
                } 

                //send to all required processes the number of lines of the subgrid and the subgrid itself.  
                for (reciever = 1; reciever <= workingProcesses ; reciever++){
                    if (reciever == workingProcesses){
                        subgridLines = gridSegmentSize.quot + gridSegmentSize.rem;
                        MPI_Send(&subgridLines, 1, MPI_INT, reciever, 0, MPI_COMM_WORLD);
                    } else {
                        subgridLines = gridSegmentSize.quot;
                        MPI_Send(&subgridLines, 1, MPI_INT, reciever, 0, MPI_COMM_WORLD);
                    }
                    MPI_Send(&grid[(reciever-1)*gridSize*gridSegmentSize.quot], subgridLines*gridSize, MPI_BYTE, reciever, 0, MPI_COMM_WORLD);
                }

                //recieve done results
                for (reciever = 1; reciever <= workingProcesses ; reciever++){
                    if (reciever == workingProcesses){
                        subgridLines = gridSegmentSize.quot + gridSegmentSize.rem;
                    } else {
                        subgridLines = gridSegmentSize.quot;
                    }
                    MPI_Recv(&grid[(reciever-1)*gridSize*gridSegmentSize.quot],subgridLines*gridSize,MPI_BYTE,reciever,0,MPI_COMM_WORLD,&status);
                }

                //Output for overview:
                printf("THIS IS THE GRID:\n");
                printGrid(&grid[0],gridSize);


                //TYPE SOME LOGIC FOR finalize the potential rectangles --> 4 signed rectangles.
                //go to border
                int i;
                for (i = 1; i < workingProcesses; i++){
                    //no need for checking the line at the end --> i < workingProcesses 
                    //printf("Check line %d and line %d\n",i*gridSegmentSize.quot - 1,i*gridSegmentSize.quot );
                    checkPotentialRectangle(&grid[0],gridSize,i*gridSegmentSize.quot - 1,1);
                    //later: check the upper line
                    checkPotentialRectangle(&grid[0],gridSize,i*gridSegmentSize.quot,0);

                } 
                printf("THIS IS THE GRID:\n");
                printGrid(&grid[0],gridSize);
                printf("\nLegend:\n0: Empty\n2: no Rectangle \n3:rectangle \n(4: potential Rectangle)\n\n");

                outputRectangleList(&grid[0],gridSize);
                //printGrid(&grid[0],gridSize);
/*
                //check first and last joined 4-pixels 
                //check next line (or previous (is there a shortcut?))
                    //if all or none -> make figure 3
                    //if sth between -> make figure 2 
*/

            } else{
                printf("could not open the data-file %s.\n",argv[1]);
                //cancel other processes
                gridSize = 0;
                for (reciever = 1; reciever < size; reciever++){
                    MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
                }
            }
        }else{
            printf("please run the rectangleChecker with a data-file as parameter.\n");
            //cancel other processes
            gridSize = 0;
            for (reciever = 1; reciever < size; reciever++){
                MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
            }
        }
	} else { 
    //The other Processes then the 0-Process
        //get grid Size
        long gridSize[2];
        MPI_Recv(&gridSize[1],1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        //just run process if needed.
        if (rank <= gridSize[1]){            
            MPI_Recv(&gridSize[0],1,MPI_INT,0,0,MPI_COMM_WORLD,&status);

            //set up grid
            //char grid[gridSize[0]*gridSize[1]];
            char *grid;
            grid = malloc(sizeof(int)*gridSize[0]*gridSize[1]);
            //recieve grid array
            MPI_Recv(&grid[0],gridSize[0]*gridSize[1],MPI_BYTE,0,0,MPI_COMM_WORLD,&status);
            
            //find out if subgrid is above or not
            int subgridAbove = (!(rank == 1));
            int subgridBelow = (!(rank*gridSize[0] >= gridSize[1]));
            
            //process the subgrid
            processSubgrid(&grid[0],gridSize[0],gridSize[1],subgridAbove,subgridBelow);
            
            //send it back to root process
            MPI_Send(&grid[0],gridSize[0]*gridSize[1],MPI_BYTE,0,0,MPI_COMM_WORLD);
        /*
        */
        } 
    }
	MPI_Finalize();
}
