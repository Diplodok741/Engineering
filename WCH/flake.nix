{
  description = "WCH dev env for Neovim";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  inputs.ch32v-nix.url = "github:patryk4815/ch32v-nix";

  outputs = { self, nixpkgs, ch32v-nix, ... }:
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    cross = pkgs.pkgsCross.riscv32-embedded;
  in {
    devShells.${system}.default = cross.mkShell {
      packages = [
        pkgs.gdb
        pkgs.just
        pkgs.bear
        pkgs.clang-tools
        ch32v-nix.packages.${system}.openocd
        pkgs.fish                  
      ];

      shellHook = ''
        exec fish -C "
          # Алиас compile (работает и с аргументами: compile -j8 и т.д.)
          # alias compile 'bear -- just -B'

          echo 'CH32V toolchain готовы'
          echo \"gcc: \$(which riscv32-none-elf-gcc)\"
          echo 'openocd: nix run github:patryk4815/ch32v-nix'
        "
      '';
    };
  };
}
# {
#         description = "WCH dev env for Neovim";
#
#         inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
#         inputs.ch32v-nix.url = "github:patryk4815/ch32v-nix";
#
#         outputs = {
#                 self,
#                 nixpkgs,
#                 ch32v-nix,
#                 ...
#         }:
#         let
#                 system = "x86_64-linux";
#                 pkgs = nixpkgs.legacyPackages.${system};
#                 cross = pkgs.pkgsCross.riscv32-embedded;
#         in {
#                 devShells.${system}.default = cross.mkShell {
#                         packages = [
#                                 pkgs.gdb
#                                 pkgs.just
#                                 pkgs.bear
#                                 pkgs.clang-tools
#                                 ch32v-nix.packages.${system}.openocd
#                         ];
#
#                         shellHook = ''
#                                 alias compile="bear -- make -B";
#                                 echo "CH32V toolchain готовы"
#                                 echo "gcc: $(which riscv32-none-elf-gcc)";
#                                 echo "openocd: nix run github:patryk4815/ch32v-nix";
#                         '';
#                 };
#         };
# }
