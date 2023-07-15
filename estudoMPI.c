#include<mpi.h>
#include<stdio.h>
#include<math.h>

double RegraTrapezio(float a_local, float b_local, long double h, int rank){
    float trapezios = (b_local - a_local)/h; // quantos trapézios precisam ser calculados dentro do intervalo
    double integral = sqrt(100.0*100.0 - a_local*a_local) + sqrt(100.0*100.0 - b_local*b_local); //valores dos pontos extremos do intervalo
    float xa = 0.0; // valor inicial para xa
    double fxa = 0.0; //valor inicial para a função em xa
    
    for(int i = 1; i<=(int)trapezios-1; i++){
        xa = a_local+i*h; //calcula o próximo ponto
        fxa = sqrt(100.0*100.0 - xa*xa); //calcula a função no ponto
        integral += 2*fxa; //adiciona duas vezes de acordo com o algoritmo descrito no documento
    }
    integral = h/2.0 *(integral); //cálculo final, multiplicando por h/2
    return integral; //retorna o valor fina do intervalo
}

int main(){
    int rank; //valor para o rank do processo
    int comm; //valor para a quantidade de processos ativados 
    double h = 0.000001; // altura dos trapézios: 0.0001 / 0.00001 / 0.000001
    double integ = 0.0; // valor inicial para a integral local
    float a_geral = 0.0; // a geral
    float b_geral = 100.0; // b geral
    float a_local; // a local de pada processo
    float b_local; // b local de cada processo
    float trapezios_locais; // quantidade de trapézios que cada processo vai calcular
    double inicio; // valor inicial para o tempo
    double final; // valor final para o tempo
    double tempo_max; // tempo do processo mais demorado
    double tempo_local; // tempo local de cada processo
    double resultado = (long double)0.0; //variável para armazenar o resultado final

    MPI_Init(NULL, NULL); //inicial MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //pega o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &comm); //pega a quantidade de processos

    trapezios_locais = ((b_geral - a_geral)/h)/comm; // divide quantidade de trapézios por quantidade de processos
    a_local = a_geral + rank*trapezios_locais*h; // ponto inicial do processo
    b_local = a_local + trapezios_locais*h; // ponto final do processo

    MPI_Barrier(MPI_COMM_WORLD); //espera todos calcularem seus pontos de início e fim
    inicio = MPI_Wtime(); //inicia o tempo do processo

    integ = RegraTrapezio(a_local, b_local, h, rank); // calcula a integral no intervalo do processo

    final = MPI_Wtime(); // finaliza o tempo do processo
    tempo_local = final - inicio; // tempo total do processo

    MPI_Reduce(&tempo_local, &tempo_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); //seleciona o processo mais demorado como tempo total do código

    MPI_Reduce(&integ, &resultado, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // soma todas as integrais na integral final

    if(rank == 0){ //se for o processo root
        __mingw_printf("Tempo de execucao = %f\n", tempo_max); // printa o tempo de execução
        __mingw_printf("Integral resultado = %f\n", resultado); // printa o resultado da integral
    }
    
    MPI_Finalize(); //finaliza MPI

    return 0;

}

