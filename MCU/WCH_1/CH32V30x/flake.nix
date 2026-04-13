{
  description = "CH32V30x devShell — WCH MRS GCC15 (riscv32)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Новый путь после переименования
        TOOLCHAIN_BIN = "/home/blank/Engineering/MCU/WCH_1/toolchain/riscv32/bin";
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = [
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

            # Патчим toolchain автоматически
            pkgs.autoPatchelfHook
            pkgs.glibc
            pkgs.zlib
          ];

          # Копируем и патчим toolchain при входе в shell
          shellHook = ''
            echo "========================================"
            echo "CH32V30x devShell — WCH MRS GCC15 (riscv32)"
            echo "========================================"

            TOOLCHAIN_PATH="${TOOLCHAIN_BIN}"

            if [ -d "$TOOLCHAIN_PATH" ]; then
              export PATH="$TOOLCHAIN_PATH:$PATH"
              echo "✓ Toolchain подключён из:"
              echo "   $TOOLCHAIN_PATH"

              if command -v riscv32-wch-elf-gcc >/dev/null 2>&1; then
                echo "   Версия: $(riscv32-wch-elf-gcc --version | head -n1)"
              else
                echo "   ⚠ riscv32-wch-elf-gcc не найден"
              fi
            else
              echo "⚠ Папка toolchain не найдена: $TOOLCHAIN_PATH"
            fi

            export PATH="$PWD:$PATH"

            # CPATH для заголовков SDK
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





# {
#   description = "CH32V30x devShell — WCH MRS Toolchain GCC15";
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
#         # Правильный путь к твоей папке toolchain
#         MRS_ROOT = "$HOME/Engineering/MCU/WCH_1/toolchain";
#       in
#       {
#         devShells.default = pkgs.mkShell {
#           packages = [
#             pkgs.cmake
#             pkgs.ninja
#             pkgs.gdb
#             pkgs.openocd
#             pkgs.minicom
#             pkgs.picocom
#             pkgs.fish
#             pkgs.wlink
#             pkgs.python3Packages.pyserial
#             pkgs.python3Packages.rich
#             pkgs.python3Packages.typer
#           ];
#
#           shellHook = ''
#             echo "========================================"
#             echo "CH32V30x devShell — WCH MRS Toolchain GCC15"
#             echo "========================================"
#
#             TOOLCHAIN_BIN="${MRS_ROOT}/riscv32/bin"
#
#             if [ -x "$TOOLCHAIN_BIN/riscv32-wch-elf-gcc" ]; then
#               export PATH="$TOOLCHAIN_BIN:$PATH"
#               echo "✓ WCH GCC15 успешно подключён:"
#               echo "   $(riscv32-wch-elf-gcc --version | head -n1)"
#               echo "   Путь: $TOOLCHAIN_BIN"
#             else
#               echo "⚠ Toolchain НЕ найден!"
#               echo "   Ожидаемый путь: $TOOLCHAIN_BIN"
#               echo "   Проверь название папок (должно быть точно 'RISC-V Embedded GCC15')"
#               ls "${MRS_ROOT}" 2>/dev/null || echo "Папка ${MRS_ROOT} не существует"
#             fi
#
#             export PATH="$PWD:$PATH"
#
#             # CPATH для заголовков
#             ROOT_DIR="$(pwd)"
#             while [[ ! -f "$ROOT_DIR/flake.nix" && "$ROOT_DIR" != "/" ]]; do
#               ROOT_DIR="$(dirname "$ROOT_DIR")"
#             done
#             if [[ -f "$ROOT_DIR/flake.nix" ]]; then
#               export CPATH="$ROOT_DIR/Inc:$ROOT_DIR/Src:$CPATH"
#             fi
#           '';
#         };
#       });
# }
