# Moore Curve
Fast implementation of finding points of the [Moore curve](https://en.wikipedia.org/wiki/Moore_curve) in `O(n)` using SIMD operations. `n` - number of the points, which is equal to `4^(curve degree)`

# Usage
```
./moore_curve [-V solution] [-B cycles] [-n degree] [-o file] [-AB] [-h]

  -V solution - Solution number
  -B cycles - Number of benchmarking cycles
  -n degree - Moore curve degree
  -o file - Output file name
  -AB - Output the average result for all benchmarks
  -h - Help
```

# Solution
The Moore curve can be expressed by a rewrite system ([L-system](https://en.wikipedia.org/wiki/L-system)):
```
Alphabet: L, R
Constants: F, +, −
Axiom: LFL+F+LFL
Production rules:
L → −RF+LFL+FR−
R → +LF−RFR−FL+
```
We had to replace recursion with an iterative algorithm to develop a faster algorithm than the naive approach. To do this, we must pre-calculate indexes of the first occurrences of the L and R strings for each degree from 1 to `degree - 1`. The following functions are used for this purpose:
```math
l\_commands\_start[i] = \begin{cases}
    n-1-i  \text{, if } n-i \text{ is odd} \\
    r\_commands\_start[i]+commands\_count(i)+2\text{, if } n-i \text{ is even}
    \end{cases}
```
```math
r\_commands\_start[i] = \begin{cases}
    n-1-i \text{, if } n-i \text{ is even} \\
    l\_commands\_start[i]+commands\_count(i)+2\text{, if } n-i \text{ is odd}
    \end{cases}
```
Now, we can directly insert the L and R of each degree into the answer string. In order to get each L and R, we copy the previous L and R. Сopying can be speed up using SIMD operations.

# Benchmarks
<img width="765" alt="Снимок экрана 2024-01-06 в 21 21 36" src="https://github.com/BagritsevichStepan/moore-curve-with-simd/assets/43710058/6ad14b2e-96b0-4212-b090-21dc4792af1c">

The benchmarks were measured for `n` from 1 to 15. A fast algorithm, a naive recursive approach, and a solution using [Gray's code](https://en.wikipedia.org/wiki/Gray_code) were compared. The iterative algorithm with SIMD operations showed the best results.
