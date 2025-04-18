## Vangke Modifnya-Kan

Programming Language

<img src="img/vmk.png" alt="vmk" width="200"/>

VMK (vi·em·kah) is a [Lua](https://github.com/lua/lua) Dialect Programming
language, forked from [5.4](https://www.lua.org/manual/5.4/readme.html#changes)
Version. This modification has been made to simplify learning, writing, and
personalizing the language for my own use.

For now, three changes have been implemented:

- The reserved word `local` has been replaced with `own`.
- The reserved word `function` has been replaced with `fn`.
- The standard library namespace `string` has been renamed to `str` (e.g.,
  `string.upper("vmk")` → `str.upper("vmk")`).

Other than these changes, everything remains identical to the original Lua
language.

---

## Installation

1. Clone the repository:
   ```bash
   git clone https://codeberg.org/nginrsw/vmk.git
   cd vmk
   ```

2. Build the program by running `make` in your terminal:
   ```bash
   make
   ```

3. After building, create a symbolic link to install the program:
   ```bash
   sudo ln -s /path/to/program/vmk/src/vmk /usr/local/bin/
   ```

   Replace `/path/to/program` with the actual path to where the compiled program
   resides.

Once this is done, you can run the `vmk` command from anywhere in your terminal!

For example, create a file named `file.vmk`, type `io.write("Hello VMK\n")`
inside it, and run it with the following command:

```bash
vmk file.vmk
```

---

## Transpiling Between Lua and VMK

If you wish to transpile code from Lua to VMK or from VMK to Lua, I’ve prepared
executable files located in the `transpiler` directory.

You can easily run these using the following commands:

- **From Lua to VMK**, just run `lua2vmk` inside current directory:
  ```bash
  ./lua2vmk
  ```

- **From VMK to Lua**, it's similar, run `vmk2lua` inside of ur current
  directory:
  ```bash
  ./vmk2lua
  ```

These commands will automatically convert all of your code between the two
languages, making it easier to switch between them.

---

## Learn VMK in 15 Minutes

If you're new to VMK and want a quick overview of how to write code in this language, a brief hands-on guide is available. You can follow the “Learn VMK in 15 Minutes” tutorial located in the [Learn VMK in 15 Minutes](Learn%20VMK%20in%2015%20Minutes/) directory.

This tutorial provides a concise and practical introduction to VMK, allowing you to get familiar with its syntax and basic concepts in no time. You can follow the tutorial offline directly from this directory by opening the `Learn Vmk in 15 Minutes.html` file in your browser.

For example, you can run the following command in your terminal:

```bash
chromium 'Learn Vmk in 15 Minutes.html'
```

Feel free to replace `chromium` with your preferred browser command, like `firefox` or `brave`.
