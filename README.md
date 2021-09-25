# riscv-v-spec
Working draft of the proposed RISC-V V vector extension.

[Version 1.0](https://github.com/riscv/riscv-v-spec/releases/tag/v1.0)
has been frozen and at this time is undergoing public review.
Version 1.0 is considered stable enough to begin developing
toolchains, functional simulators, and implementations, including in
upstream software projects, and is not expected to have incompatible
changes except if serious issues are discovered during
ratification. Once ratified, the spec will be given version 2.0.

The _previous_ stable releases are
[v1.0-rc2](https://github.com/riscv/riscv-v-spec/releases/tag/v1.0-rc2),
[v1.0-rc1](https://github.com/riscv/riscv-v-spec/releases/tag/v1.0-rc1),
[v0.10](https://github.com/riscv/riscv-v-spec/releases/tag/v0.10),
[v0.9](https://github.com/riscv/riscv-v-spec/releases/tag/0.9), and
[v0.8](https://github.com/riscv/riscv-v-spec/releases/tag/0.8).
Note, these previous releases were for experimental development purposes only and are not standard versions suitable for production use. Significant incompatible changes were made from these earlier versions prior to freezing.

The top level file is [v-spec.adoc](./v-spec.adoc)

Simply clicking on the file in the github repo viewer will render a usable
version as markdown.

For a better rendering, use the documentation build process described
below.

This work is licensed under a Creative Commons Attribution 4.0
International License. See the LICENSE file for details.

### Additional Resources

- The [Spike simulator](https://github.com/riscv/riscv-isa-sim) supports v1.0.
- The [RISC-V Proxy Kernel](https://github.com/riscv/riscv-pk)
  (to be used with e.g. Spike) supports v1.0 binaries.
- The [Binutils port for v0.8](https://github.com/riscv/riscv-binutils-gdb/tree/rvv-0.8.x)
- The [GNU toolchain port for v0.8](https://github.com/riscv/riscv-gnu-toolchain/tree/rvv-0.8.x)
- [riscvOVPsim](https://github.com/riscv/riscv-ovpsim) is a free
  RISC-V reference simulator that has support for v0.9, v0.8 and
  v0.7.1 (simulator is under a proprietary license, models are
  open source)

### Documentation generator

Requirements

`node v6+`

**Linux**: install using [nvm](https://github.com/creationix/nvm)

**macOS Homebrew**: `brew install node`

**macOS Macports**: `sudo port install npm6`

**Windows**: [nodejs.org](https://nodejs.org/en/download/)

Install documentation generator

`npm i`

Build HTML/PDF documents

`npm run build`

Resulted files

`public/*`
