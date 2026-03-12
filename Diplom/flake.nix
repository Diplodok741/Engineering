{
  description = "Python DSP / communications simulation environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in
  {
    devShells.${system}.default = pkgs.mkShell {

      packages = with pkgs; [
        fish

        (python3.withPackages (ps: with ps; [
          numpy
          scipy
          matplotlib
          jupyter
          ipython
          sympy
          scikit-dsp-comm
        ]))
      ];

      shellHook = ''
        echo "DSP / Communications environment loaded"
        echo "Python tools ready for channel simulations"
        echo "Starting fish shell..."

        exec fish
      '';
    };
  };
}
