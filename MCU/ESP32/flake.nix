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
        # shellHook будет выполнен в bash (direnv/nix-direnv)
        shellHook = ''
          echo "========================================"
          echo "ESP-IDF загружен успешно!"
          echo "IDF версия: $(idf.py --version 2>/dev/null || echo 'не определена')"
          echo "Shell: fish"
          echo "========================================"

          # Алиасы будут доступны в любой shell
          alias idf-build="idf.py build"
          alias idf-flash="idf.py flash"
          alias idf-monitor="idf.py monitor"
          alias idf-clean="idf.py clean"
          alias idf-menuconfig="idf.py menuconfig"
        '';

        postShellHook = ''
          # Автоматическая активация Python virtualenv ESP-IDF
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
