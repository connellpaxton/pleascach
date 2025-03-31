{
  description = "Flake for Vulkan and GLSLC Project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            pkgs.git
            pkgs.cmake
            pkgs.vulkan-loader
            pkgs.vulkan-headers
            pkgs.vulkan-tools
            pkgs.vulkan-validation-layers
            pkgs.shaderc
            pkgs.glfw3
          ];
          shellHook = ''
            echo "Vulkan development environment loaded."
          '';
        };
      }
    );
}
