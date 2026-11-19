## Test the Code

1. Compile the file(s)
```
g++ perlin_noice.cpp -o rng
```
2. Run the file(s)
```
./rng
```

## Each File's Purpose

#### gaussian\_randomness.cpp
Sums 3 uniform random numbers whose average will always approach a mean of 0 with a std deviation of [-1, 1]
This is the Central Limit Theorem
* 66.7% of results fall within 1 std deviation [-1, 1]
* 95.8% of results fall within [-2, 2]
* 100% of results fall within [-3, 3]
Useful for realistic randomness or aspects that vary across a population IE: simulating height of trees, npc reload speed 

#### filtered\_randomness.cpp
Displays a comparison of randomness to filtered randomness that follows human rules
* Prints random numbers representing coinFlips on the first line with a given seed
  - computer generated randomness may appear to have consistencies like 4 tails in a row
* Prints second line with the same seed that follows one rule
  - any sequences of over 3 flips will manually be swapped
Subjective rule-based rng useful for randomness where perception is more important than fairness IE: critical hits

#### perlin\_noise.cpp
One-Dimensional Perlin Noise smoothly interpolates 'randomly' along a predictable function
* Prints out characters that change in color based on the time and f(x) of the rng function
  - '@' character is printed with additional characters printed until the program ends
  - 256-bit color is displayed according to the height of the Perlin Noise function
* Each Sigmoid (or separate smooth part) of an Octave smooths out to a horizontal line
  - this allows each of them to smoothly blend into each other
* Each Octave increases fine-grained detail in the final signal
  - every subsequent Octave after the first is shrunk on the y-axis and summed with the previous octaves
  - this leads to the smooth function based on the arbitrary y-values that are input
