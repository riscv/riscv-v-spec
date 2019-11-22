# riscv-v-spec
Working draft of the proposed RISC-V V vector extension.

For the most recent stable release, v0.7.1, click [here](https://github.com/riscv/riscv-v-spec/releases/tag/0.7.1).
The corresponding Binutils port is [here](https://github.com/sifive/riscv-binutils-gdb/tree/2ce33d5584b11454ee2eb250a679888c310c5d18),
and the Spike simulator port is [here](https://github.com/riscv/riscv-isa-sim/tree/49eb5a544864e063975af994f8efe3604b4980ae).

The free riscvOVPsim reference simulator from Imperas is available [here on GitHub](https://github.com/riscv/riscv-ovpsim). It is a complete envelope model simulator that covers the full RISC-V specification including the base Vector Extension. It supports all versions of the Vector specification covering the last 12 months and has command line options to select specific versions. It tracks the latest draft specs within about a week of the specs release.

The top level file is [v-spec.adoc](./v-spec.adoc)

Simply clicking on the file in the github repo viewer will render a usable
version as markdown.

For a better rendering, use "asciidoctor v-spec.adoc".

This work is licensed under a Creative Commons Attribution 4.0
International License. See the LICENSE file for details.

[[HTML](https://riscv.github.io/documents/riscv-v-spec/)] [[PDF](https://riscv.github.io/documents/riscv-v-spec/riscv-v-spec.pdf)]

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
