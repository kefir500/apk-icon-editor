#!/usr/bin/env python3

import os
import sys
from urllib.request import urlretrieve
from zipfile import ZipFile

def unzip(z, src, dst):
    dst = os.path.join(dst, os.path.basename(src))
    with open(dst, 'wb') as f:
        f.write(z.read(src))

def pathTo(path):
    return os.path.join(os.path.dirname(__file__), path)

def binDir():
    if sys.platform == 'win32':
        return pathTo('win32')
    elif sys.platform == 'linux':
        return pathTo('linux')
    elif sys.platform == 'darwin':
        return pathTo('macosx/MacOS')

def binFile(path):
    return path if sys.platform != 'win32' else path + '.exe'

def progress(blocknum, blocksize, totalsize):
    bytesdone = min(blocknum * blocksize, totalsize)
    percent = bytesdone * 1e2 / totalsize
    s = '\r%5.1f%% %*d / %d KB' % (percent, len(str(totalsize)), bytesdone / 1024, totalsize / 1024)
    sys.stderr.write(s)
    if bytesdone >= totalsize:
        sys.stderr.write('\n')

# Prepare

os.makedirs(binDir(), exist_ok=True)

# Download Apktool

print('(1/2) Downloading Apktool...')
urlretrieve('https://bitbucket.org/iBotPeaches/apktool/downloads/apktool_2.4.1.jar', pathTo('general/apktool.jar'), progress)

# Download and unpack Android Build Tools

print('(2/2) Downloading Android Build Tools...')
buildToolsUrl = 'https://dl.google.com/android/repository/build-tools_r28-{}.zip'
if sys.platform == 'win32':
    buildToolsUrl = buildToolsUrl.format('windows')
elif sys.platform == 'linux':
    buildToolsUrl = buildToolsUrl.format('linux')
elif sys.platform == 'darwin':
    buildToolsUrl = buildToolsUrl.format('macosx')
urlretrieve(buildToolsUrl, 'build-tools.zip', progress)

with ZipFile('build-tools.zip') as z:
    unzip(z, 'android-9/lib/apksigner.jar', pathTo('general/signer'))
    unzip(z, binFile('android-9/zipalign'), binDir())
os.remove('build-tools.zip')
