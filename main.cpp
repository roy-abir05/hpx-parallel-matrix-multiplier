#include <hpx/algorithm.hpp>
#include <hpx/future.hpp>
#include <hpx/init.hpp>

#include <iostream>
#include <vector>

std::vector<std::vector<int>> A, B, R;
int m1, n1, m2, n2;

bool is_matrix_multiplication_possible()
{
    return n1 == m2;
}

void print_matrix(std::vector<std::vector<int>> &M)
{
    for (auto &i : M)
    {
        for (int j : i)
        {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
}

int hpx_main()
{
    std::cout << "Enter the dimensions of the first matrix: ";
    std::cin >> m1 >> n1;
    A.resize(m1);
    hpx::future<void> A_init = hpx::async([]()
                                          { hpx::experimental::for_loop(hpx::execution::par, 0, m1, [&](auto i)
                                                                        { A[i].resize(n1); }); });

    std::cout << "Enter the dimensions of the second matrix: ";
    std::cin >> m2 >> n2;
    B.resize(m2);
    hpx::future<void> B_init = hpx::async([]()
                                          { hpx::experimental::for_loop(hpx::execution::par, 0, m2, [&](auto i)
                                                                        { B[i].resize(n2); }); });

    if(!is_matrix_multiplication_possible()){
        std::cout << "Matrix multiplication is not possible with the given dimensions" << std::endl;
        return hpx::local::finalize();
    }

    R.resize(m1);
    hpx::future<void> R_init = hpx::async([]()
                                          { hpx::experimental::for_loop(hpx::execution::par, 0, m1, [&](auto i)
                                                                        { R[i].resize(n2); }); });

    auto A_ready = hpx::when_all(A_init).then([&](auto &&)
                                              {
        std::cout << "Enter the elements of first matrix:" << std::endl;
        for(int i=0; i<m1; ++i){
            for(int j=0; j<n1; ++j){
                std::cin >> A[i][j];
            }
        } });

    A_ready.wait();

    auto B_ready = hpx::when_all(B_init).then([&](auto &&)
                                              {
        std::cout << "Enter the elements of first matrix:" << std::endl;
        for(int i=0; i<m2; ++i){
            for(int j=0; j<n2; ++j){
                std::cin >> B[i][j];
            }
        } });

    B_ready.wait();

    R_init.wait();

    // hpx::experimental::for_loop(hpx::execution::par, 0, rowsA, [&](auto i) {
    //     hpx::experimental::for_loop(0, colsB, [&](auto j) {
    //         R[i * colsR + j] = 0;
    //         hpx::experimental::for_loop(0, rowsB, [&](auto k) {
    //             R[i * colsR + j] += A[i * colsA + k] * B[k * colsB + j];
    //         });
    //     });
    // });

    hpx::experimental::for_loop(hpx::execution::par, 0, m1, [&](auto i)
                                { hpx::experimental::for_loop(0, n2, [&](auto j)
                                                              {
            R[i][j] = 0;
            hpx::experimental::for_loop(0, n1, [&](auto k) {
                R[i][j] += A[i][k] * B[k][j];
            }); }); });

    std::cout << "Result Matrix:" << std::endl;
    print_matrix(R);

    return hpx::local::finalize();
}

// hpx::experimental::for_loop(hpx::execution::par, 0, rowsA, [&](auto i) {
//         hpx::experimental::for_loop(0, colsB, [&](auto j) {
//             R[i * colsR + j] = 0;
//             hpx::experimental::for_loop(0, rowsB, [&](auto k) {
//                 R[i * colsR + j] += A[i * colsA + k] * B[k * colsB + j];
//             });
//         });
//     });

int main(int argc, char *argv[])
{
    return hpx::local::init(hpx_main, argc, argv);
}