#include <stdio.h>

class MyClass1 {
public:
    static void myMethod1(int x);
};


class MyClass2 {
public:
    static void myMethod2(const char *ptr);
};

void MyClass2::myMethod2(const char *ptr) {
    fprintf(stderr, "the string: %c\n", ptr[0]);
}

void MyClass1::myMethod1(int x) {
    MyClass2::myMethod2(NULL);
}




int main() {
    MyClass1::myMethod1(32);
}


