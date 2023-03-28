//
// Created by sophie on 24/03/23.
//

int main() {
    int n = 9;
    int anterior = 0;
    int resultado = 1;
    int aux = 0;
    int i = 0;

    for(i=0; i<n; i++) {
        aux = anterior + resultado;
        anterior = resultado;
        resultado = aux;
    }

    printf("fibonacci: ");
    printf("%d", resultado);
    return 0;
}
