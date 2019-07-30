#include "rsa.h"
// #include <time.h>
// #include <math.h>
// #include <stdlib.h>
// #include <stdio.h>
#include "primes_list.h"

#define RAND_MAX 32767 // not quite sure about this

static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;

void rand_seed(void)
{
    // TODO implement this
    z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
}

unsigned int my_rand(void)
{
    unsigned int b;
    b = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & 4294967294U) << 18) ^ b;
    b = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & 4294967288U) << 2) ^ b;
    b = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & 4294967280U) << 7) ^ b;
    b = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & 4294967168U) << 13) ^ b;
    return (z1 ^ z2 ^ z3 ^ z4);
}

// This should totally be in the math library.
long long gcd(long long a, long long b)
{
    long long c;
    while (a != 0)
    {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}

long long ExtEuclid(long long a, long long b)
{
    long long x = 0, y = 1, u = 1, v = 0, gcd = b, m, n, q, r;
    while (a != 0)
    {
        q = gcd / a;
        r = gcd % a;
        m = x - u * q;
        n = y - v * q;
        gcd = a;
        a = r;
        x = u;
        y = v;
        u = m;
        v = n;
    }
    return y;
}

long long rsa_modExp(long long b, long long e, long long m)
{
    if (b < 0 || e < 0 || m <= 0){  // this fails the program somehow why would anyone be less than zero?
      return -1;
    }
    b = b % m;
    if (e == 0)
        return 1;
    if (e == 1)
        return b;
    if (e % 2 == 0)
    {
        return (rsa_modExp(b * b % m, e / 2, m) % m);
    }
    if (e % 2 == 1)
    {
        return (b * rsa_modExp(b, (e - 1), m) % m);
    }
}

// Calling this function will generate a public and private key and store them in the pointers
// it is given.
void rsa_gen_keys(struct public_key_class *pub, struct private_key_class *priv)
{
    // count number of primes in the list
    long long prime_count = sizeof(primes_list) / sizeof(primes_list[0]);

    // choose random primes from the list, store them as p,q
    long long p = 0;
    long long q = 0;

    // long long e = powl(2, 8) + 1; // dk why is this hard coded
    long long e = 257;
    long long d = 0;
    long long max = 0;
    long long phi_max = 0;

    // srand(time(NULL));

    do
    {
        // a and b are the positions of p and q in the list
        int a = (double)my_rand() * (prime_count + 1) / (RAND_MAX + 1.0);
        int b = (double)my_rand() * (prime_count + 1) / (RAND_MAX + 1.0);

        // get p and q
        p = primes_list[a];
        q = primes_list[b];

        max = p * q;
        phi_max = (p - 1) * (q - 1);
    } while (!(p && q) || (p == q) || (gcd(phi_max, e) != 1));

    // Next, we need to choose a,b, so that a*max+b*e = gcd(max,e). We actually only need b
    // here, and in keeping with the usual notation of RSA we'll call it d. We'd also like
    // to make sure we get a representation of d as positive, hence the while loop.
    d = ExtEuclid(phi_max, e);
    while (d < 0)
    {
        d = d + phi_max;
    }

    // printf("primes are %lld and %lld\n",(long long)p, (long long )q);
    // We now store the public / private keys in the appropriate structs
    pub->modulus = max;
    pub->exponent = e;

    priv->modulus = max;
    priv->exponent = d;
}

long long *rsa_encrypt(const char *message, const unsigned long message_size,
                       const struct public_key_class *pub)
{
    // long long *encrypted = malloc(sizeof(long long) * message_size); // maybe find someway to write ifdef
    long long *encrypted;

    long long modExp_result;
    long long i = 0;
    for (i = 0; i < message_size; i++)
    {
        modExp_result = rsa_modExp(message[i], pub->exponent, pub->modulus);
        if (modExp_result == -1) {
            *encrypted = -1;
            return encrypted;
        }
        encrypted[i] = rsa_modExp(message[i], pub->exponent, pub->modulus);rsa_modExp(message[i], pub->exponent, pub->modulus);
    }
    return encrypted;
}

char *rsa_decrypt(const long long *message,
                  const unsigned long message_size,
                  const struct private_key_class *priv)
{
    // char *decrypted = malloc(message_size / sizeof(long long));
    // char *temp = malloc(message_size);
    char *decrypted;
    char *temp;

    long long i = 0;
    long long modExp_result;
    for (i = 0; i < message_size / 8; i++)
    {
        modExp_result = rsa_modExp(message[i], priv->exponent, priv->modulus);
        if (modExp_result == -1) {
            *decrypted = -1;
            return decrypted;
        }
        temp[i] = rsa_modExp(message[i], priv->exponent, priv->modulus);
    }
    // The result should be a number in the char range, which gives back the original byte.
    // We put that into decrypted, then return.
    for (i = 0; i < message_size / 8; i++)
    {
        decrypted[i] = temp[i];
    }
    // free(temp);
    return decrypted;
}
