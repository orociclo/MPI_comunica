#include "stdafx.h"
#include <iostream>
#include "mpi.h"

/*
El objetivo de este programa es permitir la entrada del usuario a un n�mero,
a continuaci�n, calcular la suma de 1 +2 +3 + .... n�mero introducido por el usuario.
El �ltimo n�mero est� incluido.
Por ejemplo, si el usuario introduce 100, la suma de 1 a 100 incluir� 100.
Este programa se ejecutar� con el mayor n�mero de hilos que pueda o se le diga
que es lo que todas las aplicaciones MPI dise�ados adecuadamente deben hacer.

*/

int main(int argc, char* argv[])
{

	int maxNum;

	int  nTasks, rank, temp;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		printf ("Numero de hilos = %d\n", nTasks);
		do
		{
			std::cout << "Introduzca un numero entre 0 and 50000: ";
			std::cin >> maxNum;
		} while (maxNum < 0 || maxNum > 50000);
	}

	MPI_Barrier(MPI_COMM_WORLD); 

	MPI_Bcast(&maxNum, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//printf("El hilo %d a recibido %d\n", rank, maxNum);

	// Cada hilo s�lo tiene que calcular una parte del problema.
	int portion = maxNum / nTasks;

	int startNum = rank * portion;	// calcular el n�mero de partida de este hilo
	int endNum = (rank + 1) * portion;	// calcular el n�mero en el que termina +1 de este hilo
	if (rank == nTasks - 1)
	{
		/*
		    Si soy el �ltimo hilo, entonces debemos calcular autom�ticamente el n�mero final. 
			Esto es porque cuando nos dividimos MAXNUM por nTasks, podr�a haber quedado un remanente. 
			Mi configuraci�n manual de la serie final, nos aseguramos de que todos los n�meros est�n bien calculadas.
		 */
		endNum = maxNum + 1;
	}
	// Cada hilo calcula su parte de la suma
	int total = 0;
	for (int i = startNum; i < endNum; i++)
	{
		total += i;
	}

	printf("El hilo %d suma de %d \ta %d \t= %d \n", rank, startNum, endNum - 1, total);

	if (rank == 0) // Sii estamos en el hilo 0, se recibe de los demas
	{
		// El hilo principal tendr� que recibir todos los c�lculos de todos los dem�s hilos.
		MPI_Status status;
		                 
		/*
			Tenemos que recibir los datos de todos los otros hilos. 
			La etiqueta arbitraria que elegimos es 1, por ahora.
			MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, 
			int tag, MPI_Comm comm, MPI_Status *status)
		*/

		for (int i = 1; i < nTasks; i++)
		{
			int temp;
			MPI_Recv(&temp, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
			printf("Recibido %d del hilo %d\n", temp, i);
			total += temp;
		}
	}
	else // Si estamos en el hilo que no sea 0, enviaremos al hilo 0.
	{
		/*
			Hemos terminado con los resultados en este hilo, y enviar los datos al hilo 0 con etiqueta 1. 
			El destino es hilo 0, y la etiqueta arbitraria que elegimos por ahora es 1.
			MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
		*/
		MPI_Send(&total, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	if (rank == 0)
	{
		// Muestra el valor final de la suma total
		printf("La suma acumulada de todos los hilos es %d\n", total);
	}

	MPI_Finalize();

	return 0;
}

