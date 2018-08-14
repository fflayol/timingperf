#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

namespace timing
{
static std::vector<std::pair<std::string, std::function<int(int)>>> benches;
typedef std::pair<std::pair<std::string, std::string>, std::pair<uint64_t, u_int64_t>> OneResult;
static std::vector<OneResult> benchresultses;

static u_int8_t s_timingFunction;
static uint64_t s_number = 10;
double s_ratio = 1;

#ifdef __amd64
__inline__ uint64_t rdtsc()
{
      unsigned cycles_low, cycles_high;
      asm volatile(
          "RDTSC\n\t"
          "mov %%edx, %0\n\t"
          "mov %%eax, %1\n\t"
          : "=r"(cycles_high), "=r"(cycles_low));
      return ((uint64_t)cycles_high << 32) | cycles_low;
}
#elif __i386
__inline__ uint64_t rdtsc()
{
      uint64_t a, d;
      __asm__ volatile("rdtsc"
                       : "=a"(a), "=d"(d));
      return (d << 32) | a;
}
#endif

__inline__ void executeRtc(std::function<int(int)> f, int parameter, uint64_t *total, u_int64_t *min)
{
      uint64_t elapsed_all = 0;
      *min = UINT64_MAX;
      for (int i = s_number; i != 0; i--)
      {
            uint64_t elapsed = rdtsc();
            f(parameter);
            uint64_t elapsed2 = rdtsc();
            uint64_t total = 0;
            if (elapsed2 > elapsed)
                  total = elapsed2 - elapsed;
            else
                  total = elapsed - elapsed2;
            if (total < *min)
                  *min = total;
            elapsed_all += total;
      }

      *total = elapsed_all / s_number;
}

#ifdef __amd64
__inline__ uint64_t rdtscp()
{
      unsigned cycles_low, cycles_high;
      asm volatile(
          "RDTSCP\n\t"
          "mov %%edx, %0\n\t"
          "mov %%eax, %1\n\t"
          : "=r"(cycles_high), "=r"(cycles_low));
      return ((uint64_t)cycles_high << 32) | cycles_low;
}
#elif __i386
__inline__ uint64_t rdtscp()
{
      uint64_t a, d;
      __asm__ volatile("rdtscp"
                       : "=a"(a), "=d"(d));
      return (d << 32) | a;
}
#endif

__inline__ void executeRtcp(std::function<int(int)> f, int parameter, uint64_t *total, u_int64_t *min)
{
      uint64_t elapsed_all = 0;
      *min = UINT64_MAX;
      for (int i = s_number; i != 0; i--)
      {
            uint64_t elapsed = rdtscp();
            f(parameter);
            uint64_t elapsed2 = rdtscp();
            uint64_t total = 0;
            if (elapsed2 > elapsed)
                  total = elapsed2 - elapsed;
            else
                  total = elapsed - elapsed2;
            if (total < *min)
                  *min = total;
            elapsed_all += total;
      }

      *total = elapsed_all / s_number;
}

__inline void setTimingFunction(const u_int8_t timingFunction) { s_timingFunction = timingFunction; }
__inline void setNumberExecution(const u_int64_t number) { s_number = number; }
__inline__ void addFunction(const std::string &name, std::function<int(int)> f)
{
      benches.push_back(std::make_pair(name, f));
}
__inline void clearResults()
{
      benches.clear();
      benchresultses.clear();
}
__inline__ void ExecuteCpp(int parameter)
{
      u_int64_t elapsed;
      u_int64_t min = UINT64_MAX;
      for (auto &bench : benches)
      {
            auto startInitial = std::chrono::high_resolution_clock::now();
            auto endInitial = std::chrono::high_resolution_clock::now();

            double elapsedInitial =
                (double)std::chrono::duration_cast<std::chrono::nanoseconds>(endInitial - startInitial).count();
            std::cout << "call initial" << elapsedInitial << std::endl;
            std::cout << "\033[1;32mBegin [ " << bench.first << " ]\033[0m\n" << std::endl;
            for (int i = s_number; i != 0; i--)
            {
                  auto start = std::chrono::high_resolution_clock::now();
                  bench.second(parameter);
                  auto end = std::chrono::high_resolution_clock::now();
                  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
                  if (min > elapsed)
                        min = elapsed;
                  double elapsedClock =
                      (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000000.0;
                  std::cout << "\033[1;32m End    [ " << bench.first << " \033[0m\n] : " << elapsedClock << " secondes" << std::endl;
            }
            benchresultses.push_back(std::make_pair(make_pair(bench.first, "CHRONO"), std::make_pair(elapsed, min)));
      }
}
__inline__ void ExecuteRtc(int parameter)
{
      u_int64_t min, elapsed;

      for (auto &bench : benches)
      {
            std::cout << "\033[1;32mBegin [ " << bench.first << " ]\033[0m\n";
            executeRtc(bench.second, parameter, &elapsed, &min);
            benchresultses.push_back(std::make_pair(make_pair(bench.first, "RDTSC"), std::make_pair(elapsed, min)));
            std::cout << "\033[1;32mEnd   [ " << bench.first << " ]\033[0m\n";
      }
}

__inline__ void ExecuteRtcp(int parameter)
{
      u_int64_t min, elapsed;

      for (auto &bench : benches)
      {
            std::cout << "\033\[1;32mBegin [ " << bench.first << " ]\033[0m\n";
            executeRtcp(bench.second, parameter, &elapsed, &min);
            benchresultses.push_back(std::make_pair(make_pair(bench.first, "RDTSCP"), std::make_pair(elapsed, min)));
            std::cout << "\033[1;32mEnd [ " << bench.first << " ]\033[0m\n";
      }
}

__inline__ void Execute(int parameter)
{
      switch (s_timingFunction)
      {
      case 0:
            ExecuteRtc(parameter);
            break;
      case 1:
            ExecuteRtcp(parameter);
            break;
      case 2:
            ExecuteRtc(parameter);
            ExecuteRtcp(parameter);
            break;
      case 3:
            ExecuteCpp(parameter);
            break;
      case 99:
            ExecuteRtc(parameter);
            ExecuteRtcp(parameter);
            ExecuteCpp(parameter);
            break;
      default:
            ExecuteCpp(parameter);
            break;
      }
}
int Conversion()
{
      uint64_t elapsed = rdtscp();
      uint64_t elapsed2 = rdtscp();
      auto start = std::chrono::high_resolution_clock::now();
      auto end = std::chrono::high_resolution_clock::now();
      double elapsedInitial = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

      std::cout << "Cycle: " << elapsed2 - elapsed << std::endl;
      std::cout << "Timer  " << elapsedInitial << std::endl;
      s_ratio = elapsedInitial / (elapsed2 - elapsed);
      std::cout << "Ratio  " << s_ratio << std::endl;
}

void CalcResult(void)
{
      OneResult res = benchresultses[0];
      double ref = (double)res.second.first / res.second.first * 100;
      std::cout << "|--------------------------------------------------------------------|\n";
      std::cout << "|---Name--------Timer-----Duration------Diff-------Min-------Diff----|\n";
      std::cout << "|          |           |           |           |         |           |\n";
      for (const auto value : benchresultses)
      {
            std::cout << "\033[0;40;0m|" << std::setw(9) << value.first.first << std::setw(1) << " | " << std::setw(9)
                      << value.first.second << std::setw(1) << " | " << std::setw(9) << value.second.first << std::setw(1)
                      << " | " << std::setw(8) << (double)res.second.first / value.second.first * 100 << std::setw(1) << " %"
                      << std::setw(1) << "|" << std::setw(9) << value.second.second << std::setw(1) << "|" << std::setw(8)
                      << (double)res.second.second / value.second.second * 100 << " % "
                      << "|" << std::setw(1) << " \033[1;30m\n";
      }
      std::cout
          << "|------------------------------------------------------------------------------------------------------|"
          << std::endl;
}
} // namespace timing

