# tpc-compiler

Compiler for a `tpc` language, which is a subset of the `c` language.

This is part of a university compilation course.

How To Use
------

```sh
$ make  # compile
$ ./bin/tpcc < file.tpc
```

Authors
------

Lagneau Gaétan - Pinto Bryan

Rendu Intermédiaire
------

Les options ne sont encore pas implémentées, le programme print donc les
différentes tables de symbole ainsi que l'arbre abstrait par défaut.

Certaines erreurs sémentiques sont implémentées, en particulier les
redéclarations de variables, de structures ou de membres de structure.

