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

        riscvToolchain = import nixpkgs {
          inherit system;
          crossSystem = {
            config = "riscv32-none-elf";
            libc = "newlib-nano";
            abi = "ilp32";          
          };
        };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.cmake
            pkgs.ninja
            pkgs.gdb
            pkgs.openocd
            pkgs.minicom
            pkgs.picocom
            pkgs.fish
            pkgs.wlink
            riscvToolchain.buildPackages.gcc   
            riscvToolchain.buildPackages.binutils
            pkgs.python3Packages.pyserial
            pkgs.python3Packages.rich     
            pkgs.python3Packages.typer   
          ];

          shellHook = ''
            export PATH="$PWD:$PATH"
            echo "========================================"
            echo "CH32V30x devShell"
            echo "Shell: Fish"
            echo "========================================"

            ROOT_DIR="$(pwd)"
            while [[ ! -f "$ROOT_DIR/flake.nix" && "$ROOT_DIR" != "/" ]]; do
              ROOT_DIR="$(dirname "$ROOT_DIR")"
            done

            if [[ -f "$ROOT_DIR/flake.nix" ]]; then
              export CPATH="$ROOT_DIR/Inc:$ROOT_DIR/Src:$CPATH"
            fi
          '';
        };
      });
}
