with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  buildInputs = [
    systemd
    lxc
    autoconf
    automake
    pkgconfig
  ];
}
