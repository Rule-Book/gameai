1. Compile the file(s)
```
g++ walking_state.cpp -o walking_state
```
2. Run the file(s)
```
./walking_state
```

## Each File's Purpose

# main.cpp
Connects all 3 FSM classes together to execute transitions between states of wearing and not wearing a shirt.
* FiniteStateMachine
* FSMState
* FSMTransition

# terminal\_gui.cpp
Program to graphically display a moving character within a char\[8\]\[8\] array
* initialPrint
  - prints a *blank* 8x8 array of chars
* movePos
  - moves a pointer in a cardinal direction
* redraw
  - moves terminal cursor to start of printed array
  - clears current line of text
  - redraws array with character 'x' at the new location

# walking\_state.cpp
Connects both above programs by simulating movement through use of a FSM
* Movement State
  - pick a cardinal direction to move in
  - redraw array
  - wait .25s
* Stun State
  - do not move
  - change the character to 'o'
  - conditionally stay *stunned* and display '0' character
  - redraw array and wait .5s
* every time a move is done, there is a 50% chance to be stunned
