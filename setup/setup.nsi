!include MUI2.nsh
!include FileFunc.nsh
!include nsProcess.nsh

!define APPNAME "APK Icon Editor"
!define COMPANYNAME "Alexander Gorishnyak"
!define VERSIONMAJOR 1
!define VERSIONMINOR 7
!define VERSIONPATCH 1
!define VERSION "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONPATCH}"
!define ABOUTURL "http://kefir500.github.io/apk-icon-editor"
!define UPDATEURL "http://kefir500.github.io/apk-icon-editor/download/#update"
!define EXE "$INSTDIR\apk-icon-editor.exe"

Name "${APPNAME} v${VERSION}"
OutFile "apk-icon-editor_${VERSION}.exe"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKCU "Software\apk-icon-editor" "InstallDir"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

!define MUI_ICON "..\res\icons\win32\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "gfx\welcome.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "gfx\header.bmp"

# Before Install:

Function .onInit
	${nsProcess::FindProcess} "apk-icon-editor.exe" $R0
	${If} $R0 == 0
		MessageBox MB_YESNO "${APPNAME} is currently running. Close?" IDYES close IDNO abort
	${Else}
		Goto continue
	${EndIf}
abort:
	MessageBox MB_OK "${APPNAME} needs to be closed before installation."
	Abort
close:
	${nsProcess::KillProcess} "apk-icon-editor.exe" $R5
continue:
FunctionEnd

# Before Uninstall:

Function un.onInit
	${nsProcess::FindProcess} "apk-icon-editor.exe" $R0
	${If} $R0 == 0
		MessageBox MB_YESNO "${APPNAME} is currently running. Close?" IDYES close IDNO abort
	${Else}
		Goto continue
	${EndIf}
abort:
	MessageBox MB_OK "${APPNAME} needs to be closed before uninstalling."
	Abort
close:
	${nsProcess::KillProcess} "apk-icon-editor.exe" $R5
continue:
FunctionEnd

# Installer Pages:

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

# Uninstaller Pages:

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

# Setup Language:

!insertmacro MUI_LANGUAGE "English"

### INSTALL ###

Section
	SetOutPath "$INSTDIR"
	File /r /x "*.manifest" "..\bin\win32\"
	SetOutPath "$INSTDIR\gfx"
	File "..\res\icons\win32\icon.ico"
	WriteUninstaller "$INSTDIR\uninstall.exe"
	SetOutPath "$INSTDIR"
	CreateShortCut "$SMPROGRAMS\${APPNAME}.lnk" ${EXE} "" "" "" "" "" "${APPNAME} v${VERSION}"
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegStr HKCU "Software\apk-icon-editor" "InstallDir" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\gfx\icon.ico$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "${COMPANYNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "${ABOUTURL}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "${UPDATEURL}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" "${VERSIONMAJOR}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" "${VERSIONMINOR}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" "$0"
SectionEnd

Section "Desktop Shortcut" SEC_DESKTOP
	SetOutPath "$INSTDIR"
	CreateShortCut "$DESKTOP\${APPNAME}.lnk" ${EXE} "" "" "" "" "" "${APPNAME} v${VERSION}"
SectionEnd

Section "Associate APK" SEC_ASSOCIATE
	WriteRegStr HKCU "Software\Classes\apk-icon-editor.apk\DefaultIcon" "" "${EXE},1"
	WriteRegStr HKCU "Software\Classes\apk-icon-editor.apk\Shell\Open\Command" "" "${EXE} $\"%1$\""
	WriteRegStr HKCU "Software\Classes\.apk" "" "apk-icon-editor.apk"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DESKTOP} "Create $\"${APPNAME}$\" desktop shortcut."
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ASSOCIATE} "Associate APK files with $\"${APPNAME}$\"."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

### UNINSTALL ###

