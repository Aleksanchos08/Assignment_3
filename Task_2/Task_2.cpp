#include <iostream>
#include <mpi.h>
#include <vector>
using namespace std;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int n = 0;
	vector<int> A, B, C;

	if (rank == 0) {
		cout << "Enter the size of the matrices: ";
		cin >> n;
		if (n <= 0 || n % size != 0) {
			cout << "The size of the matrices must be a positive integer and divisible by the number of processes." << endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
	}
	A.resize(n * n);
	B.resize(n * n);
	C.resize(n * n);

	cout << "Enter the elements of matrix A: ";
	for (int i = 0; i < n * n; i++) {
		cin >> A[i];
	}
	cout << "Enter the elements of matrix B: ";
	for (int i = 0; i < n * n; i++) {
		cin >> B[i];
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int rowsPerProcess = n / size;
	int elementsPerProcess = rowsPerProcess * n;

	vector<int> localA(elementsPerProcess);
	vector<int> localB(elementsPerProcess);
	vector<int> fullB(n * n);
	vector<int> localC(elementsPerProcess);

	MPI_Scatter(A.data(), elementsPerProcess, MPI_INT, localA.data(), elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(B.data(), elementsPerProcess, MPI_INT, localB.data(), elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Allgather(localB.data(), elementsPerProcess, MPI_INT, fullB.data(), elementsPerProcess, MPI_INT, MPI_COMM_WORLD);

	for (int i = 0; i < rowsPerProcess; i++) {
		for (int j = 0; j < n; j++) {
			int sum = 0;
			for (int k = 0; k < n; k++) {
				sum += localA[i * n + k] * fullB[k * n + j];
			}
			localC[i * n + j] = sum;
		}
	}
	MPI_Gather(localC.data(), elementsPerProcess, MPI_INT, C.data(), elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "Resultant matrix C: " << endl;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				cout << C[i * n + j] << " ";
			}
			cout << endl;
		}
	}
	MPI_Finalize();
	return 0;
}