{
  description = "CH32V20x / WCH devShell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Специальный cross-пакет с явным soft-float
        crossPkgs = import nixpkgs {
          inherit system;
          crossSystem = {
            config = "riscv32-none-elf";
            libc = "newlib";
            abi = "ilp32";
            # Эти параметры заставляют nixpkgs собрать newlib и libgcc правильно (soft-float)
            fpu = null;
            float-abi = "soft";
          };
        };

        riscvGcc = crossPkgs.buildPackages.gcc;
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
            riscvGcc
            pkgs.python3Packages.pyserial
            pkgs.python3Packages.rich
            pkgs.python3Packages.typer
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V20x devShell (Закос под ESP-IDF)"
            echo "GCC: $(riscv32-none-elf-gcc --version | head -n1)"
            echo "Shell: Fish"
            echo "========================================"

            export PATH="$PWD:$PATH"

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
