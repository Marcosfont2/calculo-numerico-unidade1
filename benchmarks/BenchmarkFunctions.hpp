#ifndef BENCHMARK_FUNCTIONS_HPP
#define BENCHMARK_FUNCTIONS_HPP

#include <cmath>
#include <functional>
#include <string>
#include <vector>

// Representa uma função de referência usada na comparação dos métodos.
struct BenchmarkCase {
    std::string name;
    std::function<double(double)> function;
    double a;
    double b;
    double exactRoot;
};

// Calcula x^2 - 2 para a suíte de benchmark.
// Entrada: valor real x. Saída: valor de x^2 - 2.
inline double squareMinusTwo(double x) {
    return x * x - 2.0;
}

// Calcula x^3 - 0,216 para a suíte de benchmark.
// Entrada: valor real x. Saída: valor de x^3 - 0,216.
inline double cubeRight(double x) {
    return x * x * x - 0.216;
}

// Calcula (1-x)^3 - 0,216 para observar o comportamento simétrico do método.
// Entrada: valor real x. Saída: valor de (1-x)^3 - 0,216.
inline double cubeLeft(double x) {
    const double y = 1.0 - x;
    return y * y * y - 0.216;
}

// Calcula x^2 - 0,81 para um caso que costuma rejeitar a extrapolação.
// Entrada: valor real x. Saída: valor de x^2 - 0,81.
inline double squareRight(double x) {
    return x * x - 0.81;
}

// Calcula cos(x) - x para a suíte de benchmark.
// Entrada: valor real x. Saída: valor de cos(x) - x.
inline double cosineMinusX(double x) {
    return std::cos(x) - x;
}

// Calcula exp(x) - 3 para a suíte de benchmark.
// Entrada: valor real x. Saída: valor de exp(x) - 3.
inline double exponentialMinusThree(double x) {
    return std::exp(x) - 3.0;
}

// Fornece os casos fixos usados pelo benchmark portado do projeto em C.
// Entrada: não recebe argumentos. Saída: casos com intervalo e raiz de referência conhecidos.
inline const std::vector<BenchmarkCase>& benchmarkCases() {
    static const std::vector<BenchmarkCase> cases = {
        {"x^2-2", squareMinusTwo, 1.0, 2.0, 1.4142135623730950488},
        {"x^3-0.216", cubeRight, 0.0, 1.0, 0.6},
        {"(1-x)^3-0.216", cubeLeft, 0.0, 1.0, 0.4},
        {"x^2-0.81", squareRight, 0.0, 1.0, 0.9},
        {"cos(x)-x", cosineMinusX, 0.0, 1.0, 0.7390851332151606417},
        {"exp(x)-3", exponentialMinusThree, 0.0, 2.0, 1.0986122886681096914}
    };
    return cases;
}

#endif // BENCHMARK_FUNCTIONS_HPP
