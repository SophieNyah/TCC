//
// Created by sophie on 24/03/23.
//

int fib(int n);

int main() {
    int n = 9;
//    int anterior = 0;
    int resultado = 0;
//    int aux = 0;
//    int i = 0;

    printf("Insira o número: ");
    scanf("%d", &n);
    resultado = fib(n);

//    for(i=0; i<n; i++) {
//        aux = anterior + resultado;
//        anterior = resultado;
//        resultado = aux;
//    }

    printf("fibonacci: ");
    printf("%d", resultado);
    return 0;
}

int fib(int n) {
    int n1 = n - 1;
    int n2 = n - 2;
    int fib1 = 0;
    int fib2 = 0;
    if(n<=1) { return n; }
    fib1 = fib(n1);
    fib2 = fib(n2);
    return fib1 + fib2;
}
