//
// Created by sophie on 28/02/23.
//

int fat(int n) {
    int n1;
    n1 = n - 1;

    if(n == 1) {
        return 1;
    }

    return n * fat(n1);
}

int main() {
    fat(5);
//    printf("%d\n", fat(5));
    return 0;
}