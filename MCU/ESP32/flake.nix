{
  description = "ESP-IDF development environment — отдельный flake (fish-friendly)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    espIdf.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs = { self, nixpkgs, espIdf }:
    let
      system = "x86_64-linux";
    in {
      devShells.${system}.default = espIdf.devShells.${system}.esp-idf-full.overrideAttrs (old: {
        shellHook = ''
          echo "========================================"
          echo "ESP-IDF загружен успешно!"
          echo "IDF версия: $(idf.py --version 2>/dev/null || echo 'не определена')"
          echo "Shell: fish"
          echo "========================================"
        '';

        postShellHook = ''
          if test -z "$IDF_PYTHON_ENV_PATH"
            echo "→ Активирую Python virtualenv от ESP-IDF..."
            if test -d "$HOME/.espressif/python_env"
              source "$HOME/.espressif/python_env/bin/activate.fish" 2>/dev/null || true
            else if test -d ".venv"
              source ".venv/bin/activate.fish" 2>/dev/null || true
            end
          end
        '';
      });
    };
}
