# /dev/fibonacci

Just playing around with Linux Kernel Module programming.

## What is this?

A Linux driver for a device which outputs the Fibonacci series.

## Usage

*Disclaimer:* You should probably run this kernel module in a virtual machine,
as it is literally running in kernel-space, and I don't consider my Linux kernel
module programming experience high enough to confidently claim that this won't
wreak complete havoc on your machine. It works on my machine, though.

### Building

Clone this repository, `cd` to it, then run `make`.

### Loading

After building the module, run `insmod fibonacci.ko` with root privileges to
load it.

### Using the device

After loading the module, you can read the Fibonacci series from
`/dev/fibonacci` (you'll probably need root privileges to access the device):

```sh
head -n 12 /dev/fibonacci
```

Output:

```
0
1
1
2
3
5
8
13
21
34
55
89
```

### Unloading

Run `rmmod fibonacci` with root privileges to unload the module.

## Notes

- The `fibonacci` device can only be read from the beginning. Reading from an
  offest results in errno `EINVAL` being returned ("Invalid argument").
- Fibonacci numbers are internally represented as `long long unsigned` and
  output is terminated when an overflow occurs.
