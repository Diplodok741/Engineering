{
  description = "DSP Python environment on NixOS";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    python = pkgs.python3.withPackages (ps: with ps; [
      numpy
      scipy
      matplotlib
      pandas
      sympy
      ipython
      jax
      jaxlib
    ]);
  in
  {
    devShells.${system}.default = pkgs.mkShell {
      packages = [
        pkgs.fish
        python
      ];

      shellHook = ''
        echo "Специальное окружение с Python для дипломной работы готово"
      '';
    };
  };
}
