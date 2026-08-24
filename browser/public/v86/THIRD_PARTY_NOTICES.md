# Third-party emulator assets

The browser build uses the official [v86](https://github.com/copy/v86) package
and its documented BIOS setup.

- v86 is distributed under the BSD 2-Clause license; see the package's
  `node_modules/v86/LICENSE` after installation.
- SeaBIOS and VGA BIOS binaries were obtained from the official v86 repository.
  Their corresponding source and license information are available in the
  [v86 BIOS directory](https://github.com/copy/v86/tree/master/bios) and the
  linked upstream projects.

No emulator code implements MiniOS scheduling. It only provides the virtual x86
hardware on which the kernel runs.
