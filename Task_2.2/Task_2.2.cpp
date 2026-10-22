#include <iostream>
#include <vector>
#include <mpi.h>

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = 0;
    vector<double> A, B, C;

    if (rank == 0)
    {
        cin >> n;
        A.resize(n * n);
        B.resize(n * n);
        C.resize(n * n);

        for (int i = 0; i < n * n; i++)
            cin >> A[i];

        for (int i = 0; i < n * n; i++)
            cin >> B[i];
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int startRow = rank * n / size;
    int endRow = (rank + 1) * n / size;
    int rows = endRow - startRow;
    vector<double> localA(rows * n);
    vector<double> localC(rows * n);
    vector<double> fullB(n * n);

    if (rank == 0)
    {
        fullB = B;

        for (int p = 1; p < size; p++)
        {
            int first = p * n / size;
            int last = (p + 1) * n / size;
            int processRows = last - first;

            if (processRows > 0)
            {
                MPI_Send(
                    A.data() + first * n,
                    processRows * n,
                    MPI_DOUBLE, p, 0, MPI_COMM_WORLD
                );

                MPI_Send(
                    B.data(),
                    n * n,
                    MPI_DOUBLE, p, 1, MPI_COMM_WORLD
                );
            }
        }

        for (int i = 0; i < rows * n; i++)
            localA[i] = A[startRow * n + i];
    }
    else if (rows > 0)
    {
        MPI_Recv(
            localA.data(), rows * n, MPI_DOUBLE,
            0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );

        MPI_Recv(
            fullB.data(), n * n, MPI_DOUBLE,
            0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double sum = 0.0;

            for (int k = 0; k < n; k++)
            {
                sum += localA[i * n + k] *
                    (rank == 0 ? B[k * n + j]
                        : fullB[k * n + j]);
            }

            localC[i * n + j] = sum;
        }
    }

    if (rank == 0)
    {
        for (int i = 0; i < rows * n; i++)
            C[i] = localC[i];

        for (int p = 1; p < size; p++)
        {
            int first = p * n / size;
            int last = (p + 1) * n / size;
            int processRows = last - first;

            if (processRows > 0)
            {
                MPI_Recv(
                    C.data() + first * n,
                    processRows * n,
                    MPI_DOUBLE, p, 2,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
            }
        }
        for (int i = 0; i < n * n; i++)
        {
            if (i > 0)
                cout << " ";

            cout << C[i];
        }

        cout << endl;
    }
    else if (rows > 0)
    {
        MPI_Send(
            localC.data(), rows * n, MPI_DOUBLE,
            0, 2, MPI_COMM_WORLD
        );
    }

    MPI_Finalize();

    return 0;
}
