{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell 
{
	name = "draweasy";
	buildInputs = with pkgs;
	[
        gtkmm4
        gtk4
        pkg-config
        meson
        ninja
        # hell spawn
        cambalache
        colordiff
        obs-studio
	];
	
	hardeningDisable = ["all"];

    # hooks to setup meson environment variables
    shellHook=''
        export PKG_CONFIG_PATH="${pkgs.gtkmm4.dev}/lib/pkgconfig:${pkgs.gtk4.dev}/lib/pkgconfig:$PKG_CONFIG_PATH"
        export XDG_DATA_DIRS="${pkgs.gtk4}/share/gsettings-schemas/gtk4-4.16.3/glib-2.0/schemas/:$XDG_DATA_DIRS"
        export GSETTINGS_SCHEMA_DIR="${pkgs.gtk4}/share/gsettings-schemas/${pkgs.gtk4.name}/glib-2.0/schemas"

        alias ninja="ninja -j8"
    '';
}
