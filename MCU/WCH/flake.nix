{
  description = "WCH CH32V development environment (CH32V003 / CH32V203 / CH32V307)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    flake-utils.url = "github:numtide/flake-utils";

    # Исходники SDK / HAL для CH32V
    ch32v003-src = {
      url = "github:openwch/ch32v003";
      flake = false;
    };
    ch32v20x-src = {
      url = "github:openwch/ch32v20x";
      flake = false;
    };
    ch32v307-src = {
      url = "github:openwch/ch32v307";
      flake = false;
    };
  };

  outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        # Cross-toolchain RISC-V 32bit
        cross = import nixpkgs {
          inherit system;
          crossSystem = pkgs.lib.systems.examples.riscv32-embedded;
        };

        # Функция для сборки SDK: include + src
        mkSDK = src: name: cross.stdenvNoCC.mkDerivation {
          pname = "${name}-sdk";
          version = "unstable";
          src = src;
          phases = [ "installPhase" ];
          installPhase = ''
            mkdir -p $out/include
            mkdir -p $out/src
            find . -type f -name '*.h' -exec cp --parents {} $out/include \;
            find . -type f -name '*.c' -exec cp --parents {} $out/src \;
          '';
        };

        sdk = {
          ch32v003 = mkSDK inputs.ch32v003-src "ch32v003";
          ch32v20x = mkSDK inputs.ch32v20x-src "ch32v20x";
          ch32v307 = mkSDK inputs.ch32v307-src "ch32v307";
        };
      in {

        # ---------------------
        # Общие devShells
        # ---------------------
        devShells.ch32v003 = pkgs.mkShell {
          packages = [
            pkgs.just
            pkgs.bear
            pkgs.clang-tools
            pkgs.gdb
            pkgs.openocd
            cross.buildPackages.gcc
            cross.buildPackages.binutils
          ];

          shellHook = ''
            export MCU=ch32v003
            export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
            export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
            export CFLAGS="-march=rv32ec -mabi=ilp32e -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${sdk.ch32v003}/include"
            export SDK_SRC="${sdk.ch32v003}/src"
            echo "CH32V003 devShell ready"
            echo "CC: $CC"
            echo "CFLAGS: $CFLAGS"
            echo "SDK_SRC: $SDK_SRC"
          '';
        };

        devShells.ch32v203 = pkgs.mkShell {
          packages = [
            pkgs.just
            pkgs.bear
            pkgs.clang-tools
            pkgs.gdb
            pkgs.openocd
            cross.buildPackages.gcc
            cross.buildPackages.binutils
          ];

          shellHook = ''
            export MCU=ch32v203
            export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
            export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
            export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${sdk.ch32v20x}/include"
            export SDK_SRC="${sdk.ch32v20x}/src"
            echo "CH32V203 devShell ready"
            echo "CC: $CC"
            echo "CFLAGS: $CFLAGS"
            echo "SDK_SRC: $SDK_SRC"
          '';
        };

        devShells.ch32v307 = pkgs.mkShell {
          packages = [
            pkgs.just
            pkgs.bear
            pkgs.clang-tools
            pkgs.gdb
            pkgs.openocd
            cross.buildPackages.gcc
            cross.buildPackages.binutils
          ];

          shellHook = ''
            export MCU=ch32v307
            export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
            export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
            export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${sdk.ch32v307}/include"
            export SDK_SRC="${sdk.ch32v307}/src"
            echo "CH32V307 devShell ready"
            echo "CC: $CC"
            echo "CFLAGS: $CFLAGS"
            echo "SDK_SRC: $SDK_SRC"
          '';
        };
      });
}




# {
#   description = "WCH CH32V dev environment without newlib";
#
#   inputs = {
#     nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
#     flake-utils.url = "github:numtide/flake-utils";
#
#     ch32v003-src = {
#       url = "github:openwch/ch32v003";
#       flake = false;
#     };
#     ch32v20x-src = {
#       url = "github:openwch/ch32v20x";
#       flake = false;
#     };
#     ch32v307-src = {
#       url = "github:openwch/ch32v307";
#       flake = false;
#     };
#   };
#
#   outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
#     flake-utils.lib.eachDefaultSystem (system:
#       let
#         pkgs = import nixpkgs { inherit system; };
#
#         # Минимальный cross-toolchain
#         cross = import nixpkgs {
#           inherit system;
#           crossSystem = pkgs.lib.systems.examples.riscv32-embedded;
#         };
#
#         mkHeaders = src: name: cross.stdenvNoCC.mkDerivation {
#           pname = "${name}-headers";
#           version = "unstable";
#           src = src;
#           phases = [ "installPhase" ];
#           installPhase = ''
#             mkdir -p $out/include
#             find . -type f -name '*.h' -exec cp --parents {} $out/include \;
#           '';
#         };
#
#         headers = {
#           ch32v003 = mkHeaders inputs.ch32v003-src "ch32v003";
#           ch32v20x = mkHeaders inputs.ch32v20x-src "ch32v20x";
#           ch32v307 = mkHeaders inputs.ch32v307-src "ch32v307";
#         };
#       in {
#
#         devShells.ch32v003 = pkgs.mkShell {
#           packages = [
#             pkgs.just
#             pkgs.bear
#             pkgs.clang-tools
#             pkgs.gdb
#             pkgs.openocd
#             cross.buildPackages.gcc
#             cross.buildPackages.binutils
#           ];
#
#           shellHook = ''
#             MCU=ch32v003
#             export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
#             export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
#             export CFLAGS="-march=rv32ec -mabi=ilp32e -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers.ch32v003}/include"
#             echo "CH32V003 devShell ready"
#             echo "CC: $CC"
#             echo "CFLAGS: $CFLAGS"
#           '';
#         };
#
#         devShells.ch32v203 = pkgs.mkShell {
#           packages = [
#             pkgs.just
#             pkgs.bear
#             pkgs.clang-tools
#             pkgs.gdb
#             pkgs.openocd
#             cross.buildPackages.gcc
#             cross.buildPackages.binutils
#           ];
#
#           shellHook = ''
#             MCU=ch32v203
#             export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
#             export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
#             export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers.ch32v20x}/include"
#             echo "CH32V203 devShell ready"
#             echo "CC: $CC"
#             echo "CFLAGS: $CFLAGS"
#           '';
#         };
#
#         devShells.ch32v307 = pkgs.mkShell {
#           packages = [
#             pkgs.just
#             pkgs.bear
#             pkgs.clang-tools
#             pkgs.gdb
#             pkgs.openocd
#             cross.buildPackages.gcc
#             cross.buildPackages.binutils
#           ];
#
#           shellHook = ''
#             MCU=ch32v307
#             export PATH="${cross.buildPackages.gcc}/bin:${cross.buildPackages.binutils}/bin:$PATH"
#             export CC=$(basename $(find ${cross.buildPackages.gcc}/bin -name "*-gcc" | head -n1))
#             export CFLAGS="-march=rv32imac -mabi=ilp32 -mcmodel=medany -Os -g -Wall -Wextra -ffunction-sections -fdata-sections -I${headers.ch32v307}/include"
#             echo "CH32V307 devShell ready"
#             echo "CC: $CC"
#             echo "CFLAGS: $CFLAGS"
#           '';
#         };
#       });
# }
