#ifndef FALSA_POSICAO_HPP
#define FALSA_POSICAO_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class FalsaPosicao {
    public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        double a_in,
        double b_in,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Falsa Posicao";
        res.intervaloFinal = {a_in, b_in};
        const auto inicio = std::chrono::high_resolution_clock::now();
        const auto finalizar = [&res, &inicio]() {
            const auto fim = std::chrono::high_resolution_clock::now();
            res.tempoMicrosegundos = std::chrono::duration<double, std::micro>(fim - inicio).count();
            return res;
        };
        const auto avaliar = [&f, &res](double x, double& valor) {
            ++res.avaliacoesFuncao;
            valor = f(x);
            return std::isfinite(valor);
        };
        const auto sinaisOpostos = [](double esquerda, double direita) {
            return (esquerda < 0.0 && direita > 0.0) || (esquerda > 0.0 && direita < 0.0);
        };

        if (!f || !std::isfinite(a_in) || !std::isfinite(b_in) || !std::isfinite(tol) || tol < 0.0 || maxIter <= 0) {
            res.status = MetodoStatus::ArgumentoInvalido;
            res.mensagemErro = "Funcao, intervalo, tolerancia ou maximo de iteracoes invalido";
            return finalizar();
        }
        if (!(a_in < b_in)) {
            res.status = MetodoStatus::IntervaloInvalido;
            res.mensagemErro = "O intervalo deve obedecer a < b";
            return finalizar();
        }

        double a = a_in;
        double b = b_in;
        double fa;
        double fb;
        if (!avaliar(a, fa) || !avaliar(b, fb)) {
            res.status = MetodoStatus::AvaliacaoNaoFinita;
            res.mensagemErro = "A funcao gerou um valor nao finito";
            return finalizar();
        }
        if (fa == 0.0 || fb == 0.0) {
            res.raiz = fa == 0.0 ? a : b;
            res.fRaiz = fa == 0.0 ? fa : fb;
            res.intervaloFinal = {res.raiz, res.raiz};
            res.status = MetodoStatus::RaizExata;
            res.convergiu = true;
            return finalizar();
        }
        if (!sinaisOpostos(fa, fb)) {
            res.raiz = a;
            res.fRaiz = fa;
            res.status = MetodoStatus::SemMudancaDeSinal;
            res.mensagemErro = "Nao existe mudanca de sinal nas extremidades";
            return finalizar();
        }

        double atual = a;
        double fAtual = fa;
        for (int iteracao = 1; iteracao <= maxIter; ++iteracao) {
            const double denominador = fb - fa;
            if (!std::isfinite(denominador) || denominador == 0.0) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.intervaloFinal = {a, b};
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "Denominador nulo ou nao finito na interpolacao";
                return finalizar();
            }
            const double proximo = (a * fb - b * fa) / denominador;
            if (!std::isfinite(proximo)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.intervaloFinal = {a, b};
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "A interpolacao gerou um valor nao finito";
                return finalizar();
            }
            double fProximo;
            if (!avaliar(proximo, fProximo)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.intervaloFinal = {a, b};
                res.status = MetodoStatus::AvaliacaoNaoFinita;
                res.mensagemErro = "A funcao gerou um valor nao finito";
                return finalizar();
            }

            res.raiz = proximo;
            res.fRaiz = fProximo;
            res.iteracoes = iteracao;
            if (fProximo == 0.0) {
                res.intervaloFinal = {proximo, proximo};
                res.status = MetodoStatus::RaizExata;
                res.convergiu = true;
                return finalizar();
            }
            if (std::abs(fProximo) <= tol) {
                res.intervaloFinal = {a, b};
                res.status = MetodoStatus::ToleranciaFuncao;
                res.convergiu = true;
                return finalizar();
            }
            if (iteracao > 1 && std::abs(proximo - atual) <= tol) {
                res.intervaloFinal = {a, b};
                res.status = MetodoStatus::ToleranciaPasso;
                res.convergiu = true;
                return finalizar();
            }

            if (sinaisOpostos(fa, fProximo)) {
                b = proximo;
                fb = fProximo;
            } else {
                a = proximo;
                fa = fProximo;
            }
            res.intervaloFinal = {a, b};
            if (b - a <= tol) {
                res.status = MetodoStatus::ToleranciaIntervalo;
                res.convergiu = true;
                return finalizar();
            }
            atual = proximo;
            fAtual = fProximo;
        }

        res.raiz = atual;
        res.fRaiz = fAtual;
        res.intervaloFinal = {a, b};
        res.iteracoes = maxIter;
        res.status = MetodoStatus::MaximoDeIteracoes;
        res.mensagemErro = "Numero maximo de iteracoes atingido sem convergencia";
        return finalizar();
    }
};

#endif // FALSA_POSICAO_HPP
