//
// Created by sophie on 28/02/23.
//

int fat(int n);

int main() {
    int n = 5;
    int f = 0;
    f = fat(n);
    printf("fatorial: ");
    printf("%d", f);
    return 0;
}

int fat(int n) {
    int n1 = n - 1;

    if(n == 1) {
        return 1;
    }

    return n * fat(n1);
}
