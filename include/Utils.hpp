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

    // Gera o código da tabela diretamente formatado para o arquivo main.tex do relatório
    static void exportarTabelaLatex(const std::string& caminhoArquivo, const std::vector<ResultadoMetodo>& resultados) {
        std::ofstream file(caminhoArquivo);
        if (!file.is_open()) return;

        file << "\\begin{table}[H]\n";
        file << "\\centering\n";
        file << "\\caption{Comparativo de Desempenho dos Métodos Iterativos}\n";
        file << "\\begin{tabular}{|l|c|c|c|c|}\n";
        file << "\\hline\n";
        file << "\\textbf{Método} & \\textbf{Raiz (x)} & \\textbf{f(x)} & \\textbf{Iterações} & \\textbf{Tempo (\\mu s)} \\\\\n";
        file << "\\hline\n";

        for (const auto& r : resultados) {
            file << r.nomeMetodo << " & "
                 << std::fixed << std::setprecision(6) << r.raiz << " & "
                 << std::scientific << std::setprecision(2) << r.fRaiz << " & "
                 << r.iteracoes << " & "
                 << std::fixed << std::setprecision(2) << r.tempoMicrosegundos << " \\\\\n";
        }

        file << "\\hline\n";
        file << "\\end{tabular}\n";
        file << "\\end{table}\n";

        file.close();
    }
};

#endif // UTILS_HPP