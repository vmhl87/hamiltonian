very simple terminal UI game

given a rectangular board (dimensions `11x17`) consisting of empty (gray `[]`) and obstacle (white `##`) cells, try to trace out a hamiltonian path with your cursor (red `<>`)

move either with vim keys (`hjkl`) or arrows. no undos. hit `q` to quit the game

every board is guaranteed to be solvable

compilation requires `ioctl`, `termios`, and ANSI escape codes so this may not work out-of-the-box outside of linux/mac

run either with `make` or compile and run `main.c` manually

have fun!
