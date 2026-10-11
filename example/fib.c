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

        ++i;
    }

    return a;
}

extern int atoi(const char *);
extern int printf(const char *, ...);

int main(int argc, const char **argv)
{
    if (argc != 2)
        return -1;

    int n = atoi(argv[1]);
    int r = fib(n);

    printf("fib(%d) = %d\r\n", n, r);

    return r;
}
