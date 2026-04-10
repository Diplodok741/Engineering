{
  description = "CH32V30x devShell (riscv32-none-elf)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        riscvToolchain = import nixpkgs {
          inherit system;
          crossSystem = {
            config = "riscv32-none-elf";     
            libc = "newlib";                 
            abi = "ilp32";                
          };
        };
      in
      {
        devShells.default = pkgs.mkShell {
          name = "ch32v30x-riscv32-none-elf";

          packages = [
            riscvToolchain.gcc
            riscvToolchain.binutils
            riscvToolchain.gdb
            pkgs.gdb
            pkgs.openocd
            pkgs.wlink
            pkgs.fish
            pkgs.gnumake
            pkgs.cmake
            pkgs.ninja
            pkgs.clang-tools
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V30x devShell"
            echo "GCC:      $(riscv32-none-elf-gcc --version | head -n1)"
            echo "Binutils: $(riscv32-none-elf-ld --version | head -n1)"
            echo "GDB:      $(riscv32-none-elf-gdb --version | head -n1)"
            echo "Shell:    Fish"
            echo "========================================"
          '';
        };
      });
}



# {
#   description = "CH32V30x devShell";
#
#   inputs = {
#     nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
#     flake-utils.url = "github:numtide/flake-utils";
#   };
#
#   outputs = { self, nixpkgs, flake-utils }:
#     flake-utils.lib.eachDefaultSystem (system:
#       let
#         pkgs = nixpkgs.legacyPackages.${system};
#
#         riscvToolchain = pkgs.pkgsCross.riscv32-embedded;
#       in
#       {
#         devShells.default = pkgs.mkShell {
#           packages = [
#             pkgs.gdb
#             pkgs.openocd
#             pkgs.wlink
#             pkgs.fish
#             riscvToolchain.gcc
#             riscvToolchain.binutils
#             riscvToolchain.gdb
#           ];
#
#           shellHook = ''
#             echo "========================================"
#             echo "CH32V30x devShell"
#             echo "Shell: Fish"
#             echo "GCC: $(riscv32-none-elf-gcc --version | head -n1)"
#             echo "========================================"
#           '';
#         };
#       });
# }
