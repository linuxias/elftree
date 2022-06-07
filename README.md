[![CircleCI](https://circleci.sec.samsung.net/gh/tizen-sys-con/elftree.svg?style=svg)](https://circleci.sec.samsung.net/gh/tizen-sys-con/elftree)

# elftree

The **ELFtree** is a project to debug ELF's library dependency.
The dependency relationship can be expressed in a tree structure and various information such as ELF Header, Program Header, Section Header, etc.

![elftree](./docs/img/elftree-live-demo.gif)

### Supported Architectures
- armv7l
- aarch64
- i586
- x86_64

### Key interface
* `←`: Fold
* `→`: Unfold
* `↓`: Move down
* `↑`: Move up
* `Page Up`: Scroll up ELF information
* `Page Down`: Scroll down ELF information
* `s`: Displays a list of sections and prints a dump of the section selected by the user.
* `d`: Prints a dump of the entered address by the user.

### Additional Function
#### 1. 's' : View section dump

![elftree-section-dump](./docs/img/elftree_section_dump.png)

#### 2. 'd' : View address dump

![elftree-address-dump](./docs/img/elftree_address_dump.gif)
