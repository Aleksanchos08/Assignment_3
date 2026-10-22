#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm parent;
	MPI_Comm_get_parent(&parent);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (parent != MPI_COMM_NULL)
	{
		cout << "I am " << rank << " process from " << size << " processes!" << endl;
		cout << "My parent is 0." << endl;
		MPI_Comm_disconnect(&parent);
	}
	else {
		int n = 0;
		if (rank == 0) {
			cin >> n;
			if (n <= 0) {
				cerr << "The number of spawned processes must be positive." << endl;
				MPI_Abort(MPI_COMM_WORLD, 1);
			}
		}
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		cout << "I am " << rank << " process from " << size << " processes!" << endl;
		cout << "My parent is none." << endl;
		MPI_Comm children;
		MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, n, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &children, MPI_ERRCODES_IGNORE);
		MPI_Comm_disconnect(&children);
	}
	MPI_Finalize();
	return 0;
}