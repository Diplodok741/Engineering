{
  description = "CH32V20x devShell (MounRiverStudio toolchain)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        toolchainPath = "/home/blank/Engineering/MCU/WCH/toolchain/riscv32";
      in
      {
        devShells.default = pkgs.mkShell {
          name = "ch32v20x-mrs-devshell";

          packages = with pkgs; [
            cmake
            ninja
            gdb
            wlink
            minicom
            picocom
            python3Packages.pyserial
            python3Packages.rich
            python3Packages.typer
            fish
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V20x devShell (MounRiverStudio)"
            echo "========================================"

            export PATH="${toolchainPath}/bin:$PWD:$PATH"

            if command -v riscv32-wch-elf-gcc >/dev/null 2>&1; then
              echo "✓ GCC : $(riscv32-wch-elf-gcc --version | head -n1)"
            else
              echo "✗ riscv32-wch-elf-gcc не найден!"
            fi

            export CMAKE_PREFIX_PATH="${toolchainPath}"
            export CPATH="$PWD/Inc:$PWD/Src:$CPATH"

            echo "========================================"
            echo "→ Доступные команды:"
            echo "   wdf.py build          # собрать"
            echo "   wdf.py flash          # собрать + прошить"
            echo "   wdf.py monitor        # UART монитор"
            echo "   wdf.py clean"
            echo "========================================"
          '';
        };
      });
}
