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
        toolchainPath = "/home/blank/Engineering/MCU/WCH/toolchain/riscv32";
      in
      {
        devShells.default = pkgs.mkShell {
          name = "ch32v30x-mrs-devshell";
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
            echo "CH32V30x devShell"
            echo "Shell: Fish"
            echo "Закос под Idf.py"
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
            echo "   wch.py build     # собрать"
            echo "   wch.py flash     # собрать + прошить"
            echo "   wch.py monitor   # UART монитор"
            echo "   wch.py clean"
            echo "========================================"
          '';

          env = {
            CFLAGS = "-march=rv32imacxw -mabi=ilp32 -msave-restore";
          };
        };
      });
}
