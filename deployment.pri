win32 {
	dlls.path = $$PREFIX
	qt5platforms.path = $${dlls.path}/platforms

	MAIN_DLLS=Qt5Core Qt5Gui Qt5Widgets libpng16-16
	for(dll, MAIN_DLLS): dlls.files += $$[QT_INSTALL_BINS]/$${dll}.dll

	dlls.files += /usr/x86_64-w64-mingw32/lib/zlib1.dll
	dlls.files += /usr/lib/gcc/x86_64-w64-mingw32/6.3-win32/libstdc++-6.dll
	dlls.files += /usr/lib/gcc/x86_64-w64-mingw32/6.3-win32/libgcc_s_seh-1.dll

	# add required Qt plugin DLLs
	qt5platforms.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll

	INSTALLS += dlls qt5platforms
}

target.path = $$PREFIX

INSTALLS += target
