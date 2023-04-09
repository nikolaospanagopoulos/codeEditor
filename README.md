# Text editor

This text editor is very similar to vim and is very performant.

## Features

- Read and edit files
- Save and create file if it doesn't exist
- More features are being added
- Very performant.

This text you see here is \*actually- written in Markdown! To get a feel
for Markdown's syntax, type some text into the left window and
watch the results in the right.

## Tech

Dillinger uses a number of open source projects to work properly:

- [AngularJS] - HTML enhanced for web apps!
- [Ace Editor] - awesome web-based text editor
- [markdown-it] - Markdown parser done right. Fast and easy to extend.
- [Twitter Bootstrap] - great UI boilerplate for modern web apps
- [node.js] - evented I/O for the backend
- [Express] - fast node.js network app framework [@tjholowaychuk]
- [Gulp] - the streaming build system
- [Breakdance](https://breakdance.github.io/breakdance/) - HTML
  to Markdown converter
- [jQuery] - duh

And of course Dillinger itself is open source with a [public repository][dill]
on GitHub.

## Installation

This project needs g++ to run and make. C++ 20 is being used.

Download the files, create a folder named "build" and type "make" on your terminal.

```sh
cd codeEditor
make
./bin/main + options
```

Options...

```sh
./bin/main (after editing the empty file you will be prompted to save it)
./bin/main + "filename"
```

## Keys

More keys will be added along with the new features.

| Keys             | Action                                      |
| ---------------- | ------------------------------------------- |
| Arrow keys       | Move                                        |
| PAGE_DOWN        | Move down one page                          |
| PAGE_UP          | Move up one page                            |
| END              | Move to the end of line                     |
| HOME             | Move to the beggining of a line             |
| ENTER, BACKSPACE | As they are being used in every text editor |

## License

MIT

**Free Software, Do what you want with it!!**
