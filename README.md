# riscv-v-spec
Working draft of the proposed RISC-V V vector extension.

For the current stable release, v0.8, click [here](https://github.com/riscv/riscv-v-spec/releases/tag/0.8).

For the _previous_ stable release, v0.7.1, click [here](https://github.com/riscv/riscv-v-spec/releases/tag/0.7.1).

The top level file is [v-spec.adoc](./v-spec.adoc)

Simply clicking on the file in the github repo viewer will render a usable
version as markdown.

For a better rendering, use "asciidoctor v-spec.adoc".

This work is licensed under a Creative Commons Attribution 4.0
International License. See the LICENSE file for details.

[[HTML](https://riscv.github.io/documents/riscv-v-spec/)] [[PDF](https://riscv.github.io/documents/riscv-v-spec/riscv-v-spec.pdf)]

### Additional Resources
- Binutils port for v0.8 is not yet available, for v0.7.1 click [here](https://github.com/sifive/riscv-binutils-gdb/tree/2ce33d5584b11454ee2eb250a679888c310c5d18)
- Spike simulator for v0.8 is not yet available, for v0.7.1 click [here](https://github.com/riscv/riscv-isa-sim/tree/49eb5a544864e063975af994f8efe3604b4980ae)
- [riscvOVPsim](https://github.com/riscv/riscv-ovpsim) is a free RISC-V reference simulator on GitHub and for Vectors v0.8 click [here](https://github.com/riscv/riscv-ovpsim), and for v0.7.1 click [here](https://github.com/riscv/riscv-ovpsim).

### Documentation generator

Requirements

`node v6+`

**Linux**: install using [nvm](https://github.com/creationix/nvm)

**OSX**: `brew install node`

**Windows**: [nodejs.org](https://nodejs.org/en/download/)

Install documentation generator

`npm i`

Build HTML/PDF documents

`npm run build`

Resulted files

`public/*`
