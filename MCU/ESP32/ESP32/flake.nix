{
  description = "ESP-IDF devShell (независимый)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    espIdf.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs = { self, nixpkgs, espIdf, ... }@inputs:
    let
      system = "x86_64-linux";
    in {
      devShells.${system}.default = espIdf.devShells.${system}.esp-idf-full;

      # Можно добавить несколько вариантов
      # devShells.${system}.esp32 = espIdf.devShells.${system}.esp32-idf;
    };
}
