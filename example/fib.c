int fib(int n)
{
    int i, a, b, c;

    a = 0;
    b = 1;
    i = 0;

    while (i < n)
    {
        c = a + b;
        a = b;
        b = c;

        i = i + 1;
    }

    return a;
}

extern int atoi(const char *);
extern int printf(const char *, ...);

int main(int, char **argv)
{
    char **np = argv + 1;
    char *ns = *np;
    int n = atoi(ns);
    int r = fib(n);
    printf("fib(%d) = %d\x0d\x0a", n, r);
    return r;
}
