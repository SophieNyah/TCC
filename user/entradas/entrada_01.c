//
// Created by sophie on 28/02/23.
//

int fat(int n) {
    int n1 = 0;
    if(n == 1) { return 1; }
    n1 = n-1;
    n1 = n * n1;
    n * n;
    return n * fat(n1);
}

int main() {
    printf("%d\n", fat(5));
    return 0;
}