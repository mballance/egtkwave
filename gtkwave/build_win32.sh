#!/bin/sh

cwd=`pwd`

gtkbundle=$cwd/gtkbundle
pkgs=$cwd/pkgs
inst=$cwd/inst
final_inst=$cwd/gtkwave_win32

rm -rf $inst
mkdir -p $inst

gtkbundle=$inst

mkdir -p $gtkbundle
cd $gtkbundle
unzip -o $pkgs/gtk+-bundle_2.24.10-20120208_win32.zip
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/gtk-2.0"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/glib-2.0"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/lib/glib-2.0/include"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/lib/gtk-2.0/include"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/cairo"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/pango-1.0"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/gtk-2.0/include"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/gdk-pixbuf-2.0"
GTK_CFLAGS="$GTK_CFLAGS -I$gtkbundle/include/atk-1.0 -mms-bitfields"
export GTK_CFLAGS

GTK_LIBS="$GTK_LIBS -L$gtkbundle/lib"
GTK_LIBS="$GTK_LIBS -lgtk-win32-2.0.dll"
GTK_LIBS="$GTK_LIBS -lglib-2.0.dll"
GTK_LIBS="$GTK_LIBS -lgio-2.0.dll"
GTK_LIBS="$GTK_LIBS -lgobject-2.0.dll"
GTK_LIBS="$GTK_LIBS -lgdk-win32-2.0.dll"
GTK_LIBS="$GTK_LIBS -lgdk_pixbuf-2.0.dll"
GTK_LIBS="$GTK_LIBS -lpangocairo-1.0.dll"
GTK_LIBS="$GTK_LIBS -lpangowin32-1.0.dll"
GTK_LIBS="$GTK_LIBS -lpangoft2-1.0.dll"
GTK_LIBS="$GTK_LIBS -lpango-1.0.dll"
GTK_LIBS="$GTK_LIBS -lcairo-gobject.dll"
GTK_LIBS="$GTK_LIBS -lcairo.dll"
GTK_LIBS="$GTK_LIBS -lintl.dll"
export GTK_LIBS

cd $cwd
tcldir=tcl8.5.15
if test ! -d $tcldir; then
  tar xvf $pkgs/${tcldir}-src.tar.gz
fi

rm -rf ${tcldir}_build_win32
mkdir ${tcldir}_build_win32
cd ${tcldir}_build_win32

../${tcldir}/win/configure --prefix=$inst --enable-threads
if test $? -ne 0; then
  exit 1
fi

make 
if test $? -ne 0; then
  echo "Trying again"
  make
  if test $? -ne 0; then
    exit 1
  fi
fi

make install
if test $? -ne 0; then
  echo "Trying link again"
  make install
  if test $? -ne 0; then
    exit 1
  fi
fi

cd $cwd
tkdir=tk8.5.15
if test ! -d $tkdir; then
  tar xvf $pkgs/${tkdir}-src.tar.gz
fi
rm -rf ${tkdir}_build_win32
mkdir ${tkdir}_build_win32
cd ${tkdir}_build_win32

../${tkdir}/win/configure --prefix=$inst --with-tcl=$inst/lib
if test $? -ne 0; then
  exit 1
fi

make
if test $? -ne 0; then
  exit 1
fi

make install
if test $? -ne 0; then
  exit 1
fi

cd $cwd
rm -rf xz-5.0.5
#tar xvf $pkgs/xz-5.0.5.tar.gz
#cd xz-5.0.5

#./configure --prefix=$inst 
#if test $? -ne 0; then
#  exit 1
#fi

#make
#if test $? -ne 0; then
#  exit 1
#fi

#make install
#if test $? -ne 0; then
#  exit 1
#fi

cd $cwd
gtkwave_dir=gtkwave-3.3.52

rm -rf ${gtkwave_dir}_build_win32
mkdir ${gtkwave_dir}_build_win32
cd ${gtkwave_dir}_build_win32

../${gtkwave_dir}/configure --prefix=$inst --disable-xz
if test $? -ne 0; then
  exit 1
fi

make
if test $? -ne 0; then
  exit 1
fi

make install
if test $? -ne 0; then
  exit 1
fi

rm -rf $final_inst
mkdir -p $final_inst

mkdir $final_inst/bin
cp /bin/pthreadGC2.dll $final_inst/bin
cp $inst/bin/gtkwave.exe $final_inst/bin
cp $inst/bin/*.dll $final_inst/bin

mkdir -p $final_inst/lib/gtk-2.0/2.10.0
cp -r $inst/lib/gtk-2.0/2.10.0/engines $final_inst/lib/gtk-2.0/2.10.0

cp -r $inst/lib/tcl8.5 $final_inst/lib
cp -r $inst/lib/tk8.5 $final_inst/lib

cp -r $inst/manifest $final_inst

mkdir $final_inst/share
cp -r $inst/share/themes $final_inst/share

cd $final_inst/bin
strip *.exe *.dll


