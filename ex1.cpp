#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>
#include "performancetiming.hpp"
int __inline__ fibonacci(int n)
{
      if (n < 3)
            return 1;
      return fibonacci(n - 1) + fibonacci(n - 2);
}

int __inline__ fibonacciOptim(int n)
{
      int first = 0, second = 1, next, c;
      for (c = 0; c < n; c++)
      {
            if (c <= 1)
                  next = c;
            else
            {
                  next = first + second;
                  first = second;
                  second = next;
            }
      }
      return next;
}

int main(int argc, char **argv)
{
      timing::addFunction("FIB", fibonacci);
      timing::addFunction("FIB-OPTIM", fibonacciOptim);
      timing::setTimingFunction(2);
      timing::setNumberExecution(1000000);
      timing::Execute(15);
      timing::CalcResult();
}
