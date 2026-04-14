{
  description = "CH32V30x devShell — максимально близко к MRS 2.2.0";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Путь к твоему официальному WCH тулчейну
        toolchainPath = "/home/blank/Engineering/MCU/WCH_1/toolchain/riscv32";
      in
      {
        devShells.default = pkgs.mkShell {
          name = "ch32v30x-mrs-devshell";

          packages = with pkgs; [
            cmake
            ninja
            gdb
            openocd
            minicom
            picocom
            python3Packages.pyserial
            python3Packages.rich
            python3Packages.typer
            fish
          ];

          shellHook = ''
            echo "========================================"
            echo "CH32V30x devShell (MRS-style)"
            echo "========================================"

            # Основной тулчейн
            export PATH="${toolchainPath}/bin:$PWD:$PATH"

            # Проверка тулчейна
            if command -v riscv32-wch-elf-gcc >/dev/null 2>&1; then
              echo "✓ GCC : $(riscv32-wch-elf-gcc --version | head -n1)"
            else
              echo "✗ riscv32-wch-elf-gcc не найден!"
            fi

            echo "✓ wlink : $(command -v wlink || echo 'не найден')"
            echo "✓ wch.py : $(command -v wch.py || echo 'не найден — сделай chmod +x')"

            # Важные переменные для CMake
            export CMAKE_PREFIX_PATH="${toolchainPath}"
            export CPATH="$PWD/Inc:$PWD/Src:$CPATH"

            echo ""
            echo "→ Доступные команды:"
            echo "   wch.py build     # собрать"
            echo "   wch.py flash     # собрать + прошить"
            echo "   wch.py monitor   # UART монитор"
            echo "   wch.py clean"
            echo "========================================"
          '';

          # Эти переменные помогают CMake собирать ближе к MRS
          env = {
            # Можно добавить дополнительные CFLAGS, если нужно
            CFLAGS = "-march=rv32imacxw -mabi=ilp32 -msave-restore";
          };
        };
      });
}



# {
#   description = "CH32V30x devShell — официальный MounRiverStudio toolchain";
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
#         toolchainPath = "/home/blank/Engineering/MCU/WCH_1/toolchain/riscv32";
#       in
#       {
#         devShells.default = pkgs.mkShell {
#           name = "ch32v30x-mrs-devshell";
#
#           packages = [
#             pkgs.cmake
#             pkgs.ninja
#             pkgs.gdb
#             pkgs.openocd
#             pkgs.minicom
#             pkgs.picocom
#             pkgs.wlink
#             pkgs.python3Packages.pyserial
#             pkgs.python3Packages.rich
#             pkgs.python3Packages.typer
#             pkgs.fish
#           ];
#
#           shellHook = ''
#             echo "========================================"
#             echo "CH32V30x devShell (MounRiverStudio toolchain)"
#             echo "========================================"
#
#             # Добавляем официальный тулчейн + текущую директорию (для wch.py)
#             export PATH="${toolchainPath}/bin:$PWD:$PATH"
#
#             # Проверка тулчейна
#             if command -v riscv32-wch-elf-gcc >/dev/null 2>&1; then
#               echo "✓ GCC      : $(riscv32-wch-elf-gcc --version | head -n1)"
#               echo "✓ objcopy  : $(riscv32-wch-elf-objcopy --version | head -n1)"
#             else
#               echo "✗ riscv32-wch-elf-gcc не найден!"
#             fi
#
#             echo "✓ wlink    : $(command -v wlink)"
#             echo "✓ wch.py   : $(command -v wch.py || echo 'не найден — проверь chmod +x')"
#             echo "========================================"
#
#             export CMAKE_PREFIX_PATH="${toolchainPath}"
#             export CPATH="$PWD/Inc:$PWD/Src:$CPATH"
#
#             echo "→ Доступные команды:"
#             echo "   wch.py build          # собрать проект"
#             echo "   wch.py flash          # собрать + прошить"
#             echo "   wch.py monitor        # монитор UART"
#             echo "   wch.py clean          # очистить build"
#             echo "   wch.py size"
#           '';
#         };
#       });
# }
