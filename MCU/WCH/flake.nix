{
  description = "WCH CH32V203 + CH32V307 dev environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      cross = pkgs.pkgsCross.riscv64-embedded;
    in {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          cross.stdenv.cc
          pkgs.gnumake
          pkgs.libusb1
          pkgs.pkg-config
          pkgs.clang-tools   # для clangd + LSP в Neovim
        ];

        shellHook = ''
          echo "=== CH32V203 / CH32V307 готов к работе ==="
          echo "riscv64-none-elf-gcc: $(riscv64-none-elf-gcc --version | head -1)"
          alias flash='minichlink -w'
          alias flashb='minichlink -w -b'   # flash + reset
        '';
      };
    };
}
