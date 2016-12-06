with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  buildInputs = [
    dbus
    lxc
    autoconf
    automake
    pkgconfig
  ];
}
