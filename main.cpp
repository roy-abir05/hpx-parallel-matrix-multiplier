#include <hpx/algorithm.hpp>
#include <hpx/future.hpp>
#include <hpx/init.hpp>
#include <hpx/modules/timing.hpp>

#include <iostream>
#include <vector>

void print_matrix(std::vector<int> &M, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            std::cout << M[i * n + j] << " ";
        }
        std::cout << std::endl;
    }
}

int hpx_main()
{
    std::vector<int> A, B, R;
    int m1, n1, m2, n2;

    std::cout << "Enter the dimensions of the first matrix: ";
    std::cin >> m1 >> n1;
    A.resize(m1 * n1);

    std::cout << "Enter the dimensions of the second matrix: ";
    std::cin >> m2 >> n2;
    B.resize(m2 * n2);

    if (n1 != m2)
    {
        std::cout << "Matrix multiplication is not possible with the given dimensions" << std::endl;
        return hpx::local::finalize();
    }

    R.resize(m1 * n2, 0);


    std::cout << "Enter the elements of first matrix:" << std::endl;
    for (int i = 0; i < m1 * n1; ++i)
    {
        std::cin >> A[i];
    }

    std::cout << "Enter the elements of first matrix:" << std::endl;
    for (int i = 0; i < m2 * n2; ++i)
    {
        std::cin >> B[i];
    }

    hpx::chrono::high_resolution_timer t;

    // Swapped j and k loops for better harwdware prefetching
    hpx::experimental::for_loop(hpx::execution::par, 0, m1, [&](auto i)
                                {
            for(int k=0; k<n1; ++k)
                for(int j=0; j<n2; ++j)
                    R[i * n2 + j] += A[i * n1 + k] * B[k * n2 + j];
    });

    double elapsed = t.elapsed();

    std::cout << "Result Matrix:" << std::endl;
    print_matrix(R, m1, n2);
    std::cout << "Time elapsed: " << elapsed << " seconds\n";

    return hpx::local::finalize();
}

int main(int argc, char *argv[])
{
    return hpx::local::init(hpx_main, argc, argv);
}