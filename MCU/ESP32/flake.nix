{
  description = "ESP-IDF dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    espIdf.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs = { self, nixpkgs, espIdf }:
    let
      system = "x86_64-linux";
    in {
      devShells.${system}.default = espIdf.devShells.${system}.esp-idf-full;
    };
}
