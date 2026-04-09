# {
#   description = "CH32V devShell (V20X + V30X)";
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
#         riscv = pkgs.pkgsCross.riscv32-embedded;
#
#         # Жёсткая обёртка для clangd — отключает host glibc полностью
#         clangd-wrapped = pkgs.writeShellScriptBin "clangd" ''
#           exec ${pkgs.clang-tools}/bin/clangd \
#             --query-driver="${riscv.stdenv.cc}/bin/riscv32-none-elf-gcc" \
#             --background-index \
#             --header-insertion=never \
#             "$@"
#         '';
#       in
#       {
#         devShells.default = pkgs.mkShell {
#           packages = [
#             pkgs.cmake
#             pkgs.ninja
#             pkgs.gdb
#             pkgs.openocd
#             pkgs.wlink
#             riscv.buildPackages.gcc
#             riscv.buildPackages.binutils
#             clangd-wrapped   # ← используем нашу обёртку
#           ];
#
#           shellHook = ''
#             echo "========================================="
#             echo "CH32V devShell запущен (clangd с query-driver)"
#             echo "GCC: $(riscv32-none-elf-gcc --version | head -n1)"
#             echo "========================================="
#
#             # export CPATH="$PWD/Inc/V30X:$PWD/Inc/V20X:$PWD/Src/V30X:$PWD/Src/V20X:$CPATH"
#           '';
#         };
#       });
# }

{
  description = "CH32V30x devShell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        riscvToolchain = pkgs.pkgsCross.riscv32-embedded;
      in
      {
        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.gdb
            pkgs.openocd
            pkgs.wlink
            pkgs.fish
            riscvToolchain.buildPackages.gcc
            riscvToolchain.buildPackages.binutils
            riscvToolchain.buildPackages.gdb
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V30x devShell"
            echo "Shell: Fish"
            echo "GCC: $(riscv32-none-elf-gcc --version | head -n1)"
            echo "========================================"
          '';
        };
      });
}
