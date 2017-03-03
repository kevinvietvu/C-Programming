/*
 * sudoku_KevinVu_554.c
 *
 *  Created on: Oct 1, 2016
 *      Author: Kevin Vu
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#define GRID_SIZE 9
#define NUM_THREADS 11

typedef struct parameters {
	//starting row
	int row;
	//starting column
	int column;
	//pointer to grid
	int (*grid)[GRID_SIZE];
} parameters;

//print sudoku board
void printGrid(int grid[GRID_SIZE][GRID_SIZE]) {

	int i, j;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			printf("%d", grid[i][j]);
			if (i == 8 && j == 8)
			{
				continue;
			}
			printf(",");
		}
		printf("\n");
	}

}

//create sudoku board
void createGrid(int grid[GRID_SIZE][GRID_SIZE]) {

	int i, j;
	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++)
			grid[i][j] = j + 1;
			

}

//checks each row for duplicates
int checkRows(int grid[GRID_SIZE][GRID_SIZE]) {

	for (int i = 0; i < GRID_SIZE; i++) {
		int checkDuplicates[10] = { 0 };
		for (int j = 0; j < GRID_SIZE; j++) {
			int num = grid[i][j];
			if (checkDuplicates[num] == 0) {
				checkDuplicates[num] = 1;
			} else {
				return 0;
			}
		}
	}
	return 1;
}

//checks each column for duplicates
int checkColumns(int grid[GRID_SIZE][GRID_SIZE]) {

	for (int i = 0; i < GRID_SIZE; i++) {
		int checkDuplicates[10] = { 0 };
		for (int j = 0; j < GRID_SIZE; j++) {
			int num = grid[j][i];
			if (checkDuplicates[num] == 0) {
				checkDuplicates[num] = 1;
			} else {
				return (void*) 0;
			}
		}
	}
	return (void*) 1;
}

//checks each 3x3 box within the grid for duplicates
int checkBoxes(int r, int c, int grid[GRID_SIZE][GRID_SIZE]) {
	int row = r;
	int col = c;

	int checkDuplicates[10] = { 0 };
	for (int i = row; i < row + 3; i++) {
			for (int j = col; j < col + 3; j++) {
				int num = grid[i][j];
				if (checkDuplicates[num] == 0) {
					checkDuplicates[num] = 1;
				} else {
					return (void*) 0;
				}
			}
		}
	return (void*) 1;
}


//Thread runs this function to check sudoku board solution
void *solutionChecker(void *data) {
	struct parameters *my_data = (struct parameters*) data;
	int returnValue;
	int row = (*my_data).row;
	int column = (*my_data).column;

	if (row == 0 && column == 0)
	{
		returnValue = checkBoxes(row, column, (*my_data).grid);
	}
	else if (row == 1 && column == 1)
	{
		returnValue = checkRows((*my_data).grid);
	}
	else if (row == 2 && column == 2)
	{
		returnValue = checkColumns((*my_data).grid);
	}
	else
	{
		returnValue = checkBoxes(row, column, (*my_data).grid);
	}

	free(data);
	pthread_exit(returnValue);
}

int main(void) {
	pthread_t tid[NUM_THREADS];
	pthread_attr_t attr;
	printf("CS149 Sudoku from Kevin Vu\n");

	//creating array to hold return values of solutionChecker
	int* returnValues;
	returnValues = (int*) malloc(NUM_THREADS * sizeof(int));

	//hard coded grid solution
	int grid[GRID_SIZE][GRID_SIZE] = { { 6, 5, 3, 1, 2, 8, 7, 9, 4 }, { 1, 7, 4, 3, 5, 9, 6, 8, 2 },
			{ 9, 2, 8, 4, 6, 7, 5, 3, 1 }, { 2, 8, 6, 5, 1,4, 3, 7, 9 }, { 3, 9, 1, 7, 8, 2, 4, 5, 6 },
			{ 5, 4, 7, 6, 9, 3, 2,1, 8 }, { 8, 6, 5, 2, 3, 1, 9, 4, 7 },
				{ 4, 1, 2, 9, 7, 5, 8, 6, 3 }, { 7, 3, 9, 8, 4, 6, 1, 2, 5 } };

	printGrid(grid);

	//counters for incrementing parameters
	int a = 0; //for top 3 boxes
	int b = 0; //for middle 3 boxes
	int c = 0; //for bottom 3 boxes
	//create array of structs to hold parameters
	struct parameters dataList[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		//parameters for checking boxes(0,3 | 3,6), (0,6 | 3,9)
		if (i > 2 && i < 5)
		{
			dataList[i].row = 0;
			dataList[i].column = a + 3;
			dataList[i].grid = grid;
			a += 3;
		}
		//parameters for checking boxes (3,0 | 6,3), (3,3 | 6,6), (3,6 | 6,9)
		else if (i > 4 && i < 8)
		{
			dataList[i].row = 3;
			dataList[i].column = b;
			dataList[i].grid = grid;
			b += 3;
		}
		//parameters for checking boxes (6,0 | 9,3), (6,3 | 9,6), (6,6 | 9,9)
		else if (i > 7 && i < 11)
		{
			dataList[i].row = 6;
			dataList[i].column = c;
			dataList[i].grid = grid;
			c += 3;
		}
		//parameters for rows,columns and top left box
		else {
		dataList[i].row = i;
		dataList[i].column = i;
		dataList[i].grid = grid;
		}
	}

	//declaring pointer variable to struct data
	struct parameters *data;

	pthread_attr_init(&attr);

	//creating 11 threads
	for (int i = 0; i < NUM_THREADS; i++) {
		//-> used when you have a pointer to a struct and you want to dereference one of the stuct's fields.
		int row = (dataList[i]).row;
		int column = (dataList[i]).column;
		data = (parameters *) malloc(sizeof(parameters));
		data->row = row;
		data->column = column;
		data->grid = grid;
		pthread_create(&tid[i], &attr, solutionChecker, (void*) data);
	}

	//joining threads after completion with return value and adding value to returnValues
	for (int i = 0; i < NUM_THREADS; i++) {
		void * returnValue;
		pthread_join(tid[i], &returnValue);
		returnValues[i] = (int) returnValue;
	}
	
	//checking if return values are all 1 and prints if the sudoku solution is correct or not
	int count = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		if (returnValues[i] == 1) count++;
		else {
			break;
		}
	}

	if (count != 11)
	{
		printf("Puzzle Solution Incorrect\n");
	}
	else
	{
		printf("Puzzle Solution Correct\n");
	}

	free(returnValues);

}


