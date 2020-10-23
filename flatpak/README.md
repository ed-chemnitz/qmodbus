How to compile
--------------

It is assumed to followed the setup instructions from https://flathub.org/setup

```
flatpak-builder --install-deps-from=flathub --force-clean --repo repo app de.edchemnitz.QModbus.yaml
```

Packaging for sharing with others
---------------------------------

```
flatpak build-bundle repo de.edchemnitz.QModbus.flatpak de.edchemnitz.QModbus
```

Install the bundle
------------------

```
flatpak install de.edchemnitz.QModbus.flatpak
```
