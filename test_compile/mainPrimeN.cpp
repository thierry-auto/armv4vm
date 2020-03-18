
// C++ program to print all primes smaller than or equal to
// n using Sieve of Eratosthenes
#include <bits/stdc++.h>
using namespace std;

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
extern unsigned char uart_base;

uint32_t SieveOfEratosthenes(int n)
{
    // Create a boolean array "prime[0..n]" and initialize
    // all entries it as true. A value in prime[i] will
    // finally be false if i is Not a prime, else true.
    bool prime[n+1];
    uint32_t lastPrimeNumber;

    memset(prime, true, sizeof(prime));

    for (int p=2; p*p<=n; p++)
    {
        // If prime[p] is not changed, then it is a prime
        if (prime[p] == true)
        {
            // Update all multiples of p greater than or
            // equal to the square of it
            // numbers which are multiple of p and are
            // less than p^2 are already been marked.
            for (int i=p*p; i<=n; i += p)
                prime[i] = false;
        }
    }

    // Print all prime numbers
    for (int p=2; p<=n; p++)  {

        if (prime[p]) {

            lastPrimeNumber = p;
        }
    }

    return lastPrimeNumber;
}

// Driver Program to test above function
int main()
{
    uint32_t result = SieveOfEratosthenes(3000);

    UART_DR(&uart_base) = result;

    return 0;
}
