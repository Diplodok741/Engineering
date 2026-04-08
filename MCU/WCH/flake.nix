{
  description = "CH32 devShell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Полноценный cross-toolchain с soft-float newlib-nano
        riscvToolchain = import nixpkgs {
          inherit system;
          crossSystem = {
            config = "riscv32-none-elf";
            libc = "newlib-nano";
            abi = "ilp32";          # ← это ключ
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
            pkgs.wlink
            pkgs.wchisp
            pkgs.libusb1
            pkgs.fish

            riscvToolchain.buildPackages.gcc   # главный пакет
            riscvToolchain.buildPackages.binutils
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32 devShell"
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
