{
  description = "Development environment for WCH CH32V microcontrollers";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    flake-parts.url = "github:hercules-ci/flake-parts";
    flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";

    ch32v-nix.url = "github:patryk4815/ch32v-nix";

    ch32v003-src = {
      url = "github:openwch/ch32v003";
      flake = false;
    };

    ch32v20x-src = {
      url = "github:openwch/ch32v20x";
      flake = false;
    };
  };

  outputs = inputs@{ flake-parts, nixpkgs, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {

      systems = [ "x86_64-linux" "aarch64-linux" ];

      perSystem = { pkgs, system, lib, inputs', ... }: let

        cross = pkgs.pkgsCross.riscv32-embedded;

        mkHeaders = src: name: pkgs.stdenvNoCC.mkDerivation {
          pname = "${name}-headers";
          version = "unstable";
          src = src;
          phases = [ "installPhase" ];
          installPhase = ''
            mkdir -p $out/include
            find . -type f -name '*.h' -exec cp --parents {} $out/include \;
          '';
        };

        headers-v003 = mkHeaders inputs.ch32v003-src "ch32v003";
        headers-v20x = mkHeaders inputs.ch32v20x-src "ch32v20x";

      in {

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [ just ];
          shellHook = ''
            echo "Общее окружение CH32V"
            echo "Запусти nix develop .#ch32v203 или nix develop .#ch32v003"
          '';
        };

        devShells.ch32v203 = pkgs.mkShell {
          packages = [
            pkgs.just
            pkgs.bear
            pkgs.clang-tools
            pkgs.gdb
            (inputs'.ch32v-nix.packages.openocd or pkgs.openocd)

            # Только эти два пакета из cross — они минимальные
            cross.gcc
            cross.binutils
          ];

          shellHook = ''
            # Явно добавляем toolchain в PATH
            export PATH="${cross.gcc}/bin:${cross.binutils}/bin:$PATH"

            export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers-v20x}/include"

            echo "CH32V203 dev env готов"
            echo "gcc:      $(which riscv32-none-elf-gcc || echo 'не найден')"
            echo "ld:       $(which riscv32-none-elf-ld  || echo 'не найден')"
            echo "objcopy:  $(which riscv32-none-elf-objcopy || echo 'не найден')"
            echo "gdb:      $(which gdb)"
            echo "openocd:  $(which openocd)"
            echo "CFLAGS:   $CFLAGS"
            echo ""
            echo "Если gcc не найден — попробуйте nix develop --impure + NIXPKGS_ALLOW_UNSUPPORTED_SYSTEM=1"
          '';
        };

        devShells.ch32v003 = pkgs.mkShell {
          packages = [
            pkgs.just
            pkgs.bear
            pkgs.clang-tools
            pkgs.gdb
            (inputs'.ch32v-nix.packages.openocd or pkgs.openocd)

            cross.gcc
            cross.binutils
          ];

          shellHook = ''
            export PATH="${cross.gcc}/bin:${cross.binutils}/bin:$PATH"

            export CFLAGS="-march=rv32ec -mabi=ilp32e -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers-v003}/include"

            echo "CH32V003 dev env готов"
            echo "gcc:      $(which riscv32-none-elf-gcc || echo 'не найден')"
            echo "CFLAGS:   $CFLAGS"
          '';
        };

        formatter = pkgs.nixpkgs-fmt;
      };
    };
}
# {
#   description = "Development environments for WCH CH32V series (RISC-V MCUs)";
#
#   inputs = {
#     nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
#     flake-parts.url = "github:hercules-ci/flake-parts";
#     flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";
#
#     ch32v-nix.url = "github:patryk4815/ch32v-nix";
#
#     # Исходники с заголовками (CMSIS + peripheral)
#     ch32v003-src = {
#       url = "github:openwch/ch32v003";
#       flake = false;
#     };
#
#     ch32v20x-src = {
#       url = "github:openwch/ch32v20x";
#       flake = false;
#     };
#   };
#
#   outputs = inputs@{ flake-parts, nixpkgs, ... }:
#     flake-parts.lib.mkFlake { inherit inputs; } {
#
#       systems = [ "x86_64-linux" "aarch64-linux" ];
#
#       perSystem = { config, self', inputs', pkgs, system, lib, ... }: let
#
#         configuredPkgs = import inputs.nixpkgs {
#             inherit system;
#             config = {
#               allowUnsupportedSystem = true;
#               # allowUnfree = true; если нужно
#             };
#           };
#         # Минимальный cross-toolchain (только gcc + binutils, без лишнего)
#         cross = configuredPkgs.pkgsCross.riscv32-embedded;
#
#         # Хост-пакеты для инструментов, которые запускаются на твоей машине
#         hostPkgs = pkgs;
#
#         # Функция для создания derivation с заголовками
#         mkHeaders = src: name: cross.stdenvNoCC.mkDerivation {
#           pname = "${name}-headers";
#           version = "unstable";
#           src = src;
#           phases = [ "installPhase" ];
#           installPhase = ''
#             mkdir -p $out/include
#             # Копируем все .h файлы из репозитория
#             find . -type f -name '*.h' -exec cp --parents {} $out/include \;
#           '';
#         };
#
#         headers-v003 = mkHeaders inputs.ch32v003-src "ch32v003";
#         headers-v20x = mkHeaders inputs.ch32v20x-src "ch32v20x";
#
#       in {
#
#         devShells.default = cross.mkShell {
#           packages = with pkgs; [ just bear clang-tools ];
#           shellHook = ''
#             echo "Общее окружение CH32V (без toolchain и заголовков)"
#             echo "Используй nix develop .#ch32v203 или .#ch32v003"
#           '';
#         };
#
#         devShells.ch32v203 = pkgs.mkShell {
#           packages = [
#             pkgs.just
#             pkgs.bear
#             pkgs.clang-tools
#             pkgs.gdb               
#             (inputs'.ch32v-nix.packages.openocd or pkgs.openocd)
#
#             cross.gcc
#             cross.binutils
#           ];
#
#           shellHook = ''
#             # Добавляем пути к cross-toolchain в PATH вручную
#             export PATH="${cross.gcc}/bin:${cross.binutils}/bin:$PATH"
#
#             # CFLAGS для твоего чипа
#             export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers-v20x}/include"
#
#             echo "CH32V203 dev env готов (без cross-stdenv)"
#             echo "gcc: $(which riscv32-none-elf-gcc)"
#             echo "ld:  $(which riscv32-none-elf-ld)"
#             echo "gdb: $(which gdb)  # используй gdb -ex 'set architecture riscv:rv32'"
#             echo "openocd: $(which openocd)"
#             echo "CFLAGS: $CFLAGS"
#           '';
#         };
#
#         devShells.ch32v003 = cross.mkShell {
#           buildInputs = with cross; [
#             gcc
#             binutils
#           ];
#
#           nativeBuildInputs = with hostPkgs; [
#             just
#             bear
#             clang-tools
#             gdb-multiarch
#             (inputs'.ch32v-nix.packages.openocd or pkgs.openocd)
#           ];
#
#           CFLAGS = lib.concatStringsSep " " [
#             "-march=rv32ec"
#             "-mabi=ilp32e"
#             "-Os"
#             "-g"
#             "-Wall"
#             "-Wextra"
#             "-ffunction-sections"
#             "-fdata-sections"
#             "-I${headers-v003}/include"
#           ];
#
#           shellHook = ''
#             echo "CH32V003 окружение готово"
#             echo "Заголовки: ${headers-v003}/include"
#             echo "CFLAGS:   $CFLAGS"
#           '';
#         };
#
#         # Опционально: formatter
#         formatter = pkgs.nixpkgs-fmt;
#       };
#     };
# }
# {
#   description = "Development environments for various WCH CH32V microcontrollers";
#
#   inputs = {
#     nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
#     flake-parts.url = "github:hercules-ci/flake-parts";
#     flake-parts.inputs.nixpkgs-lib.follows = "nixpkgs";
#
#     ch32v-nix.url = "github:patryk4815/ch32v-nix";
#   };
#
#   outputs = inputs@{ flake-parts, ... }:
#     flake-parts.lib.mkFlake { inherit inputs; } {
#
#       systems = [ "x86_64-linux" "aarch64-linux" ];
#
#       perSystem = { config, self', inputs', pkgs, system, lib, ... }: let
#         cross = pkgs.pkgsCross.riscv32-embedded;
#         commonPackages = with cross; [
#           gcc
#           binutils
#           gdb
#           openocd   # или из ch32v-nix
#         ];
#       in {
#
#         # devShell по умолчанию — самый общий
#         devShells.default = cross.mkShell {
#           packages = commonPackages ++ (with pkgs; [ just bear clang-tools ]);
#           shellHook = ''
#             echo "Общий CH32V toolchain (riscv32-embedded)"
#           '';
#         };
#
#         # Специфичные shell'ы под семейства
#         devShells.ch32v003 = cross.mkShell {
#           packages = commonPackages;
#           shellHook = ''
#             export CFLAGS="-march=rv32ec -mabi=ilp32e ..."
#             echo "CH32V003 environment ready (very tiny core)"
#           '';
#         };
#
#         devShells.ch32v203 = cross.mkShell {
#           packages = commonPackages;
#           shellHook = ''
#             export CFLAGS="-march=rv32imac -mabi=ilp32 ..."
#             echo "CH32V203 environment ready (144 MHz, USB, CAN...)"
#           '';
#         };
#
#         devShells.ch32v307 = cross.mkShell {
#           packages = commonPackages ++ [ /* может быть дополнительные пакеты */ ];
#           shellHook = ''
#             export CFLAGS="-march=rv32imac -mabi=ilp32 ..."
#             echo "CH32V307 environment ready (high-end, Ethernet possible)"
#           '';
#         };
#
#         # Можно ещё добавить пакеты с примерами прошивок
#         packages = {
#           blink-v203 = cross.callPackage ./firmware/blink { chip = "v203"; };
#         };
#
#         # Алиасы / apps для удобства
#         apps.flash-v203 = {
#           type = "app";
#           program = toString (pkgs.writeShellScript "flash-v203" ''
#             openocd -f interface/wchlink.cfg -f target/ch32v203.cfg -c "program $1 verify reset exit 0x00000000"
#           '');
#         };
#       };
#
#       # Можно импортировать чужие модули (например, treefmt-nix, devenv и т.д.)
#       # imports = [ ... ];
#     };
# }
# {
#   description = "WCH env for Neovim";
#
#   inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
#   inputs.ch32v-nix.url = "github:patryk4815/ch32v-nix";
#
#   outputs = { self, nixpkgs, ch32v-nix, ... }:
#   let
#     system = "x86_64-linux";
#     pkgs = nixpkgs.legacyPackages.${system};
#     cross = pkgs.pkgsCross.riscv32-embedded;
#   in {
#     devShells.${system}.default = cross.mkShell {
#       packages = [
#         pkgs.gdb
#         pkgs.just
#         pkgs.bear
#         pkgs.clang-tools
#         ch32v-nix.packages.${system}.openocd
#         pkgs.fish                  
#       ];
#
#       shellHook = ''
#         exec fish -C "
#           # Алиас compile (работает и с аргументами: compile -j8 и т.д.)
#           # alias compile 'bear -- just -B'
#
#           echo 'CH32V toolchain готовы'
#           echo \"gcc: \$(which riscv32-none-elf-gcc)\"
#           echo 'openocd: nix run github:patryk4815/ch32v-nix'
#         "
#       '';
#     };
#   };
# }
# # {
# #         description = "WCH dev env for Neovim";
# #
# #         inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
# #         inputs.ch32v-nix.url = "github:patryk4815/ch32v-nix";
# #
# #         outputs = {
# #                 self,
# #                 nixpkgs,
# #                 ch32v-nix,
# #                 ...
# #         }:
# #         let
# #                 system = "x86_64-linux";
# #                 pkgs = nixpkgs.legacyPackages.${system};
# #                 cross = pkgs.pkgsCross.riscv32-embedded;
# #         in {
# #                 devShells.${system}.default = cross.mkShell {
# #                         packages = [
# #                                 pkgs.gdb
# #                                 pkgs.just
# #                                 pkgs.bear
# #                                 pkgs.clang-tools
# #                                 ch32v-nix.packages.${system}.openocd
# #                         ];
# #
# #                         shellHook = ''
# #                                 alias compile="bear -- make -B";
# #                                 echo "CH32V toolchain готовы"
# #                                 echo "gcc: $(which riscv32-none-elf-gcc)";
# #                                 echo "openocd: nix run github:patryk4815/ch32v-nix";
# #                         '';
# #                 };
# #         };
# # }
