//
// Created by sophie on 02/04/23.
//

int main() {
    int v[8];
    int i=0;
    int aux=0;
    int aux2 = 0;
    int max = 8;

    v[0] =  2;
    v[1] =  9;
    v[2] = 15;
    v[3] = 22;
    v[4] = 30;
    v[5] = 37;
    v[6] = 41;
    v[7] = 90;

    for(i=0;i<4;i++){
        aux = v[i];
        aux2 = v[max - i - 1];
        v[i] = aux2;
        v[max - i - 1] = aux;
    }

    for(i=0; i<max; i++) {
        aux = v[i];
        if(aux%3) {
            printf("%d", aux);
            printf("\n");
        } else {
            printf("MD3\n");
        }
    }

    return 0;
}