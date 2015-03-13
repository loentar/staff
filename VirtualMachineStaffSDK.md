## Virtual Machine ##

This virtual machine includes complete Staff SDK:
  * Axis2/C 1.6.0 with modules and samples;
  * Staff-2.0.0-tp4-[r374](https://code.google.com/p/staff/source/detail?r=374) +samples;
  * QT-Creator 2.0.1;
  * Sun Wireless Toolkit 2.5.2 (WTK);
  * Eclipse with WTP and CLDC support to develop J2ME applications.

Virtual machine was created in VMWare Player 3.1.2.

Guest OS is Ubuntu 10.10. [Screenshot](http://code.google.com/p/staff/wiki/Screenshots#VM_Staff_SDK)


## Download ##

Virtual Machine has size 1,8Gb (1 925 099 907 bytes).

Sha-1 sum: **e1d53678fc83e2be94c19ff744bedf6fed4d420c**

Please select download location:
  * [Direct FTP Mirror](ftp://95.83.100.210/upload/Ubuntu-staff-sdk-r374.7z) _Internet Speed ~2Mbit_
  * [Yandex](http://narod.ru/disk/1872669001/Ubuntu-staff-sdk-r374.7z.html) _fast, no ads, no wait, not splitted, easy captcha_
  * [Badongo](http://www.badongo.com/file/24853104) _ads, wait, splitted_
  * [our site](http://s-projects.ru/upload/download.php?f=Ubuntu-staff-sdk-r374.7z) _slow, direct link_

## Installation directions ##

  1. Download and install VMWare player 3.1.2 or above.
  1. Unpack downloaded Ubuntu-staff-sdk-[r374](https://code.google.com/p/staff/source/detail?r=374).7z using 7z archiver to any place.
  1. Start `Ubuntu.vmx` from `Ubuntu-staff-sdk` directory.
  1. When virtual machine is booted, please read the "README" file, located on the Ubuntu's desktop.


## How to update ##

To update Staff under this virtual machine, please start terminal and enter:
```
# go to staff source code direcotry
cd ~/devel/staff
# update source code from svn
svn up
# recompile the source code
make distclean && make -j8
# install the updated version
sudo -E make install
```


## Troubleshooting ##

If You run into errors while starting Axis2/C after updating Staff, please try to update ld.so.cache by entering:
```
sudo ldconfig
```

If errors is still remain, try to clean Staff's installation directory up and reinstall Staff:
```
# remove staff installation directory
sudo rm -Rf /usr/local/staff
# go to staff source code direcotry
cd ~/devel/staff
# recompile the source code, and reinstall staff
make distclean && make -j8 && sudo -E make install
# update ld.so.cache
sudo ldconfig
```

If You get some other troubles or have a questions, You can always ask in SupportPage.
