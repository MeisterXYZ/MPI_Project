#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


void printGrid(char *gridPointer,int gridSize){
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

void printCharArray(char *arr, int size){
    int i;
    for (i = 0; i < size; i++){
        printf("%d",arr[i]);
    }
    printf("\n");
}  

void cancelFigure(char *gridPointer,int gridColumns ,int startline ,int startcolumn ,int endline ,int endcolumn , int oldSign ,int newSign){
	//mark all pixels in the given range, that are marked with the label == oldSign, with the label == newSign
	int i,k;
	for (i = startline; i <= endline; i++){
		for (k = startcolumn; k <= endcolumn; k++){
			if (gridPointer[i*gridColumns + k] == oldSign){
				gridPointer[i*gridColumns + k] = newSign;
			} 
		}
	}
}

void processSubgrid(char *gridPointer, int gridLines, int gridColumns, int subgridAbove, int subgridBelow){
    //step from pixel to pixel line-wise
    int i,k,l;
    for (i = 0; i < gridLines; i++){
        for (k = 0; k < gridColumns; k++){
            //check if the pixel is marked with 1 (black, not checked yet)
            if (gridPointer[i * gridColumns + k] == 1){
                //inspect the figure which starts with the current pixel and check if it's a rectangle:
                //informations we need:
                int startcolumn = k; //remians on k
                int endcolumn = k; //might grow
                int startline = i; //remians on i
                int endline = i; //might grow
                int isRectangle = 1; //used as bool. shows if the inspected figute is a rectangle 
                int rectangleChecked = 0; //used as bool. will be set to 1 when the figure is a rectangle and the check is completed 
                
                //get endcolumn of the figure by checking where the figure ends in the startline
                while (endcolumn+1 < gridColumns && gridPointer[i*gridColumns + endcolumn + 1] == 1){
                    endcolumn += 1;
                }

                //every pixel in the line above the startline within the column-range of the figure must be 0. Otherwise it's not a rectangle
                if (startline -1 >= 0){
                    for (l=startcolumn; l<= endcolumn; l++){
                        if (!(gridPointer[(startline -1)*gridColumns + l] == 0)){
                            isRectangle = 0;
                            break;
                        }
                    } 
                } 

                //every pixel in the line below the startline within the column-range of the figure must be 0 or 1. Otherwise it's not a rectangle.
                //if every pixel is a 0 the endline of the figure is the line before the empty line
                //if every pixel is a 1 the pixels before the start column and the pixel after the end column must be a 0. Otherwise it's not a pixel.
                while (isRectangle && !rectangleChecked && (endline+1 < gridLines)){
                    //check for the following line if the line is fully 1 or 0
                    int lineEmpty = 1;
                    for (l = startcolumn; l <= endcolumn; l++){
                        //first column in line defines if line should be empty or full
                        if (l == startcolumn){
                            if (gridPointer[(endline + 1)*gridColumns + l] == 1){
                                lineEmpty = 0;
                            }
                        } else {
                            //check for the following columns of the line.  If conditions for a full or empty line are violated it's not a rectangle.
                            if ((lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 1) || (!lineEmpty && gridPointer[(endline + 1)*gridColumns + l] == 0)){
                                isRectangle = 0;
                                break;
                            }
                        }
                    }                    
                    //if the line is fully 1, check if there is another pixel before startcolumn or after endcolumn. In this case the figure is not a rectangle.
                    if (!lineEmpty){
                        if ((startcolumn-1 >= 0 && gridPointer[(endline+1)*gridColumns + startcolumn-1] == 1)||(endcolumn+1 < gridColumns && gridPointer[(endline+1)*gridColumns + endcolumn + 1] == 1)) {
                            isRectangle = 0;
                            break;
                        } else {
                            //if the line was correct raise the endline of the figure. Loop will continue checking in following line or end if grid-end is reached.
                            endline += 1;
                        }
                    } else {
                        //if the line was fully 0 the upper shape is a rectangle. set flag will end the loop.
                        rectangleChecked = 1;                         
                    }
                }

                //write the result to the grid
                if (!isRectangle){
                    //set all black fields (marked as 1) in the range of the viewed figure to "no rectangle" (marked as 2)
                    cancelFigure(gridPointer,gridColumns,startline,startcolumn,endline,endcolumn,1,2);
                } else {
                    //If the figure ends on a subgrid-end and the original grid continues at this point, it's only a potential rectangle (marked as 4)
                    //Otherwise it's a rectangle
                    if ((subgridAbove && i == 0) || (subgridBelow && endline + 1 == gridLines)) {
                        //mark all black fields (marked as 1) in the range of the viewed figure to "potential rectangle" (marked as 4)
                        cancelFigure(gridPointer,gridColumns,startline,startcolumn,endline,endcolumn,1,4);
                    } else{
                        //mark all black fields (marked as 1) in the range of the viewed figure to "rectangle" (marked as 3)
                        cancelFigure(gridPointer,gridColumns,startline,startcolumn,endline,endcolumn,1,3);
                    }
                }
            }
        }
    }

}

void cancelRectangle(char *gridPointer, int gridColumns, int startline ,int startcolumn, int searchDownward, int newSign){
    //gets the renctangle in the grid and replaces the current sign with the newSign 
    int i,k,endline,endcolumn,oldSign,offset;
    endcolumn = startcolumn;
    endline = startline;
    
    //set offset depending on which direction you want to search
    if (searchDownward) {
        offset = 1;
    } else {
        offset = -1;
    }

    //get current sign
    oldSign = gridPointer[startline*gridColumns + startcolumn];
    //get endcolumn
    while((endcolumn+1 < gridColumns )&& (gridPointer[startline*gridColumns + endcolumn + 1] == oldSign)) {
        endcolumn++;
    }
    //get endline
    while((endline + offset >= 0 ) && (endline + offset < gridColumns ) && (gridPointer[(endline + offset)*gridColumns + startcolumn] == oldSign)) {
        endline += offset;
    }
    //swap endline with startline in case of upward-search
    if (endline < startline){
        int swap = endline;
        endline = startline;
        startline = swap;
    }

    //for the moment: print the stuff ya got
    //printf("startcol: %d endcol: %d startline: %d endline: %d\n",startcolumn,endcolumn,startline,endline);
    for (i=startline; i <= endline; i++){
        for(k=startcolumn;k<=endcolumn;k++){
            gridPointer[i*gridColumns + k] = newSign;
        }
    }

}

void checkPotentialRectangle(char *gridPointer, int gridColumns, int line, int nextBeforeBreakLine, int isUpperLine){
    //check for every potential rectangle: is every pixel in the line opposite the splitpoint also potential rectangle or 0. Otherwise it's not a rectangle.
    //if every pixel is also potential rectangle the pixels in this line before the startcolumn and after the endcolumn of the viewed potential rectangle must be 0. Otherwise it's not a rectangle.
    
    int i;
    //set variable lineOffset depending on if the viewed line is the upper Line of the split point. Defines the check-direction of the function
    int lineOffset;
    if (isUpperLine){
        lineOffset = 1;
    } else {
        lineOffset = -1;
    }

    //go from pixel to pixel in the line to check.
    for (i=0; i < gridColumns; i++){
        //check if the pixel is marked with 4 (potential rectangle)
        if (gridPointer[gridColumns * line + i] == 4){
            int startcolumn = i;
            int endcolumn = i;
            //get endcolumn of the potential rectangle by checking where the figure ends in the line
            while (endcolumn+1 < gridColumns && gridPointer[gridColumns * line + endcolumn + 1] == 4){
                endcolumn += 1;
            }
            
            //check for the opposite line if the line is fully 1 or 0
            int isRectangle = 1;
            int lineEmpty = 1;
            int l;
            for (l=startcolumn; l<= endcolumn; l++){                
                //if following line contains a shape which is not a rectangle the considered shape can't be a rectangle
                if (gridPointer[gridColumns * (line + lineOffset) + l] == 2) {
                    isRectangle = 0;
                    break;
                } else {
                    if (l == i){
                        //if the first column contains a potential rectangle shape the line must be completely filled for the shape beeing a rectangle. Value can just be 4 or 0
                        lineEmpty = (gridPointer[gridColumns * (line + lineOffset) + l] == 0);
                    } else {
                        //for all other columns check if line is completely filled or is completely empty. otherwise considered shape can't be a rectangle
                        if ((lineEmpty && !(gridPointer[gridColumns * (line + lineOffset) + l] == 0)) || (!lineEmpty && !(gridPointer[gridColumns * (line + lineOffset) + l] == 4))) {
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
                }
            }
            //write results to grid
            int potRectEndline;
            int endsBeforNextBreak = 0;
            if(isUpperLine){
                    // wenn wir die obere zeile prüfen, betrachten wir die beschaffenheit der zweiten zeile 
                    // da können noch keine 3en stehetn, die wir anschauen - höchstens 4en oder 0en oder von beidem was 
                    // Wenn lineempty war, lönnen wir gleich ohne bedenken die 3 setzen

                    // wenn 4en drinnen waren muss zeilenweise geprüft werden:
                    //     wenn irgendwann ertses zeichen 0 kann man davon ausgehen, dass rechteck abgeschlossen ist -> 3
                    //     wenn das komplette grid dort zuende ist -> dito: 3

                    //     wenn wir aber die zeile vor dem nächsten bruch erreichen und dort auch noch 4 steht, müssen wir das so stehen lassen
                if (!isRectangle){
                    cancelRectangle(gridPointer,gridColumns,line,i,0,2);
                } else {
                    if (lineEmpty){
                        cancelRectangle(gridPointer,gridColumns,line,i,0,3);
                    } else {
                        potRectEndline = line;
                        while(!endsBeforNextBreak && !(potRectEndline == nextBeforeBreakLine)){
                            if(potRectEndline + 1 == gridColumns){
                                endsBeforNextBreak = 1;
                                break;
                            }
                            if(gridPointer[gridColumns * (potRectEndline + 1) + startcolumn]==4){
                                potRectEndline++;
                            }else{
                                endsBeforNextBreak = 1;
                            }
                        }
                        if(endsBeforNextBreak){
                            cancelRectangle(gridPointer,gridColumns,potRectEndline,i,0,3);
                        } else {
                            i+= endcolumn-startcolumn;
                        }
                    }
                }

            } else {
                    // Wenn wir die untere Zeile prüfen, dann betrachten wir die obere zeile.
                    // da können noch keine 3en stehen, sonst wären die zu prüfenden zeichen auch 3en - da kann stehen: 0 oder 4
                    // ist aber egal, da wir vorher schon geprüft haben ob die zeilen foll 0 oder 4 sind und bei 4 nix davor/dahinter stört
                    // was wir jetzt machen müssen:

                    //     Kann erst sagen, obs ne 3 wird, wenn ich ein rechteck hab, was bei 0 endet

                    //  Wenn es kein rechteck ist, dann können wir nur das potentielle rechteck des Abschnittes löschen, da sonst in dem bereich zu viel gelöscht wird
                    //  also prüfe, wo die figur endet. lösche dann die potentielle Figur bis zum potentiellen entsprechenden Ende mit cancelFigure
                potRectEndline = line;
                while(!endsBeforNextBreak && !(potRectEndline == nextBeforeBreakLine)){
                    if(potRectEndline + 1 == gridColumns){
                        endsBeforNextBreak = 1;
                        break;
                    }
                    if(gridPointer[gridColumns * (potRectEndline + 1) + startcolumn]==4){
                        potRectEndline++;
                    }else{
                        endsBeforNextBreak = 1;
                    }
                }
                if (!isRectangle){
                    if(endsBeforNextBreak){
                        cancelFigure(gridPointer,gridColumns,line,startcolumn,potRectEndline,endcolumn,4,2);
                    }else{
                        cancelFigure(gridPointer,gridColumns,line,startcolumn,nextBeforeBreakLine,endcolumn,4,2);
                    }
                }else{
                    if(endsBeforNextBreak){
                        cancelRectangle(gridPointer,gridColumns,potRectEndline,i,0,3);
                    }else {
                        i+= endcolumn-startcolumn;
                    }
                }

            }            
        }  
    }
}

void outputRectangleList(char *gridPointer,int gridSize){
    int k,i,rectangleCounter;
    int recEndline, recEndcolumn;
    rectangleCounter=0;
    for (k=0; k < gridSize; k++){
        for (i=0; i < gridSize; i++){
            if (gridPointer[k*gridSize + i]==3){
                rectangleCounter++;
                recEndline = k;
                recEndcolumn = i;
                while((recEndline+1 < gridSize)&&(gridPointer[(recEndline+1)*gridSize + i]==3)){
                    recEndline++;
                }
                while ((recEndcolumn+1 < gridSize) && (gridPointer[recEndline*gridSize + recEndcolumn + 1]==3)){
                    recEndcolumn++;
                }
                if ((recEndline == k) && (recEndcolumn == i)){
                    printf("Rectangle %d at %d/%d\n",rectangleCounter,k+1,i+1);
                }else{
                    printf("Rectangle %d from %d/%d to %d/%d\n",rectangleCounter,k+1,i+1, recEndline+1,recEndcolumn+1);
                }
                //cancelFigure(gridPointer,gridSize,gridSize,k,i,5);
                cancelRectangle(gridPointer,gridSize,k,i,1,5);
                //printf("%d/%d\n",k+1,i+1);

            }
        }
    }
}

void writeTimeToFile(double time, double ignore, double maxSubProcTime){
    //for getting , instead of . as numeric seperator
    setlocale(LC_NUMERIC, "de_DE");
    FILE *fp;
	fp = fopen("measures.txt", "a");
    fprintf(fp, "algTime: %f\n",time);

    if (!(ignore == 0 && maxSubProcTime == 0)){
        fprintf(fp, "hereby Ignore: %f\n",ignore);    
        fprintf(fp, "hereby maxSubProcTime: %f\n",ignore);    
        fprintf(fp, "hereby time tithout communication: %f\n",time - ignore + maxSubProcTime);
    } else {
        fprintf(fp, "was the sequential\n");
        fprintf(fp, "hereby time without communication: %f\n",time);
    }
    
    
	fclose(fp);
}


int main(int argc, char** argv) {
    //MPI variables
	int noOfProcs, rank;
    MPI_Status status;
    double subProcTimeMax, subProcTime;

    //Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &noOfProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
        double startTime, endTime, ignore, ignoreTimeStart, ignoreTimeEnd;
		int gridSize;
        char *grid;
        int reciever;
        if (argv[1]){
            FILE *file;
            file = fopen(argv[1], "r");
            if (file) {    
                {//import the grid to a char array
                    printf("Scanning the text-file... \n");
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
                    //Output for overview:
                    printf("Loading the Grid completed.\nTHIS IS THE GRID:\n");
                    printGrid(&grid[0],gridSize);
                }                
                {//processing the grid
                    //Start of the actual algorithm
                    printf("\nStarting the Algorithm. \n");
                    //Start time measurement
                    startTime = MPI_Wtime();
                    if (noOfProcs==1){//run the sequential porcess
                        printf("...as sequential single process\n");
                        processSubgrid(&grid[0],gridSize,gridSize,0,0);
                        printGrid(&grid[0],gridSize);
                        endTime = MPI_Wtime();
                        printf("%f\n",endTime-startTime);
                        writeTimeToFile(endTime - startTime, 0,0);

                    } else { //run the cluster process
                        printf("...with a root process and %d subprocesses\n",noOfProcs-1);

                        //calculation how to split up array for task allocation:
                        div_t gridSegmentSize;
                        int workingProcesses;
                        {
                            if (noOfProcs - 1 > gridSize){
                                gridSegmentSize = div(gridSize,gridSize);
                                workingProcesses = gridSize;
                            } else {
                                gridSegmentSize = div(gridSize,noOfProcs-1);
                                workingProcesses = noOfProcs-1;
                            }
                        }
                        
                        //send the grid size to the sub-processes (necessary for sup-processes, also handles that processes quit if not needed)
                        //for time measurement: ignore the send-time
                        {
                            for (reciever = 1; reciever < noOfProcs; reciever++){
                                ignoreTimeStart = MPI_Wtime();
                                MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
                                ignoreTimeEnd = MPI_Wtime();
                                ignore += ignoreTimeEnd - ignoreTimeStart;
                            } 
                        }

                        //send to all required processes the number of lines of the subgrid and the subgrid itself. 
                        //ignore the send-Time 
                        int subgridLines;
                        {
                            for (reciever = 1; reciever <= workingProcesses ; reciever++){
                                if (reciever == workingProcesses){
                                    subgridLines = gridSegmentSize.quot + gridSegmentSize.rem;
                                    ignoreTimeStart = MPI_Wtime();
                                    MPI_Send(&subgridLines, 1, MPI_INT, reciever, 0, MPI_COMM_WORLD);
                                    ignoreTimeEnd = MPI_Wtime();
                                    ignore += ignoreTimeEnd - ignoreTimeStart;
                                } else {
                                    subgridLines = gridSegmentSize.quot;
                                    ignoreTimeStart = MPI_Wtime();
                                    MPI_Send(&subgridLines, 1, MPI_INT, reciever, 0, MPI_COMM_WORLD);
                                    ignoreTimeEnd = MPI_Wtime();
                                    ignore += ignoreTimeEnd - ignoreTimeStart;
                                }
                                ignoreTimeStart = MPI_Wtime();
                                MPI_Send(&grid[(reciever-1)*gridSize*gridSegmentSize.quot], subgridLines*gridSize, MPI_BYTE, reciever, 0, MPI_COMM_WORLD);
                                ignoreTimeEnd = MPI_Wtime();
                                ignore += ignoreTimeEnd - ignoreTimeStart;
                            }
                        }

                        //recieve done results
                        //ignore the passing time in P0 
                        {
                            //ignore the passing time in P0 
                            ignoreTimeStart = MPI_Wtime();
                            for (reciever = 1; reciever <= workingProcesses ; reciever++){
                                if (reciever == workingProcesses){
                                    subgridLines = gridSegmentSize.quot + gridSegmentSize.rem;
                                } else {
                                    subgridLines = gridSegmentSize.quot;
                                }
                                MPI_Recv(&grid[(reciever-1)*gridSize*gridSegmentSize.quot],subgridLines*gridSize,MPI_BYTE,reciever,0,MPI_COMM_WORLD,&status);
                            }
                            //after recieved all start time again
                            ignoreTimeEnd = MPI_Wtime();
                            ignore += ignoreTimeEnd - ignoreTimeStart;
                        }
                        
                        //Output for overview:
                        printf("THIS IS THE GRID:\n");
                        printGrid(&grid[0],gridSize);
                        
                        //check potential rectangles
                        {
                            int i;
                            for (i = 1; i < workingProcesses; i++){
                                //check break-lines at split-points of the grid
                                //check the potential rectangles in the line above the split-point 

                                //need to input the line before next break -> -0 if none
                                //what is the next line?
                                int nextBeforeBreakLine;
                                if (i+1 < workingProcesses){
                                    nextBeforeBreakLine = (i+1)*gridSegmentSize.quot - 1;
                                } else {
                                    nextBeforeBreakLine = 0;
                                }
                                checkPotentialRectangle(&grid[0],gridSize,i*gridSegmentSize.quot - 1,nextBeforeBreakLine ,1);
                                
                                //check the potential rectangles in the line below the split-point
                                checkPotentialRectangle(&grid[0],gridSize,i*gridSegmentSize.quot,nextBeforeBreakLine ,0);
                                

                            }
                        }
                         
                        printf("THIS IS THE GRID:\n");
                        printGrid(&grid[0],gridSize);
                        printf("\nLegend:\n0: Empty\n2: no Rectangle \n3:rectangle \n(4: potential Rectangle)\n\n");

                        outputRectangleList(&grid[0],gridSize);
                        printGrid(&grid[0],gridSize);
                        
                        endTime = MPI_Wtime();

                        subProcTime = 0;
                        MPI_Reduce(&subProcTime,&subProcTimeMax,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

                        printf("time: %f ignore: %f subpromax %f\n",endTime - startTime, ignore, subProcTimeMax);
                        writeTimeToFile(endTime - startTime, ignore, subProcTimeMax);
                    }
                }                
            } else{ //handle "could not open file" 
                printf("could not open the data-file %s.\n",argv[1]);
                //cancel other processes
                gridSize = 0;
                for (reciever = 1; reciever < noOfProcs; reciever++){
                    MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
                }
            }
        }else{ //handle missing argument
            printf("please run the rectangleChecker with a data-file as parameter.\n");
            //cancel other processes
            gridSize = 0;
            for (reciever = 1; reciever < noOfProcs; reciever++){
                MPI_Send(&gridSize,1,MPI_INT,reciever,0,MPI_COMM_WORLD); 
            }
        }
	} else { //The worker-processes in the cluster process
        //get grid Size
        int subgridLines, subgridColumns;
        MPI_Recv(&subgridColumns,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        
        //just run process if needed.
        if (rank <= subgridColumns){
            double subProcTimeStart, subProcTimeEnd;
            MPI_Recv(&subgridLines,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);

            //set up grid
            //char grid[gridSize[0]*gridSize[1]];
            char *grid;
            grid = malloc(sizeof(char)*subgridLines*subgridColumns);
                        
            //recieve grid array
            MPI_Recv(&grid[0],subgridLines*subgridColumns,MPI_BYTE,0,0,MPI_COMM_WORLD,&status);
            
            //start mesaurement for process
            subProcTimeStart = MPI_Wtime();
            
            //find out if subgrid is above or not
            int subgridAbove = (!(rank == 1));
            int subgridBelow = (!(rank*subgridLines >= subgridColumns));
            
            //process the subgrid
            processSubgrid(&grid[0],subgridLines,subgridColumns,subgridAbove,subgridBelow);
            
            //stop Time-Measurement
            subProcTimeEnd = MPI_Wtime();
            subProcTime = subProcTimeEnd - subProcTimeStart;
            
            //send it back to root process
            //printf("Process %d wanna send grid\n",rank);
            MPI_Send(&grid[0],subgridLines*subgridColumns,MPI_BYTE,0,0,MPI_COMM_WORLD);

            //send used time to Process
            MPI_Reduce(&subProcTime,&subProcTimeMax,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

        } else {
            subProcTime = 0;
            MPI_Reduce(&subProcTime,&subProcTimeMax,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
        }
        
    }
	MPI_Finalize();
}