Section "-un.Uninstall"
	Delete "${EXE}"
	Delete "$INSTDIR\apk-icon-editor.visualelementsmanifest.xml"
	Delete "$INSTDIR\authors.txt"
	Delete "$INSTDIR\faq.txt"
	Delete "$INSTDIR\versions.txt"
	Delete "$INSTDIR\Qt5Core.dll"
	Delete "$INSTDIR\Qt5Gui.dll"
	Delete "$INSTDIR\Qt5Network.dll"
	Delete "$INSTDIR\Qt5Widgets.dll"
	Delete "$INSTDIR\Qt5WinExtras.dll"
	Delete "$INSTDIR\Qt5Xml.dll"
	Delete "$INSTDIR\libGLESv2.dll"
	Delete "$INSTDIR\libEGL.dll"
	Delete "$INSTDIR\icudt51.dll"
	Delete "$INSTDIR\icuin51.dll"
	Delete "$INSTDIR\icuuc51.dll"
	Delete "$INSTDIR\libeay32.dll"
	Delete "$INSTDIR\msvcp100.dll"
	Delete "$INSTDIR\msvcr100.dll"
	Delete "$INSTDIR\ssleay32.dll"
	Delete "$INSTDIR\aapt.exe"
	Delete "$INSTDIR\zipalign.exe"
	Delete "$INSTDIR\apktool.jar"
	Delete "$INSTDIR\gfx\icon.ico"
	Delete "$INSTDIR\gfx\tiles\70x70.png"
	Delete "$INSTDIR\gfx\tiles\150x150.png"
	Delete "$INSTDIR\imageformats\qgif.dll"
	Delete "$INSTDIR\imageformats\qico.dll"
	Delete "$INSTDIR\imageformats\qjpeg.dll"
	Delete "$INSTDIR\lang\apk-icon-editor.de.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.el.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.es.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.fr.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.hu.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.it.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.nl.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.pt.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.ro.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.ru.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.tr.qm"
	Delete "$INSTDIR\lang\apk-icon-editor.zh.qm"
	Delete "$INSTDIR\lang\qt.de.qm"
	Delete "$INSTDIR\lang\qt.es.qm"
	Delete "$INSTDIR\lang\qt.fr.qm"
	Delete "$INSTDIR\lang\qt.hu.qm"
	Delete "$INSTDIR\lang\qt.it.qm"
	Delete "$INSTDIR\lang\qt.pt.qm"
	Delete "$INSTDIR\lang\qt.ru.qm"
	Delete "$INSTDIR\lang\qt.zh.qm"
	Delete "$INSTDIR\lang\flag.de.png"
	Delete "$INSTDIR\lang\flag.el.png"
	Delete "$INSTDIR\lang\flag.en.png"
	Delete "$INSTDIR\lang\flag.es.png"
	Delete "$INSTDIR\lang\flag.fr.png"
	Delete "$INSTDIR\lang\flag.hu.png"
	Delete "$INSTDIR\lang\flag.it.png"
	Delete "$INSTDIR\lang\flag.nl.png"
	Delete "$INSTDIR\lang\flag.pt.png"
	Delete "$INSTDIR\lang\flag.ro.png"
	Delete "$INSTDIR\lang\flag.ru.png"
	Delete "$INSTDIR\lang\flag.tr.png"
	Delete "$INSTDIR\lang\flag.zh.png"
	Delete "$INSTDIR\licenses\aapt.txt"
	Delete "$INSTDIR\licenses\android.txt"
	Delete "$INSTDIR\licenses\apk-icon-editor.txt"
	Delete "$INSTDIR\licenses\apktool.txt"
	Delete "$INSTDIR\licenses\opensans.txt"
	Delete "$INSTDIR\licenses\quazip.txt"
	Delete "$INSTDIR\licenses\simplecrypt.txt"
	Delete "$INSTDIR\licenses\zipalign.txt"
	Delete "$INSTDIR\platforms\qwindows.dll"
	Delete "$INSTDIR\signer\certificate.pem"
	Delete "$INSTDIR\signer\key.pk8"
	Delete "$INSTDIR\signer\signapk.jar"
	Delete "$INSTDIR\uninstall.exe"
	# Previous versions
	Delete "$INSTDIR\7za.exe"
	Delete "$INSTDIR\licenses\7za.txt"
	Delete "$PROFILE\apktool\framework\1.apk"
	RMDir "$PROFILE\apktool\framework"
	RMDir "$PROFILE\apktool"
	# Previous versions END
	RMDir "$INSTDIR\gfx\tiles"
	RMDir "$INSTDIR\gfx"
	RMDir "$INSTDIR\imageformats"
	RMDir "$INSTDIR\lang"
	RMDir "$INSTDIR\licenses"
	RMDir "$INSTDIR\platforms"
	RMDir "$INSTDIR\signer"
	RMDir "$INSTDIR"
	Delete "$DESKTOP\${APPNAME}.lnk"
	Delete "$SMPROGRAMS\${APPNAME}.lnk"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKCU "Software\apk-icon-editor"
SectionEnd

Section "un.Remove settings" SEC_DELETE
	RMDir /r "$APPDATA\apk-icon-editor"
	RMDir /r "$TEMP\apk-icon-editor"
	DeleteRegKey HKCU "Software\Classes\apk-icon-editor.apk"
	DeleteRegKey HKCU "Software\Classes\.apk"
SectionEnd

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DELETE} "Remove $\"${APPNAME}$\" configuration files, user data and temporary information."
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END
