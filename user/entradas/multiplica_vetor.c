//
// Created by sophie on 25/03/23.
//

int main() {
    int v[5];
    int resultado = 1;
    int i=0;
    int n=5;

    v[0] = 7;
    v[1] = 5;
    v[2] = 33;
    v[3] = 10;
    v[4] = 2;

    for(i=0; i<n; i++){
        resultado = resultado * v[i];
    }
    printf("soma: ");
    printf("%d", resultado);
    return 0;
}