#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	const char word[] = "Parallelization";
	const int length = sizeof(word) / sizeof(word[0]) - 1;

	if (size != length) {
		if (rank == 0) {
			cout << "The number of processes must be equal to the length of the word." << endl;
		}
		MPI_Finalize();
		return 0;
	}
	char letter, reversedLetter;
	MPI_Scatter(word, 1, MPI_CHAR, &letter, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	char initialLetters[length];

	MPI_Gather(&letter, 1, MPI_CHAR, initialLetters, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "Initial distribution: " << endl;
		for (int i = 0; i < length; i++) {
			cout << "Process " << i << ": " << initialLetters[i] << endl;
		}
	}
	int partner = size - rank - 1;

	MPI_Sendrecv(&letter, 1, MPI_CHAR, partner, 0, &reversedLetter, 1, MPI_CHAR, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	letter = reversedLetter;
	char reversedLetters[length];

	MPI_Gather(&letter, 1, MPI_CHAR, reversedLetters, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		cout << "Reversed distribution: " << endl;
		for (int i = 0; i < length; i++) {
			cout << "Process " << i << ": " << reversedLetters[i] << endl;
		}
	}
	MPI_Finalize();
	return 0;
}