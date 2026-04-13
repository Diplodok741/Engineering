{
  description = "CH32V30x devShell + WCH MRS Toolchain GCC15";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        MRS_ROOT = "${builtins.getEnv "HOME"}/Engineering/MCU/WCH_1/CH32V30x/MRS_toolchain";
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
            pkgs.python3Packages.pyserial
            pkgs.python3Packages.rich
            pkgs.python3Packages.typer
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V30x devShell — WCH MRS Toolchain GCC15"
            echo "========================================"

            TOOLCHAIN_BIN="${MRS_ROOT}/RISC-V Embedded GCC15/bin"

            if [ -x "$TOOLCHAIN_BIN/riscv32-wch-elf-gcc" ]; then
              export PATH="$TOOLCHAIN_BIN:$PATH"
              echo "✓ Используется WCH GCC15:"
              echo "   $(riscv32-wch-elf-gcc --version | head -n1)"
              echo "   Путь: $TOOLCHAIN_BIN"
            else
              echo "⚠ Toolchain не найден по пути:"
              echo "   $TOOLCHAIN_BIN"
            fi

            export PATH="$PWD:$PATH"

            # CPATH для SDK
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
