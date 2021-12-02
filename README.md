# nomsh
A shell written in C.

* Added support for builtin commands.
* Added support for executable commands through fork(), and exec().
* Added support for output redirection using open(), and dup2().

Make preparations:

```
git clone https://github.com/RCReddyN/nomsh.git
cd nomsh
make
```

Start the shell:

`
./shell
`

Remove binary files:

`
make clean
`
