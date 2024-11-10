with import <nixpkgs> {};
(mkShell.override { stdenv = clangStdenv; }) {
  name = "Hotcart development shell";

  packages = [
    git
    jq
    cmake
    clang
    ninja
    watchexec
    gdb
    xorg.libXext
    python3
    egl-wayland
    libllvm
  ];
  buildInputs = with pkgs; [
    wayland
    libusb
    libunwind
    libxkbcommon
    libffi
    expat
    systemd
  ];

  nativeBuildInputs = with pkgs; [
    xorg.libX11.dev
    xorg.libXft
    xorg.libXinerama
  ];
}
