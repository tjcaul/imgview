# imgview

This simple viewer displays a PNG in the terminal
using ANSI colour codes.

## Building
```gcc -O2 -o imgview -lpng *.c```

## Usage
```./imgview file [zoom]```

Where `file` is a path to a PNG file and
`zoom` is optionally and integer.

When `zoom` is omitted, the image is displayed
in the largest size that will fit on the screen.

When `zoom` is included, it specifies the number
of pixels to pack into each display pixel, so
higher zoom actually means a smaller image.
