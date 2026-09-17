#ifndef UTILS_HPP
#define UTILS_HPP

#include "Tipos.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

class Utils {
public:
    static void imprimirResultadoTerminal(const ResultadoMetodo& res) {
        std::cout << std::left << std::setw(18) << res.nomeMetodo
                  << " | Raiz: " << std::setw(10) << std::fixed << std::setprecision(6) << res.raiz
                  << " | f(x): " << std::setw(12) << std::scientific << res.fRaiz
                  << " | Iter: " << std::setw(4) << res.iteracoes
                  << " | Tempo: " << std::fixed << std::setprecision(2) << res.tempoMicrosegundos << " us\n";
    }
};

#endif // UTILS_HPP