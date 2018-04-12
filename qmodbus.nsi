
############################## Symbol Definitions ##############################
!define APP_NAME "QModBus"
!system 'echo "!define VERSION" $(git describe | sed -e "s/^v//g") > version.nsh'
!include version.nsh
!define COMPANY "EDC Electronic Design Chemnitz GmbH"
!define WEBSITE "http://www.ed-chemnitz.de"
!define COPYRIGHT "© EDC Electronic Design Chemnitz GmbH 2009-2018"
!define DESCRIPTION "Application with modbus RTU slave / TCP client"
!define INSTALLER_NAME "${APP_NAME}-${VERSION}-setup.exe"
!define APP_EXE "${APP_NAME}.exe"
!define REG_ROOT "HKLM"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${APP_EXE}"
!define REG_UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define BINARYDIR "install"


################################## Some Keys ###################################
VIProductVersion  "${VERSION}.0"
VIAddVersionKey   "ProductName" "${APP_NAME}"
VIAddVersionKey   "ProductVersion" "${VERSION}"
VIAddVersionKey   "CompanyName"  "${COMPANY}"
VIAddVersionKey   "CompanyWebsite" "${WEBSITE}"
VIAddVersionKey   "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey   "FileDescription" "${DESCRIPTION}"
VIAddVersionKey   "FileVersion"  "${VERSION}"


############################### Installer Config ###############################
SetCompressor /SOLID Lzma
RequestExecutionLevel admin
Name "${APP_NAME}"
Caption "${APP_NAME} ${VERSION}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME} ${VERSION}"
CRCCheck on
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES64\${APP_NAME}"
ShowInstDetails show
AutoCloseWindow false
ShowUninstDetails show


################################ Include files #################################
!include "Sections.nsh"
!include "MUI.nsh"


######################### Modern User Interface Usage ##########################
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!define MUI_LANGDLL_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_LANGDLL_REGISTRY_KEY "${REG_UNINSTALL_PATH}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"

!insertmacro MUI_RESERVEFILE_LANGDLL


########################### Actual Installer Section ###########################
Section ""
	SetShellVarContext all
	SetOutPath $INSTDIR
	SetOverwrite on
	File /r "${BINARYDIR}\*"
	SetOutPath $INSTDIR
	WriteUninstaller $INSTDIR\uninstall.exe

	CreateDirectory "$SMPROGRAMS\${APP_NAME}"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" $INSTDIR
	CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe" $INSTDIR

	CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" $INSTDIR

	WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${APP_EXE}"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${APP_EXE}"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "Publisher" "${COMPANY}"
	WriteRegStr ${REG_ROOT} "${REG_UNINSTALL_PATH}"  "URLInfoAbout" "${WEBSITE}"
	WriteRegDWORD ${REG_ROOT} "${REG_UNINSTALL_PATH}" "NoModify" 1
	WriteRegDWORD ${REG_ROOT} "${REG_UNINSTALL_PATH}" "NoRepair" 1
SectionEnd


########################## Actual Uninstaller Section ##########################
Section "Uninstall"
	SetShellVarContext all
	RmDir /r "$INSTDIR"

	Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
	Delete "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk"
	RmDir /r "$SMPROGRAMS\${APP_NAME}"

	Delete "$DESKTOP\${APP_NAME}.lnk"
	DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
	DeleteRegKey ${REG_ROOT} "${REG_UNINSTALL_PATH}"
SectionEnd

